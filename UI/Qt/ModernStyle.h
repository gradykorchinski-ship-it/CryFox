/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace CryFox {

// Modern UI Stylesheet
// Optimized for performance - no transitions, minimal effects
inline char const* modern_stylesheet()
{
    return R"(
        /* Main Window */
        QMainWindow {
            background-color: #1a1a1a;
            color: #e0e0e0;
        }

        /* Tab Widget */
        QTabWidget::pane {
            border: none;
            background-color: #1a1a1a;
        }

        /* Tab Bar */
        QTabBar::tab {
            background-color: #1e1e1e;
            color: #b0b0b0;
            border: none;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            padding: 8px 16px;
            margin-right: 2px;
            min-width: 120px;
            font-size: 13px;
            font-weight: 500;
        }

        QTabBar::tab:selected {
            background-color: #242424;
            color: #e0e0e0;
        }

        QTabBar::tab:hover:!selected {
            background-color: #252525;
            color: #c0c0c0;
        }

        /* Toolbar */
        QToolBar {
            background-color: #1e1e1e;
            border: none;
            spacing: 8px;
            padding: 4px 8px;
        }

        QToolButton {
            background-color: transparent;
            border: none;
            border-radius: 4px;
            padding: 6px;
            color: #e0e0e0;
        }

        QToolButton:hover {
            background-color: #2a2a2a;
        }

        QToolButton:pressed {
            background-color: #323232;
        }

        /* Location Bar (URL input) */
        QLineEdit {
            background-color: #242424;
            border: 1px solid #3a3a3a;
            border-radius: 8px;
            padding: 8px 12px;
            color: #e0e0e0;
            font-size: 14px;
            selection-background-color: #6366f1;
            selection-color: #ffffff;
        }

        QLineEdit:focus {
            border: 2px solid #6366f1;
            padding: 7px 11px;
        }

        /* Buttons */
        QPushButton {
            background-color: #2a2a2a;
            border: 1px solid #3a3a3a;
            border-radius: 6px;
            padding: 8px 16px;
            color: #e0e0e0;
            font-size: 14px;
        }

        QPushButton:hover {
            background-color: #323232;
            border-color: #4a4a4a;
        }

        QPushButton:pressed {
            background-color: #282828;
        }

        /* Menu Bar */
        QMenuBar {
            background-color: #1e1e1e;
            color: #e0e0e0;
            border-bottom: 1px solid #2a2a2a;
        }

        QMenuBar::item {
            background-color: transparent;
            padding: 6px 12px;
        }

        QMenuBar::item:selected {
            background-color: #2a2a2a;
        }

        /* Menus */
        QMenu {
            background-color: #242424;
            border: 1px solid #3a3a3a;
            border-radius: 8px;
            padding: 4px;
            color: #e0e0e0;
        }

        QMenu::item {
            padding: 8px 24px 8px 12px;
            border-radius: 4px;
        }

        QMenu::item:selected {
            background-color: #2a2a2a;
        }

        QMenu::separator {
            height: 1px;
            background-color: #3a3a3a;
            margin: 4px 8px;
        }

        /* Scrollbars */
        QScrollBar:vertical {
            background-color: #1e1e1e;
            width: 12px;
            border-radius: 6px;
        }

        QScrollBar::handle:vertical {
            background-color: #3a3a3a;
            border-radius: 6px;
            min-height: 20px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: #4a4a4a;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0px;
        }

        QScrollBar:horizontal {
            background-color: #1e1e1e;
            height: 12px;
            border-radius: 6px;
        }

        QScrollBar::handle:horizontal {
            background-color: #3a3a3a;
            border-radius: 6px;
            min-width: 20px;
        }

        QScrollBar::handle:horizontal:hover {
            background-color: #4a4a4a;
        }

        QScrollBar::add-line:horizontal,
        QScrollBar::sub-line:horizontal {
            width: 0px;
        }

        /* Status Bar */
        QStatusBar {
            background-color: #1e1e1e;
            color: #b0b0b0;
            border-top: 1px solid #2a2a2a;
        }
    )";
}

