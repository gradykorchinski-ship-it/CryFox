/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/String.h>
#include <LibAuth/LocalAuthManager.h>
#include <UI/Qt/LocalAuthDialog.h>
#include <UI/Qt/StringUtils.h>

namespace CryFox {

LocalAuthDialog::LocalAuthDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("CryFox Security");
    setFixedWidth(360);
    setup_ui();
}

void LocalAuthDialog::setup_ui()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(24, 24, 24, 24);
    m_layout->setSpacing(16);

    m_title_label = new QLabel(this);
    m_title_label->setStyleSheet("font-size: 18px; font-weight: bold; color: #e0e0e0;");
    m_layout->addWidget(m_title_label);

    m_message_label = new QLabel(this);
    m_message_label->setWordWrap(true);
    m_message_label->setStyleSheet("font-size: 13px; color: #b0b0b0;");
    m_layout->addWidget(m_message_label);

    m_password_input = new QLineEdit(this);
    m_password_input->setEchoMode(QLineEdit::Password);
    m_password_input->setPlaceholderText("Master Password");
    m_password_input->setStyleSheet("padding: 10px; border-radius: 6px; background: #2a2a2a; color: white; border: 1px solid #3a3a3a;");
    m_layout->addWidget(m_password_input);

    m_confirm_input = new QLineEdit(this);
    m_confirm_input->setEchoMode(QLineEdit::Password);
    m_confirm_input->setPlaceholderText("Confirm Password");
    m_confirm_input->setStyleSheet("padding: 10px; border-radius: 6px; background: #2a2a2a; color: white; border: 1px solid #3a3a3a;");
    m_layout->addWidget(m_confirm_input);

    m_error_label = new QLabel(this);
    m_error_label->setStyleSheet("color: #ef4444; font-size: 12px;");
    m_error_label->hide();
    m_layout->addWidget(m_error_label);

    m_submit_button = new QPushButton(this);
    m_submit_button->setStyleSheet(R"(
        QPushButton {
            background-color: #6366f1;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #4f46e5;
        }
    )");
    connect(m_submit_button, &QPushButton::clicked, this, &LocalAuthDialog::handle_submit);
    m_layout->addWidget(m_submit_button);

    update_ui();
}

void LocalAuthDialog::update_ui()
{
    if (Auth::LocalAuthManager::the().is_setup()) {
        m_title_label->setText("Unlock CryFox");
        m_message_label->setText("Enter your master password to unlock your vault and settings.");
        m_confirm_input->hide();
        m_submit_button->setText("Unlock");
    } else {
        m_title_label->setText("Setup Master Password");
        m_message_label->setText("Create a master password to secure your local data. This password never leaves your device.");
        m_confirm_input->show();
        m_submit_button->setText("Setup");
    }
}

void LocalAuthDialog::handle_submit()
{
    auto password = ak_string_from_qstring(m_password_input->text());

    if (Auth::LocalAuthManager::the().is_setup()) {
        auto result = Auth::LocalAuthManager::the().verify_master_password(password);
        if (!result.is_error() && result.value()) {
            emit authenticated();
            accept();
        } else {
            m_error_label->setText("Incorrect master password.");
            m_error_label->show();
        }
    } else {
        auto confirm = ak_string_from_qstring(m_confirm_input->text());
        if (password != confirm) {
            m_error_label->setText("Passwords do not match.");
            m_error_label->show();
            return;
        }

        if (password.is_empty()) {
            m_error_label->setText("Password cannot be empty.");
            m_error_label->show();
            return;
        }

        auto result = Auth::LocalAuthManager::the().setup_master_password(password);
        if (result.is_error()) {
            m_error_label->setText(qstring_from_ak_string(result.error().to_string()));
            m_error_label->show();
        } else {
            // Automatically authenticate after setup
            (void)Auth::LocalAuthManager::the().verify_master_password(password);
            emit authenticated();
            accept();
        }
    }
}

}
