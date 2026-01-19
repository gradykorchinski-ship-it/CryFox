/*
 * Copyright (c) 2020-2022, Andreas Kling <andreas@cryfox.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGC/Cell.h>
#include <LibGC/NanBoxedValue.h>

namespace GC {

void GC::Cell::Visitor::visit(NanBoxedValue const& value)
{
    if (value.is_cell())
        visit_impl(value.as_cell());
}

}
