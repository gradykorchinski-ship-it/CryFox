/*
 * Copyright (c) 2026, CryFox Developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/String.h>
#include <AK/Vector.h>
#include <LibURL/URL.h>

namespace Browser {

struct HistorySite {
    String url;
    String title;
    String domain;
    size_t visit_count;
    time_t last_visit;
};

class HistoryManager {
public:
    static HistoryManager& the();

    HistoryManager();
    ~HistoryManager();

    ErrorOr<void> initialize();
    ErrorOr<void> add_visit(URL::URL const& url, String const& title);
    ErrorOr<Vector<HistorySite>> get_most_visited(size_t count = 8);
    ErrorOr<String> get_most_visited_json(size_t count = 8);
    ErrorOr<void> clear_history();

private:
    ErrorOr<void> create_tables();
    ErrorOr<String> get_database_path();

    bool m_initialized { false };
    String m_database_path;
};

}
