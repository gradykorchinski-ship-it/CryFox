/**
 * Copyright (c) 2023, Andrew Kaster <akaster@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

package org.serenityos.cryfox

import android.os.Message

class RequestServerService : CryFoxServiceBase("RequestServerService") {
    override fun handleServiceSpecificMessage(msg: Message): Boolean {
        return false
    }

    companion object {
        init {
            System.loadLibrary("requestserverservice")
        }
    }
}
