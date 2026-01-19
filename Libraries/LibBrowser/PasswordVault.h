/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/String.h>
#include <AK/Vector.h>
#include <LibURL/URL.h>

namespace Browser {

struct PasswordEntry {
    int id { -1 };
    String url;
    String username;
    String password;
    u64 last_modified { 0 };
};

class PasswordVault {
public:
    static PasswordVault& the();

    ErrorOr<void> initialize();

    ErrorOr<void> add_password(PasswordEntry& entry);
    ErrorOr<void> update_password(PasswordEntry const& entry);
    ErrorOr<void> delete_password(int id);

    ErrorOr<Vector<PasswordEntry>> get_passwords();
    ErrorOr<Vector<PasswordEntry>> search_passwords(String const& query);

private:
    PasswordVault();
    ~PasswordVault();

    ErrorOr<String> get_database_path();
    ErrorOr<void> create_tables();

    ErrorOr<String> encrypt_password(String const& password);
    ErrorOr<String> decrypt_password(String const& encrypted_password);

    String m_database_path;
    bool m_initialized { false };
};

}
