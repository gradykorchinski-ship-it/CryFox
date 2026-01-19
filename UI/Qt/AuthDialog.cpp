/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QFormLayout>
#include <QMessageBox>
#include <QProgressDialog>
#include <QtConcurrent>
#include <UI/Qt/AuthDialog.h>

namespace CryFox {

AuthDialog::AuthDialog(QWidget* parent, Mode mode)
    : QDialog(parent)
    , m_mode(mode)
{
    setWindowTitle("CryFox Account");
    setMinimumWidth(400);
    setup_ui();
}

void AuthDialog::setup_ui()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(16);
    m_layout->setContentsMargins(24, 24, 24, 24);

    // Title
    m_title_label = new QLabel(this);
    m_title_label->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 8px;");
    m_layout->addWidget(m_title_label);

    // Form inputs
    auto* form_layout = new QFormLayout();
    form_layout->setSpacing(12);

    m_email_input = new QLineEdit(this);
    m_email_input->setPlaceholderText("your@email.com");
    m_email_input->setMinimumHeight(36);
    form_layout->addRow("Email:", m_email_input);

    m_password_input = new QLineEdit(this);
    m_password_input->setEchoMode(QLineEdit::Password);
    m_password_input->setPlaceholderText("••••••••");
    m_password_input->setMinimumHeight(36);
    form_layout->addRow("Password:", m_password_input);

    m_layout->addLayout(form_layout);

    // Error label (initially hidden)
    m_error_label = new QLabel(this);
    m_error_label->setStyleSheet("color: #dc3545; font-size: 13px; margin-top: 8px;");
    m_error_label->setWordWrap(true);
    m_error_label->hide();
    m_layout->addWidget(m_error_label);

    // Submit button
    m_submit_button = new QPushButton(this);
    m_submit_button->setMinimumHeight(40);
    m_submit_button->setStyleSheet(
        "QPushButton {"
        "   background-color: #6366f1;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4f46e5;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #4338ca;"
        "}");
    connect(m_submit_button, &QPushButton::clicked, this, &AuthDialog::handle_submit);
    m_layout->addWidget(m_submit_button);

    // Toggle mode button
    m_toggle_mode_button = new QPushButton(this);
    m_toggle_mode_button->setFlat(true);
    m_toggle_mode_button->setStyleSheet("color: #6366f1; text-decoration: underline; border: none;");
    connect(m_toggle_mode_button, &QPushButton::clicked, this, &AuthDialog::toggle_mode);
    m_layout->addWidget(m_toggle_mode_button, 0, Qt::AlignCenter);

    // Forgot password button (only for sign in)
    m_forgot_password_button = new QPushButton("Forgot password?", this);
    m_forgot_password_button->setFlat(true);
    m_forgot_password_button->setStyleSheet("color: #6b7280; font-size: 12px; border: none;");
    connect(m_forgot_password_button, &QPushButton::clicked, this, &AuthDialog::handle_forgot_password);
    m_layout->addWidget(m_forgot_password_button, 0, Qt::AlignCenter);

    update_ui_for_mode();

    // Allow Enter key to submit
    connect(m_email_input, &QLineEdit::returnPressed, this, &AuthDialog::handle_submit);
    connect(m_password_input, &QLineEdit::returnPressed, this, &AuthDialog::handle_submit);
}

void AuthDialog::update_ui_for_mode()
{
    if (m_mode == Mode::SignIn) {
        m_title_label->setText("Sign in to CryFox");
        m_submit_button->setText("Sign In");
        m_toggle_mode_button->setText("Don't have an account? Sign up");
        m_forgot_password_button->show();
    } else {
        m_title_label->setText("Create CryFox Account");
        m_submit_button->setText("Create Account");
        m_toggle_mode_button->setText("Already have an account? Sign in");
        m_forgot_password_button->hide();
    }

    m_error_label->hide();
}

void AuthDialog::handle_submit()
{
    QString email = m_email_input->text().trimmed();
    QString password = m_password_input->text();

    // Basic validation
    if (email.isEmpty() || password.isEmpty()) {
        m_error_label->setText("Please enter both email and password.");
        m_error_label->show();
        return;
    }

    if (!email.contains('@')) {
        m_error_label->setText("Please enter a valid email address.");
        m_error_label->show();
        return;
    }

    if (m_mode == Mode::SignUp && password.length() < 6) {
        m_error_label->setText("Password must be at least 6 characters.");
        m_error_label->show();
        return;
    }

    if (!m_supabase_client) {
        m_error_label->setText("Authentication service not configured.");
        m_error_label->show();
        return;
    }

    // Show progress
    auto* progress = new QProgressDialog(m_mode == Mode::SignIn ? "Signing in..." : "Creating account...", QString(), 0, 0, this);
    progress->setWindowModality(Qt::WindowModal);
    progress->setCancelButton(nullptr);
    progress->show();

    // Disable form during authentication
    m_submit_button->setEnabled(false);
    m_email_input->setEnabled(false);
    m_password_input->setEnabled(false);

    // Perform authentication asynchronously
    auto future = QtConcurrent::run([this, email, password]() {
        if (m_mode == Mode::SignIn) {
            return m_supabase_client->sign_in(
                MUST(String::from_byte_string(email.toUtf8())),
                MUST(String::from_byte_string(password.toUtf8())));
        } else {
            return m_supabase_client->sign_up(
                MUST(String::from_byte_string(email.toUtf8())),
                MUST(String::from_byte_string(password.toUtf8())));
        }
    });

    auto* watcher = new QFutureWatcher<ErrorOr<Auth::AuthResponse>>(this);
    connect(watcher, &QFutureWatcher<ErrorOr<Auth::AuthResponse>>::finished, this, [this, progress, watcher]() {
        progress->close();
        progress->deleteLater();

        // Re-enable form
        m_submit_button->setEnabled(true);
        m_email_input->setEnabled(true);
        m_password_input->setEnabled(true);

        auto result = watcher->result();
        watcher->deleteLater();

        if (result.is_error()) {
            m_error_label->setText(QString::fromUtf8(result.error().string_literal()));
            m_error_label->show();
            emit authentication_failed(QString::fromUtf8(result.error().string_literal()));
            return;
        }

        auto response = result.release_value();
        if (!response.success) {
            m_error_label->setText(QString::fromUtf8(response.error_message.to_byte_string().characters()));
            m_error_label->show();
            emit authentication_failed(QString::fromUtf8(response.error_message.to_byte_string().characters()));
            return;
        }

        emit authentication_successful(response.session);
        accept();
    });

    watcher->setFuture(future);
}

void AuthDialog::toggle_mode()
{
    m_mode = (m_mode == Mode::SignIn) ? Mode::SignUp : Mode::SignIn;
    update_ui_for_mode();
    m_password_input->clear();
}

void AuthDialog::handle_forgot_password()
{
    QString email = m_email_input->text().trimmed();

    if (email.isEmpty() || !email.contains('@')) {
        QMessageBox::information(this, "Password Reset",
            "Please enter your email address in the email field first.");
        return;
    }

    if (!m_supabase_client) {
        QMessageBox::critical(this, "Error", "Authentication service not configured.");
        return;
    }

    auto result = m_supabase_client->request_password_reset(
        MUST(String::from_byte_string(email.toUtf8())));

    if (result.is_error()) {
        QMessageBox::critical(this, "Error",
            QString("Failed to send password reset email: %1").arg(result.error().string_literal()));
        return;
    }

    QMessageBox::information(this, "Password Reset",
        "If an account exists with this email, you will receive a password reset link shortly.");
}

}
