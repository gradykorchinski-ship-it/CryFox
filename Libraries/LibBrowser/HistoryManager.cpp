/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/JsonArray.h>
#include <AK/JsonObject.h>
#include <AK/LexicalPath.h>
#include <LibBrowser/HistoryManager.h>
#include <LibCore/File.h>
#include <LibCore/System.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <time.h>

namespace Browser {

static HistoryManager* s_the = nullptr;

HistoryManager& HistoryManager::the()
{
    if (!s_the)
        s_the = new HistoryManager();
    return *s_the;
}

HistoryManager::HistoryManager()
{
}

HistoryManager::~HistoryManager()
{
}

ErrorOr<String> HistoryManager::get_database_path()
{
    char const* home_env = ::getenv("HOME");
    if (!home_env)
        return Error::from_string_literal("HOME environment variable not set");

    auto home = TRY(String::from_utf8(StringView { home_env, strlen(home_env) }));
    auto config_dir = TRY(String::formatted("{}/.config/cryfox", home));

    // Create directory if it doesn't exist
    auto result = Core::System::mkdir(config_dir.to_byte_string(), 0700);
    if (result.is_error() && result.error().code() != EEXIST)
        return result.release_error();

    return String::formatted("{}/history.db", config_dir);
}

ErrorOr<void> HistoryManager::initialize()
{
    if (m_initialized)
        return {};

    m_database_path = TRY(get_database_path());
    TRY(create_tables());

    m_initialized = true;
    return {};
}

ErrorOr<void> HistoryManager::create_tables()
{
    sqlite3* db;
    int rc = sqlite3_open(m_database_path.to_byte_string().characters(), &db);

    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return Error::from_string_literal("Failed to open history database");
    }

    char const* sql = R"(
        CREATE TABLE IF NOT EXISTS history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            url TEXT NOT NULL,
            title TEXT,
            domain TEXT,
            visit_time INTEGER NOT NULL
        );
        
        CREATE INDEX IF NOT EXISTS idx_url ON history(url);
        CREATE INDEX IF NOT EXISTS idx_domain ON history(domain);
        CREATE INDEX IF NOT EXISTS idx_visit_time ON history(visit_time DESC);
    )";

    char* err_msg = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return Error::from_string_literal("Failed to create history tables");
    }

    sqlite3_close(db);
    return {};
}

ErrorOr<void> HistoryManager::add_visit(URL::URL const& url, String const& title)
{
    if (!m_initialized)
        TRY(initialize());

    // Skip certain URLs
    auto url_string = url.serialize();
    if (url_string.starts_with_bytes("about:"sv) || url_string.starts_with_bytes("cryfox:"sv))
        return {};

    // Extract domain
    String domain = ""_string;
    if (url.host().has_value())
        domain = url.serialized_host();

    sqlite3* db;
    int rc = sqlite3_open(m_database_path.to_byte_string().characters(), &db);

    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return Error::from_string_literal("Failed to open history database");
    }

    char const* sql = "INSERT INTO history (url, title, domain, visit_time) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return Error::from_string_literal("Failed to prepare insert statement");
    }

    time_t now = time(nullptr);

    sqlite3_bind_text(stmt, 1, url_string.to_byte_string().characters(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, title.to_byte_string().characters(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, domain.to_byte_string().characters(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, now);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (rc != SQLITE_DONE)
        return Error::from_string_literal("Failed to insert history entry");

    return {};
}

ErrorOr<Vector<HistorySite>> HistoryManager::get_most_visited(size_t count)
{
    if (!m_initialized)
        TRY(initialize());

    Vector<HistorySite> sites;

    sqlite3* db;
    int rc = sqlite3_open(m_database_path.to_byte_string().characters(), &db);

    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return Error::from_string_literal("Failed to open history database");
    }

    char const* sql = R"(
        SELECT url, title, domain, COUNT(*) as visit_count, MAX(visit_time) as last_visit
        FROM history
        WHERE domain != ''
        GROUP BY domain
        ORDER BY visit_count DESC, last_visit DESC
        LIMIT ?
    )";

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return Error::from_string_literal("Failed to prepare select statement");
    }

    sqlite3_bind_int(stmt, 1, static_cast<int>(count));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        HistorySite site;

        char const* url = reinterpret_cast<char const*>(sqlite3_column_text(stmt, 0));
        char const* title = reinterpret_cast<char const*>(sqlite3_column_text(stmt, 1));
        char const* domain = reinterpret_cast<char const*>(sqlite3_column_text(stmt, 2));

        site.url = url ? MUST(String::from_utf8(StringView { url, strlen(url) })) : ""_string;
        site.title = title ? MUST(String::from_utf8(StringView { title, strlen(title) })) : ""_string;
        site.domain = domain ? MUST(String::from_utf8(StringView { domain, strlen(domain) })) : ""_string;
        site.visit_count = sqlite3_column_int(stmt, 3);
        site.last_visit = sqlite3_column_int64(stmt, 4);

        TRY(sites.try_append(site));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return sites;
}

ErrorOr<String> HistoryManager::get_most_visited_json(size_t count)
{
    auto sites = TRY(get_most_visited(count));

    JsonArray array;
    for (auto const& site : sites) {
        JsonObject obj;
        obj.set("url"sv, JsonValue(site.url));
        obj.set("title"sv, JsonValue(site.title));
        obj.set("domain"sv, JsonValue(site.domain));
        obj.set("visit_count"sv, JsonValue(site.visit_count));
        obj.set("last_visit"sv, JsonValue(static_cast<u64>(site.last_visit)));
        MUST(array.append(move(obj)));
    }

    return array.serialized();
}

ErrorOr<void> HistoryManager::clear_history()
{
    if (!m_initialized)
        TRY(initialize());

    sqlite3* db;
    int rc = sqlite3_open(m_database_path.to_byte_string().characters(), &db);

    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return Error::from_string_literal("Failed to open history database");
    }

    char const* sql = "DELETE FROM history";
    char* err_msg = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return Error::from_string_literal("Failed to clear history");
    }

    sqlite3_close(db);
    return {};
}

}
