/*
 * Copyright (c) 2022, Andreas Kling <andreas@cryfox.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace Web::HTML {

// https://html.spec.whatwg.org/multipage/browsing-the-web.html#history-handling-behavior
enum class HistoryHandlingBehavior {
    Push,
    Replace,
};

}
