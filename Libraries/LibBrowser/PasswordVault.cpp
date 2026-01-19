/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Base64.h>
#include <AK/StringBuilder.h>
#include <LibAuth/LocalAuthManager.h>
#include <LibBrowser/PasswordVault.h>
#include <LibCore/System.h>
#include <LibCrypto/Cipher/AES.h>
#include <LibCrypto/Random.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <time.h>

namespace Browser {

static PasswordVault* s_the = nullptr;

PasswordVault& PasswordVault::the()
{
    if (!s_the)
        s_the = new PasswordVault();
    return *s_the;
}

PasswordVault::PasswordVault()
{
}

PasswordVault::~PasswordVault()
{
}

ErrorOr<String> PasswordVault::get_database_path()
{
    char const* home_env = ::getenv("HOME");
    if (!home_env)
        return Error::from_string_literal("HOME environment variable not set");

    auto config_dir = TRY(String::formatted("{}/.config/cryfox", home_env));
    return String::formatted("{}/passwords.db", config_dir);
}

ErrorOr<void> PasswordVault::initialize()
{
    if (m_initialized)
        return {};

    m_database_path = TRY(get_database_path());
    TRY(create_tables());

    m_initialized = true;
    return {};
}

ErrorOr<void> PasswordVault::create_tables()
{
    sqlite3* db;
    int rc = sqlite3_open(m_database_path.to_byte_string().characters(), &db);

    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return Error::from_string_literal("Failed to open password database");
    }

    char const* sql = R"(
        CREATE TABLE IF NOT EXISTS passwords (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            url TEXT NOT NULL,
            username TEXT,
            encrypted_password TEXT NOT NULL,
            last_modified INTEGER NOT NULL
        );
        CREATE INDEX IF NOT EXISTS idx_pw_url ON passwords(url);
    )";

    char* err_msg = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return Error::from_string_literal("Failed to create password tables");
    }

    sqlite3_close(db);
    return {};
}

ErrorOr<String> PasswordVault::encrypt_password(String const& password)
{
    if (!Auth::LocalAuthManager::the().is_authenticated())
        return Error::from_string_literal("Not authenticated");

    auto master_key_base64 = Auth::LocalAuthManager::the().session_key();
    auto master_key = TRY(decode_base64(master_key_base64));

    // IV (12 bytes)
    u8 iv_bytes[12];
    Crypto::fill_with_random(iv_bytes, sizeof(iv_bytes));

    Crypto::Cipher::AESGCMCipher cipher(master_key.bytes());
    auto encrypted = TRY(cipher.encrypt(password.bytes(), { iv_bytes, 12 }, {}, 16));

    // Store as IV (12) + Tag (16) + Ciphertext
    ByteBuffer blob = TRY(ByteBuffer::create_uninitialized(12 + 16 + encrypted.ciphertext.size()));
    memcpy(blob.data(), iv_bytes, 12);
    memcpy(blob.data() + 12, encrypted.tag.data(), 16);
    memcpy(blob.data() + 12 + 16, encrypted.ciphertext.data(), encrypted.ciphertext.size());

    return encode_base64(blob.bytes());
}

ErrorOr<String> PasswordVault::decrypt_password(String const& encrypted_password_base64)
{
    if (!Auth::LocalAuthManager::the().is_authenticated())
        return Error::from_string_literal("Not authenticated");

    auto master_key_base64 = Auth::LocalAuthManager::the().session_key();
    auto master_key = TRY(decode_base64(master_key_base64));

    auto decoded = TRY(decode_base64(encrypted_password_base64));
    if (decoded.size() < 12 + 16)
        return Error::from_string_literal("Invalid encrypted password data");

    ReadonlyBytes iv = decoded.bytes().slice(0, 12);
    ReadonlyBytes tag = decoded.bytes().slice(12, 16);
    ReadonlyBytes ciphertext = decoded.bytes().slice(28);

    Crypto::Cipher::AESGCMCipher cipher(master_key.bytes());
    auto plaintext = TRY(cipher.decrypt(ciphertext, iv, {}, tag));

    return String::from_utf8(StringView { plaintext.data(), plaintext.size() });
}

ErrorOr<void> PasswordVault::add_password(PasswordEntry& entry)
{
    if (!m_initialized)
        TRY(initialize());

    auto encrypted = TRY(encrypt_password(entry.password));
    time_t now = time(nullptr);

    sqlite3* db;
    TRY_OR_RETURN_ERROR(sqlite3_open(m_database_path.to_byte_string().characters(), &db), Error::from_string_literal("Open failed"));

    char const* sql = "INSERT INTO passwords (url, username, encrypted_password, last_modified) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, entry.url.to_byte_string().characters(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, entry.username.to_byte_string().characters(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, encrypted.to_byte_string().characters(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, now);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        entry.id = static_cast<int>(sqlite3_last_insert_rowid(db));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return rc == SQLITE_DONE ? ErrorOr<void>({}) : Error::from_string_literal("Insert failed");
}

ErrorOr<Vector<PasswordEntry>> PasswordVault::get_passwords()
{
    if (!m_initialized)
        TRY(initialize());
    if (!Auth::LocalAuthManager::the().is_authenticated())
        return Error::from_string_literal("Not unlocked");

    Vector<PasswordEntry> entries;
    sqlite3* db;
    sqlite3_open(m_database_path.to_byte_string().characters(), &db);

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT id, url, username, encrypted_password, last_modified FROM passwords ORDER BY url ASC", -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        PasswordEntry entry;
        entry.id = sqlite3_column_int(stmt, 0);
        entry.url = MUST(String::from_utf8(StringView { reinterpret_cast<char const*>(sqlite3_column_text(stmt, 1)), strlen(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 1))) }));
        entry.username = MUST(String::from_utf8(StringView { reinterpret_cast<char const*>(sqlite3_column_text(stmt, 2)), strlen(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 2))) }));

        auto encrypted = StringView { reinterpret_cast<char const*>(sqlite3_column_text(stmt, 3)), strlen(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 3))) };
        auto decrypted = decrypt_password(MUST(String::from_utf8(encrypted)));
        if (!decrypted.is_error())
            entry.password = decrypted.release_value();

        entry.last_modified = sqlite3_column_int64(stmt, 4);
        entries.append(entry);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return entries;
}

ErrorOr<void> PasswordVault::delete_password(int id)
{
    sqlite3* db;
    sqlite3_open(m_database_path.to_byte_string().characters(), &db);
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "DELETE FROM passwords WHERE id = ?", -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return {};
}

}
