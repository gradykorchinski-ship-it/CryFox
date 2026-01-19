/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <QToolButton>
#include <QWidget>

namespace CryFox {

class ProfileButton : public QToolButton {
    Q_OBJECT

public:
    explicit ProfileButton(QWidget* parent = nullptr);

    void set_authenticated(bool authenticated);
    void set_user_initials(QString const& initials);
    void update_appearance();

signals:
    void clicked_signal();

private:
    void paintEvent(QPaintEvent* event) override;

    bool m_authenticated { false };
    QString m_user_initials { "" };
};

}
