/*
 * Copyright (c) 2025, Andrew Kaster <andrew@cryfox.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Types.h>

namespace Web::Bindings {

enum class AgentType : u8 {
    SimilarOriginWindow,
    DedicatedWorker,
    SharedWorker,
    ServiceWorker,
    Worklet,
};

}