// Light theme variant
inline char const* light_stylesheet()
{
    return R"(
        /* Main Window */
        QMainWindow {
            background-color: #ffffff;
            color: #1a1a1a;
        }

        /* Tab Widget */
        QTabWidget::pane {
            border: none;
            background-color: #ffffff;
        }

        /* Tab Bar */
        QTabBar::tab {
            background-color: #f0f0f0;
            color: #606060;
            border: none;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            padding: 8px 16px;
            margin-right: 2px;
            min-width: 120px;
            font-size: 13px;
            font-weight: 500;
        }

        QTabBar::tab:selected {
            background-color: #ffffff;
            color: #1a1a1a;
        }

        QTabBar::tab:hover:!selected {
            background-color: #e8e8e8;
            color: #404040;
        }

        /* Toolbar */
        QToolBar {
            background-color: #f8f8f8;
            border: none;
            spacing: 8px;
            padding: 4px 8px;
        }

        QToolButton {
            background-color: transparent;
            border: none;
            border-radius: 4px;
            padding: 6px;
            color: #1a1a1a;
        }

        QToolButton:hover {
            background-color: #e8e8e8;
        }

        QToolButton:pressed {
            background-color: #d8d8d8;
        }

        /* Location Bar */
        QLineEdit {
            background-color: #ffffff;
            border: 1px solid #d0d0d0;
            border-radius: 8px;
            padding: 8px 12px;
            color: #1a1a1a;
            font-size: 14px;
            selection-background-color: #6366f1;
            selection-color: #ffffff;
        }

        QLineEdit:focus {
            border: 2px solid #6366f1;
            padding: 7px 11px;
        }

        /* Buttons */
        QPushButton {
            background-color: #f0f0f0;
            border: 1px solid #d0d0d0;
            border-radius: 6px;
            padding: 8px 16px;
            color: #1a1a1a;
            font-size: 14px;
        }

        QPushButton:hover {
            background-color: #e8e8e8;
            border-color: #c0c0c0;
        }

        QPushButton:pressed {
            background-color: #d8d8d8;
        }

        /* Menu Bar */
        QMenuBar {
            background-color: #f8f8f8;
            color: #1a1a1a;
            border-bottom: 1px solid #e0e0e0;
        }

        QMenuBar::item {
            background-color: transparent;
            padding: 6px 12px;
        }

        QMenuBar::item:selected {
            background-color: #e8e8e8;
        }

        /* Menus */
        QMenu {
            background-color: #ffffff;
            border: 1px solid #d0d0d0;
            border-radius: 8px;
            padding: 4px;
            color: #1a1a1a;
        }

        QMenu::item {
            padding: 8px 24px 8px 12px;
            border-radius: 4px;
        }

        QMenu::item:selected {
            background-color: #f0f0f0;
        }

        QMenu::separator {
            height: 1px;
            background-color: #e0e0e0;
            margin: 4px 8px;
        }

        /* Scrollbars */
        QScrollBar:vertical {
            background-color: #f8f8f8;
            width: 12px;
            border-radius: 6px;
        }

        QScrollBar::handle:vertical {
            background-color: #d0d0d0;
            border-radius: 6px;
            min-height: 20px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: #c0c0c0;
        }

        QScrollBar:horizontal {
            background-color: #f8f8f8;
            height: 12px;
            border-radius: 6px;
        }

        QScrollBar::handle:horizontal {
            background-color: #d0d0d0;
            border-radius: 6px;
            min-width: 20px;
        }

        QScrollBar::handle:horizontal:hover {
            background-color: #c0c0c0;
        }

        /* Status Bar */
        QStatusBar {
            background-color: #f8f8f8;
            color: #606060;
            border-top: 1px solid #e0e0e0;
        }
    )";
}

}
