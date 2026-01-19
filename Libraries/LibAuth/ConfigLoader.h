/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/HashMap.h>
#include <AK/String.h>

namespace Auth {

class ConfigLoader {
public:
    static ErrorOr<HashMap<String, String>> load_config(StringView config_path);

    static ErrorOr<String> get_supabase_url();
    static ErrorOr<String> get_supabase_key();

private:
    static ErrorOr<String> find_config_file();
};

}
