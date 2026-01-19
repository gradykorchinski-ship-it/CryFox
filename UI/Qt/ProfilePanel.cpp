/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibAuth/LocalAuthManager.h>
#include <QGraphicsDropShadowEffect>
#include <UI/Qt/LocalAuthDialog.h>
#include <UI/Qt/ProfilePanel.h>

namespace CryFox {

ProfilePanel::ProfilePanel(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(320);

    setup_ui();
}

void ProfilePanel::setup_ui()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    m_content_widget = new QWidget(this);
    m_content_widget->setObjectName("ProfilePanelContent");
    m_content_widget->setStyleSheet(R"(
        #ProfilePanelContent {
            background-color: #242424;
            border: 1px solid #3a3a3a;
            border-radius: 12px;
        }
        
        @media (prefers-color-scheme: light) {
            #ProfilePanelContent {
                background-color: #ffffff;
                border-color: #d0d0d0;
            }
        }
    )");

    // Add drop shadow
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(24);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 4);
    m_content_widget->setGraphicsEffect(shadow);

    m_layout->addWidget(m_content_widget);

    update_panel();
}

void ProfilePanel::set_authenticated(bool authenticated)
{
    m_authenticated = authenticated;
    update_panel();
}

void ProfilePanel::set_user_email(QString const& email)
{
    m_user_email = email;
    update_panel();
}

void ProfilePanel::update_panel()
{
    m_authenticated = Auth::LocalAuthManager::the().is_authenticated();

    // Clear existing content
    if (m_content_widget->layout()) {
        QLayoutItem* item;
        while ((item = m_content_widget->layout()->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete m_content_widget->layout();
    }

    auto* content_layout = new QVBoxLayout(m_content_widget);
    content_layout->setContentsMargins(20, 20, 20, 20);
    content_layout->setSpacing(16);

    if (m_authenticated) {
        show_authenticated_view();
    } else {
        show_unauthenticated_view();
    }
}

void ProfilePanel::show_unauthenticated_view()
{
    auto* layout = qobject_cast<QVBoxLayout*>(m_content_widget->layout());

    // Welcome message
    auto* title = new QLabel("Welcome to CryFox", m_content_widget);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #e0e0e0;");
    layout->addWidget(title);

    auto* subtitle = new QLabel("Unlock CryFox to access your passwords and settings.", m_content_widget);
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("font-size: 13px; color: #b0b0b0; margin-bottom: 8px;");
    layout->addWidget(subtitle);

    layout->addSpacing(8);

    // Unlock button
    auto* unlock_btn = new QPushButton(Auth::LocalAuthManager::the().is_setup() ? "Unlock Vault" : "Setup Passwords", m_content_widget);
    unlock_btn->setStyleSheet(R"(
        QPushButton {
            background-color: #6366f1;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 24px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #4f46e5;
        }
    )");
    connect(unlock_btn, &QPushButton::clicked, this, [this]() {
        auto* dialog = new LocalAuthDialog(this);
        connect(dialog, &LocalAuthDialog::authenticated, this, [this]() {
            update_panel();
            emit authenticated_changed(true);
        });
        dialog->show();
    });
    layout->addWidget(unlock_btn);

    // Create account button
    auto* create_btn = new QPushButton("Create Account", m_content_widget);
    create_btn->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: #6366f1;
            border: 2px solid #6366f1;
            border-radius: 8px;
            padding: 12px 24px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba(99, 102, 241, 0.1);
        }
        QPushButton:pressed {
            background-color: rgba(99, 102, 241, 0.2);
        }
    )");
    connect(create_btn, &QPushButton::clicked, this, &ProfilePanel::create_account_requested);
    layout->addWidget(create_btn);

    layout->addStretch();

    // Footer note
    auto* footer = new QLabel("Local account only (no server required)", m_content_widget);
    footer->setStyleSheet("font-size: 11px; color: #808080; font-style: italic;");
    footer->setAlignment(Qt::AlignCenter);
    layout->addWidget(footer);
}

void ProfilePanel::show_authenticated_view()
{
    auto* layout = qobject_cast<QVBoxLayout*>(m_content_widget->layout());

    // User info section
    auto* email_label = new QLabel("Master User", m_content_widget);
    email_label->setStyleSheet("font-size: 14px; font-weight: 600; color: #e0e0e0; margin-bottom: 4px;");
    layout->addWidget(email_label);

    auto* account_type = new QLabel("Local Account", m_content_widget);
    account_type->setStyleSheet("font-size: 12px; color: #808080;");
    layout->addWidget(account_type);

    // Separator
    auto* separator1 = new QWidget(m_content_widget);
    separator1->setFixedHeight(1);
    separator1->setStyleSheet("background-color: #3a3a3a; margin: 12px 0;");
    layout->addWidget(separator1);

    // Quick actions
    auto create_action_button = [this, layout](QString const& text, auto signal) {
        auto* btn = new QPushButton(text, m_content_widget);
        btn->setStyleSheet(R"(
            QPushButton {
                background-color: transparent;
                color: #e0e0e0;
                border: none;
                border-radius: 6px;
                padding: 10px 12px;
                font-size: 14px;
                text-align: left;
            }
            QPushButton:hover {
                background-color: #2a2a2a;
            }
        )");
        connect(btn, &QPushButton::clicked, this, signal);
        layout->addWidget(btn);
    };

    create_action_button("🔐 Password Manager", &ProfilePanel::password_manager_requested);
    create_action_button("⚙️  Settings", &ProfilePanel::settings_requested);

    // Separator
    auto* separator2 = new QWidget(m_content_widget);
    separator2->setFixedHeight(1);
    separator2->setStyleSheet("background-color: #3a3a3a; margin: 12px 0;");
    layout->addWidget(separator2);

    // Sign out button
    auto* sign_out_btn = new QPushButton("Lock Vault", m_content_widget);
    sign_out_btn->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: #ef4444;
            border: none;
            border-radius: 6px;
            padding: 10px 12px;
            font-size: 14px;
            text-align: left;
        }
        QPushButton:hover {
            background-color: rgba(239, 68, 68, 0.1);
        }
    )");
    connect(sign_out_btn, &QPushButton::clicked, this, [this]() {
        Auth::LocalAuthManager::the().sign_out();
        update_panel();
        emit authenticated_changed(false);
        emit sign_out_requested();
    });
    layout->addWidget(sign_out_btn);

    layout->addStretch();
}

}
