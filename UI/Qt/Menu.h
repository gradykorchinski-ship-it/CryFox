/*
 * Copyright (c) 2025, Tim Flynn <trflynn89@cryfox.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWebView/Menu.h>

class QAction;
class QMenu;
class QWidget;

namespace CryFox {

class WebContentView;

QMenu* create_application_menu(QWidget& parent, WebView::Menu&);
QMenu* create_context_menu(QWidget& parent, WebContentView&, WebView::Menu&);
QAction* create_application_action(QWidget& parent, WebView::Action&);

}
