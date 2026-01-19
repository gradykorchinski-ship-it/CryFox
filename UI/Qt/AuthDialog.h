/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Function.h>
#include <AK/NonnullOwnPtr.h>
#include <LibAuth/SupabaseClient.h>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace CryFox {

class AuthDialog : public QDialog {
    Q_OBJECT

public:
    enum class Mode {
        SignIn,
        SignUp
    };

    explicit AuthDialog(QWidget* parent = nullptr, Mode mode = Mode::SignIn);

    void set_supabase_client(Auth::SupabaseClient* client) { m_supabase_client = client; }

signals:
    void authentication_successful(Auth::AuthSession session);
    void authentication_failed(QString error_message);

private slots:
    void handle_submit();
    void toggle_mode();
    void handle_forgot_password();

private:
    void setup_ui();
    void update_ui_for_mode();

    Mode m_mode;
    Auth::SupabaseClient* m_supabase_client { nullptr };

    QVBoxLayout* m_layout { nullptr };
    QLabel* m_title_label { nullptr };
    QLineEdit* m_email_input { nullptr };
    QLineEdit* m_password_input { nullptr };
    QPushButton* m_submit_button { nullptr };
    QPushButton* m_toggle_mode_button { nullptr };
    QPushButton* m_forgot_password_button { nullptr };
    QLabel* m_error_label { nullptr };
};

}
