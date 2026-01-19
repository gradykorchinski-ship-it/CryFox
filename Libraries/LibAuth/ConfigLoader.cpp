/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/LexicalPath.h>
#include <LibAuth/ConfigLoader.h>
#include <LibCore/File.h>
#include <LibCore/System.h>
#include <stdlib.h>

namespace Auth {

ErrorOr<String> ConfigLoader::find_config_file()
{
    // Try current directory first
    if (!Core::System::stat(".supabase.config"sv).is_error())
        return ".supabase.config"_string;

    // Try home directory
    char const* home_env = ::getenv("HOME");
    if (home_env) {
        auto home_config = TRY(String::formatted("{}/.config/cryfox/.supabase.config", home_env));
        if (!Core::System::stat(home_config.bytes_as_string_view()).is_error())
            return home_config;
    }

    return Error::from_string_literal("Supabase config file not found. Please create .supabase.config");
}

ErrorOr<HashMap<String, String>> ConfigLoader::load_config(StringView config_path)
{
    auto file = TRY(Core::File::open(config_path, Core::File::OpenMode::Read));
    auto contents = TRY(file->read_until_eof());
    auto content_string = TRY(String::from_utf8(contents));

    HashMap<String, String> config;
    auto lines = content_string.bytes_as_string_view().split_view('\n');

    for (auto line : lines) {
        line = line.trim_whitespace();

        // Skip comments and empty lines
        if (line.is_empty() || line.starts_with('#'))
            continue;

        // Parse KEY=VALUE
        auto parts = line.split_view('=', SplitBehavior::KeepEmpty);
        if (parts.size() != 2)
            continue;

        auto key = TRY(String::from_utf8(parts[0].trim_whitespace()));
        auto value = TRY(String::from_utf8(parts[1].trim_whitespace()));

        TRY(config.try_set(key, value));
    }

    return config;
}

ErrorOr<String> ConfigLoader::get_supabase_url()
{
    auto config_path = TRY(find_config_file());
    auto config = TRY(load_config(config_path.bytes_as_string_view()));

    auto url = config.get("SUPABASE_URL"_string);
    if (!url.has_value() || url->is_empty())
        return Error::from_string_literal("SUPABASE_URL not configured");

    return url.value();
}

ErrorOr<String> ConfigLoader::get_supabase_key()
{
    auto config_path = TRY(find_config_file());
    auto config = TRY(load_config(config_path.bytes_as_string_view()));

    auto key = config.get("SUPABASE_ANON_KEY"_string);
    if (!key.has_value() || key->is_empty())
        return Error::from_string_literal("SUPABASE_ANON_KEY not configured");

    return key.value();
}

}
