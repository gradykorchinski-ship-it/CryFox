/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibAuth/LocalAuthManager.h>
#include <QPainter>
#include <QPainterPath>
#include <UI/Qt/ProfileButton.h>

namespace CryFox {

ProfileButton::ProfileButton(QWidget* parent)
    : QToolButton(parent)
{
    setFixedSize(36, 36);
    setToolTip("Profile & Settings");
    setCursor(Qt::PointingHandCursor);

    // Remove default button styling
    setStyleSheet(R"(
        QToolButton {
            background: transparent;
            border: none;
        }
        QToolButton:hover {
            background: rgba(99, 102, 241, 0.1);
            border-radius: 18px;
        }
    )");

    connect(this, &QToolButton::clicked, this, &ProfileButton::clicked_signal);
}

void ProfileButton::set_authenticated(bool authenticated)
{
    m_authenticated = authenticated;
    update();
}

void ProfileButton::set_user_initials(QString const& initials)
{
    m_user_initials = initials;
    update();
}

void ProfileButton::update_appearance()
{
    update();
}

void ProfileButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Calculate circle bounds with padding
    int padding = 4;
    int diameter = qMin(width(), height()) - (padding * 2);
    QRect circleRect((width() - diameter) / 2, (height() - diameter) / 2, diameter, diameter);

    m_authenticated = Auth::LocalAuthManager::the().is_authenticated();

    if (m_authenticated) {
        // Authenticated: Gradient circle with initials (or 'M' for Master)
        QString initials = m_user_initials.isEmpty() ? "M" : m_user_initials;

        QLinearGradient gradient(circleRect.topLeft(), circleRect.bottomRight());
        gradient.setColorAt(0, QColor(99, 102, 241)); // #6366f1
        gradient.setColorAt(1, QColor(139, 92, 246)); // #8b5cf6

        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(circleRect);

        // Draw initials
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setPixelSize(12);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(circleRect, Qt::AlignCenter, initials);
    } else {
        // Unauthenticated: Default user icon

        // Circle background
        painter.setBrush(QColor(60, 60, 60));
        painter.setPen(QPen(QColor(90, 90, 90), 1));
        painter.drawEllipse(circleRect);

        // Draw simple user icon (head and shoulders)
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(160, 160, 160));

        // Head (smaller circle)
        int headSize = diameter / 3;
        QRect headRect(
            circleRect.center().x() - headSize / 2,
            circleRect.top() + diameter / 4,
            headSize,
            headSize);
        painter.drawEllipse(headRect);

        // Shoulders (arc at bottom)
        int shoulderWidth = diameter * 2 / 3;
        int shoulderHeight = diameter / 3;
        QRect shouldersRect(
            circleRect.center().x() - shoulderWidth / 2,
            circleRect.bottom() - shoulderHeight / 3 * 2,
            shoulderWidth,
            shoulderHeight);

        QPainterPath path;
        path.addEllipse(shouldersRect);
        painter.setClipRect(circleRect);
        painter.drawPath(path);
        painter.setClipping(false);

        // Draw padlock badge if vault is setup but locked
        if (Auth::LocalAuthManager::the().is_setup()) {
            int badgeSize = diameter / 2.5;
            QRect badgeRect(circleRect.right() - badgeSize, circleRect.bottom() - badgeSize, badgeSize, badgeSize);

            painter.setBrush(QColor(239, 68, 68)); // Red
            painter.setPen(QPen(QColor(40, 40, 40), 1));
            painter.drawEllipse(badgeRect);

            painter.setPen(Qt::white);
            QFont font = painter.font();
            font.setPixelSize(8);
            painter.setFont(font);
            painter.drawText(badgeRect, Qt::AlignCenter, "L"); // 'L' for Locked
        }
    }
}

}
