/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Base64.h>
#include <AK/JsonObject.h>
#include <AK/JsonValue.h>
#include <AK/LexicalPath.h>
#include <LibAuth/LocalAuthManager.h>
#include <LibCore/File.h>
#include <LibCore/System.h>
#include <LibCrypto/Hash/Argon2.h>
#include <LibCrypto/Random.h>
#include <stdlib.h>
#include <string.h>

namespace Auth {

static LocalAuthManager* s_the = nullptr;

LocalAuthManager& LocalAuthManager::the()
{
    if (!s_the)
        s_the = new LocalAuthManager();
    return *s_the;
}

LocalAuthManager::LocalAuthManager()
{
    (void)load_config();
}

LocalAuthManager::~LocalAuthManager()
{
}

bool LocalAuthManager::is_setup() const
{
    return !m_password_hash.is_empty();
}

ErrorOr<void> LocalAuthManager::load_config()
{
    char const* home_env = ::getenv("HOME");
    if (!home_env)
        return Error::from_string_literal("HOME environment variable not set");

    auto config_path = TRY(String::formatted("{}/.config/cryfox/auth.json", home_env));
    auto file_or_error = Core::File::open(config_path.to_byte_string(), Core::File::OpenMode::Read);
    if (file_or_error.is_error())
        return {};

    auto file = file_or_error.release_value();
    auto buffer = TRY(file->read_until_eof());
    auto json_or_error = JsonValue::from_string(StringView { buffer.data(), buffer.size() });
    if (json_or_error.is_error())
        return {};

    auto json = json_or_error.release_value();
    if (!json.is_object())
        return {};

    auto const& obj = json.as_object();
    m_password_hash = obj.get_string("hash"sv).value_or(""_string);
    m_salt = obj.get_string("salt"sv).value_or(""_string);

    return {};
}

ErrorOr<void> LocalAuthManager::save_config()
{
    char const* home_env = ::getenv("HOME");
    if (!home_env)
        return Error::from_string_literal("HOME environment variable not set");

    auto config_dir = TRY(String::formatted("{}/.config/cryfox", home_env));
    (void)Core::System::mkdir(config_dir.to_byte_string(), 0700);

    auto config_path = TRY(String::formatted("{}/auth.json", config_dir));
    auto file = TRY(Core::File::open(config_path.to_byte_string(), Core::File::OpenMode::Write));

    JsonObject obj;
    obj.set("hash"sv, JsonValue(m_password_hash));
    obj.set("salt"sv, JsonValue(m_salt));

    auto serialized = obj.serialized();
    TRY(file->write_until_depleted(serialized.bytes()));

    return {};
}

ErrorOr<void> LocalAuthManager::setup_master_password(String const& password)
{
    // Generate salt
    u8 salt_bytes[16];
    Crypto::fill_with_random(salt_bytes, sizeof(salt_bytes));
    m_salt = encode_base64(ReadonlyBytes { salt_bytes, sizeof(salt_bytes) });

    // Derive password hash
    Crypto::Hash::Argon2 argon2(Crypto::Hash::Argon2Type::Argon2id);
    auto salt_buf = TRY(decode_base64(m_salt));

    auto hash_result = TRY(argon2.derive_key(
        password.bytes(),
        salt_buf.bytes(),
        1,     // parallelism
        65536, // memory (64MB)
        3,     // passes
        0x13,  // version (1.3)
        {},    // secret
        {},    // associated data
        32     // tag length
        ));

    m_password_hash = encode_base64(hash_result.bytes());
    TRY(save_config());

    return {};
}

ErrorOr<bool> LocalAuthManager::verify_master_password(String const& password)
{
    if (!is_setup())
        return false;

    Crypto::Hash::Argon2 argon2(Crypto::Hash::Argon2Type::Argon2id);
    auto salt_buf = TRY(decode_base64(m_salt));

    auto hash_result = TRY(argon2.derive_key(
        password.bytes(),
        salt_buf.bytes(),
        1,
        65536,
        3,
        0x13,
        {},
        {},
        32));

    auto calculated_hash = encode_base64(hash_result.bytes());
    if (calculated_hash == m_password_hash) {
        m_authenticated = true;

        // Derive session key for vault
        auto session_key_result = TRY(argon2.derive_key(
            password.bytes(),
            salt_buf.bytes(),
            1,
            65536,
            3,
            0x13,
            "vault"sv.bytes(),
            {},
            32));
        m_session_key = encode_base64(session_key_result.bytes());

        return true;
    }

    return false;
}

void LocalAuthManager::sign_out()
{
    m_authenticated = false;
    m_session_key = ""_string;
}

}
