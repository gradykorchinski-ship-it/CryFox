/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/JsonParser.h>
#include <LibAuth/SupabaseClient.h>
#include <LibCore/File.h>
#include <curl/curl.h>

namespace Auth {

// Callback for libcurl to write response data
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    auto* str = static_cast<String*>(userp);
    size_t total_size = size * nmemb;
    ByteBuffer buffer;
    buffer.append(static_cast<char const*>(contents), total_size);
    *str = MUST(String::from_utf8(StringView { static_cast<char const*>(contents), total_size }));
    return total_size;
}

SupabaseClient::SupabaseClient(String supabase_url, String supabase_key)
    : m_supabase_url(move(supabase_url))
    , m_supabase_key(move(supabase_key))
{
}

ErrorOr<NonnullOwnPtr<SupabaseClient>> SupabaseClient::create(String const& supabase_url, String const& supabase_key)
{
    return adopt_nonnull_own_or_enomem(new (nothrow) SupabaseClient(supabase_url, supabase_key));
}

ErrorOr<JsonObject> SupabaseClient::make_request(ByteString const& endpoint, JsonObject const& body, Optional<String> const& auth_token)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return Error::from_string_literal("Failed to initialize CURL");

    ScopeGuard cleanup = [&] { curl_easy_cleanup(curl); };

    ByteString url = TRY(String::formatted("{}/auth/v1{}", m_supabase_url, endpoint)).to_byte_string();
    ByteString response_data;
    ByteString request_body = body.to_byte_string();

    // Set up headers
    struct curl_slist* headers = nullptr;
    ScopeGuard headers_cleanup = [&] { curl_slist_free_all(headers); };

    headers = curl_slist_append(headers, "Content-Type: application/json");
    ByteString api_key_header = TRY(String::formatted("apikey: {}", m_supabase_key)).to_byte_string();
    headers = curl_slist_append(headers, api_key_header.characters());

    if (auth_token.has_value()) {
        ByteString auth_header = TRY(String::formatted("Authorization: Bearer {}", auth_token.value())).to_byte_string();
        headers = curl_slist_append(headers, auth_header.characters());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.characters());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.characters());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        return Error::from_string_literal(curl_easy_strerror(res));

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    auto json_result = TRY(JsonValue::from_string(response_data));
    if (!json_result.is_object())
        return Error::from_string_literal("Invalid JSON response from Supabase");

    auto json_object = json_result.as_object();

    if (http_code >= 400) {
        auto error_msg = json_object.get_byte_string("error_description"sv);
        if (!error_msg.has_value())
            error_msg = json_object.get_byte_string("msg"sv);
        if (error_msg.has_value())
            return Error::from_string_view(error_msg.value());
        return Error::from_string_literal("Unknown error from Supabase API");
    }

    return json_object;
}

ErrorOr<AuthResponse> SupabaseClient::sign_up(String const& email, String const& password)
{
    JsonObject body;
    body.set("email", email.to_byte_string());
    body.set("password", password.to_byte_string());

    auto response = TRY(make_request("/signup"_string, body));

    AuthResponse auth_response;
    if (response.has("access_token"sv)) {
        auth_response.success = true;
        auth_response.session.access_token = TRY(String::from_byte_string(response.get_byte_string("access_token"sv).value()));
        auth_response.session.refresh_token = TRY(String::from_byte_string(response.get_byte_string("refresh_token"sv).value()));

        auto user = response.get_object("user"sv);
        if (user.has_value()) {
            auth_response.session.user_id = TRY(String::from_byte_string(user->get_byte_string("id"sv).value()));
            auth_response.session.user_email = TRY(String::from_byte_string(user->get_byte_string("email"sv).value()));
        }
    }

    return auth_response;
}

ErrorOr<AuthResponse> SupabaseClient::sign_in(String const& email, String const& password)
{
    JsonObject body;
    body.set("email", email.to_byte_string());
    body.set("password", password.to_byte_string());

    auto response = TRY(make_request("/token?grant_type=password"_string, body));

    AuthResponse auth_response;
    if (response.has("access_token"sv)) {
        auth_response.success = true;
        auth_response.session.access_token = TRY(String::from_byte_string(response.get_byte_string("access_token"sv).value()));
        auth_response.session.refresh_token = TRY(String::from_byte_string(response.get_byte_string("refresh_token"sv).value()));

        auto user = response.get_object("user"sv);
        if (user.has_value()) {
            auth_response.session.user_id = TRY(String::from_byte_string(user->get_byte_string("id"sv).value()));
            auth_response.session.user_email = TRY(String::from_byte_string(user->get_byte_string("email"sv).value()));
        }

        if (response.has("expires_in"sv)) {
            auto expires_in = response.get_u32("expires_in"sv).value_or(3600);
            auth_response.session.expires_at = time(nullptr) + expires_in;
        }
    }

    return auth_response;
}

ErrorOr<void> SupabaseClient::sign_out(String const& access_token)
{
    JsonObject body;
    TRY(make_request("/logout"_string, body, access_token));
    return {};
}

ErrorOr<AuthResponse> SupabaseClient::refresh_session(String const& refresh_token)
{
    JsonObject body;
    body.set("refresh_token", refresh_token.to_byte_string());

    auto response = TRY(make_request("/token?grant_type=refresh_token"_string, body));

    AuthResponse auth_response;
    if (response.has("access_token"sv)) {
        auth_response.success = true;
        auth_response.session.access_token = TRY(String::from_byte_string(response.get_byte_string("access_token"sv).value()));
        auth_response.session.refresh_token = TRY(String::from_byte_string(response.get_byte_string("refresh_token"sv).value()));

        if (response.has("expires_in"sv)) {
            auto expires_in = response.get_u32("expires_in"sv).value_or(3600);
            auth_response.session.expires_at = time(nullptr) + expires_in;
        }
    }

    return auth_response;
}

ErrorOr<void> SupabaseClient::request_password_reset(String const& email)
{
    JsonObject body;
    body.set("email", email.to_byte_string());

    TRY(make_request("/recover"_string, body));
    return {};
}

}
