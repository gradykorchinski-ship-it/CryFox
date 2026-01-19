/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/ByteString.h>
#include <AK/Error.h>
#include <AK/Optional.h>
#include <AK/String.h>
#include <LibAuth/Forward.h>

namespace Auth {

class LocalAuthManager {
public:
    static LocalAuthManager& the();

    bool is_setup() const;
    ErrorOr<void> setup_master_password(String const& password);
    ErrorOr<bool> verify_master_password(String const& password);

    bool is_authenticated() const { return m_authenticated; }
    void sign_out();

    String const& session_key() const { return m_session_key; }

private:
    LocalAuthManager();
    ~LocalAuthManager();

    ErrorOr<void> load_config();
    ErrorOr<void> save_config();

    bool m_authenticated { false };
    String m_password_hash;
    String m_salt;
    String m_session_key; // Derived from password, used for vault encryption

    bool m_initialized { false };
};

}
