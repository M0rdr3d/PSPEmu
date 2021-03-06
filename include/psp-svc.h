/** @file
 * PSP Emulator - API for the emulated supervisor part (SVC)
 */

/*
 * Copyright (C) 2019-2020 Alexander Eichner <alexander.eichner@campus.tu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef __psp_svc_h
#define __psp_svc_h

#include <stdint.h>

#include <libpspproxy.h>

#include "psp-core.h"
#include "psp-iom.h"

/** Opaque PSP SVC state handle. */
typedef struct PSPSVCINT *PSPSVC;
/** Pointer to a PSP emulation core handle. */
typedef PSPSVC *PPSPSVC;

/**
 * Creates a new emulated PSP supervisor state.
 *
 * @returns Status code.
 * @param   phSvcState              Where to store the SVC state handle on success.
 * @param   hPspCore                The PSP core handle.
 * @param   hIoMgr                  The I/O manager handle associated with the given PSP core.
 * @param   hPspProxyCtx            PSP proxy context to use for SVC emulation.
 */
int PSPEmuSvcStateCreate(PPSPSVC phSvcState, PSPCORE hPspCore, PSPIOM hIoMgr, PSPPROXYCTX hPspProxyCtx);

/**
 * Destroys a given PSP sueprvisor state.
 *
 * @returns nothing.
 * @param   hSvcState               The SVC state handle to destroy.
 */
void PSPEmuSvcStateDestroy(PSPSVC hSvcState);

#endif /* __psp_svc_h */
