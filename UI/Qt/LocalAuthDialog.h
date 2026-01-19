/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace CryFox {

class LocalAuthDialog : public QDialog {
    Q_OBJECT

public:
    explicit LocalAuthDialog(QWidget* parent = nullptr);

signals:
    void authenticated();

private slots:
    void handle_submit();

private:
    void setup_ui();
    void update_ui();

    QVBoxLayout* m_layout { nullptr };
    QLabel* m_title_label { nullptr };
    QLabel* m_message_label { nullptr };
    QLineEdit* m_password_input { nullptr };
    QLineEdit* m_confirm_input { nullptr };
    QPushButton* m_submit_button { nullptr };
    QLabel* m_error_label { nullptr };
};

}
