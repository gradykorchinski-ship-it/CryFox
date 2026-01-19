/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace CryFox {

class ProfilePanel : public QDialog {
    Q_OBJECT

public:
    explicit ProfilePanel(QWidget* parent = nullptr);

    void set_authenticated(bool authenticated);
    void set_user_email(QString const& email);
    void update_panel();

signals:
    void authenticated_changed(bool authenticated);
    void sign_in_requested();
    void create_account_requested();
    void password_manager_requested();
    void settings_requested();
    void sign_out_requested();

private:
    void setup_ui();
    void show_unauthenticated_view();
    void show_authenticated_view();

    bool m_authenticated { false };
    QString m_user_email { "" };

    QVBoxLayout* m_layout { nullptr };
    QWidget* m_content_widget { nullptr };
};

}
