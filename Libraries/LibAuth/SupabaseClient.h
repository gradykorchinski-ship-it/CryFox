/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/ByteString.h>
#include <AK/Error.h>
#include <AK/JsonObject.h>
#include <AK/Result.h>
#include <AK/String.h>

namespace Auth {

struct AuthSession {
    String access_token;
    String refresh_token;
    String user_id;
    String user_email;
    u64 expires_at { 0 };
};

struct AuthResponse {
    AuthSession session;
    String error_message;
    bool success { false };
};

class SupabaseClient {
public:
    ~SupabaseClient() = default;

    static ErrorOr<NonnullOwnPtr<SupabaseClient>> create(String const& supabase_url, String const& supabase_key);

    // Authentication methods
    ErrorOr<AuthResponse> sign_up(String const& email, String const& password);
    ErrorOr<AuthResponse> sign_in(String const& email, String const& password);
    ErrorOr<void> sign_out(String const& access_token);
    ErrorOr<AuthResponse> refresh_session(String const& refresh_token);

    // Password reset
    ErrorOr<void> request_password_reset(String const& email);

    String const& supabase_url() const { return m_supabase_url; }

private:
    explicit SupabaseClient(String supabase_url, String supabase_key);

    ErrorOr<JsonObject> make_request(ByteString const& endpoint, JsonObject const& body, Optional<String> const& auth_token = {});

    String m_supabase_url;
    String m_supabase_key;
};

}
