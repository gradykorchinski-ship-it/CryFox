/**
 * Copyright (c) 2023, Andrew Kaster <akaster@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

package org.serenityos.cryfox

import android.content.Context
import android.content.Intent
import android.os.Message
import android.util.Log

class WebContentService : CryFoxServiceBase("WebContentService") {
    override fun handleServiceSpecificMessage(msg: Message): Boolean {
        return false
    }

    init {
        nativeInit();
    }

    private fun bindRequestServer(ipcFd: Int)
    {
        val connector = CryFoxServiceConnection(ipcFd, resourceDir)
        connector.onDisconnect = {
            // FIXME: Notify impl that service is dead and might need restarted
            Log.e(TAG, "RequestServer Died! :(")
        }
        // FIXME: Unbind this at some point maybe
        bindService(
            Intent(this, RequestServerService::class.java),
            connector,
            Context.BIND_AUTO_CREATE
        )
    }

    private fun bindImageDecoder(ipcFd: Int)
    {
        val connector = CryFoxServiceConnection(ipcFd, resourceDir)
        connector.onDisconnect = {
            // FIXME: Notify impl that service is dead and might need restarted
            Log.e(TAG, "ImageDecoder Died! :(")
        }
        // FIXME: Unbind this at some point maybe
        bindService(
            Intent(this, ImageDecoderService::class.java),
            connector,
            Context.BIND_AUTO_CREATE
        )
    }

    external fun nativeInit()

    companion object {
        init {
            System.loadLibrary("webcontentservice")
        }
    }
}
