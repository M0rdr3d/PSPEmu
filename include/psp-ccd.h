/** @file
 * PSP Emulator - API for a single PSP contained in a CCD along with the peripherals
 */

/*
 * Copyright (C) 2020 Alexander Eichner <alexander.eichner@campus.tu-berlin.de>
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
#ifndef __psp_ccd_h
#define __psp_ccd_h

#include <stdint.h>
#include <stddef.h>

#include <common/types.h>

#include <psp-cfg.h>
#include <psp-core.h>


/** Opaque PSP CCD handle. */
typedef struct PSPCCDINT *PSPCCD;
/** Pointer to a PSP CCD core handle. */
typedef PSPCCD *PPSPCCD;


/**
 * Creates a new emulated CCD containing a single PSP and all the peripherals.
 *
 * @returns Status code.
 * @param   phCcd               Where to store the handle to the CCD on success.
 * @param   idSocket            Socket ID where the CCD is located.
 * @param   idCcd               ID of the emulated CCD.
 * @param   pCfg                Global config for the system.
 */
int PSPEmuCcdCreate(PPSPCCD phCcd, uint32_t idSocket, uint32_t idCcd, PCPSPEMUCFG pCfg);


/**
 * Destroys a given CCD.
 *
 * @returns nothing.
 * @param   hCcd                The CCD handle to destroy.
 */
void PSPEmuCcdDestroy(PSPCCD hCcd);


/**
 * Queries the emulation core handle from the given CCD.
 *
 * @returns Status code.
 * @param   hCcd                The CCD handle.
 * @param   phPspCore           Where to store the handle to the PSP core on success.
 */
int PSPEmuCcdQueryCore(PSPCCD hCcd, PPSPCORE phPspCore);


/**
 * Resets the given CCD instance to the initial state right after creation, including all device states.
 *
 * @returns Status code.
 * @param   hCcd                The CCD handle.
 */
int PSPEmuCcdReset(PSPCCD hCcd);


/**
 * Let the given CCD instance run.
 *
 * @returns Status code.
 * @param   hCcd                The CCD handle.
 */
int PSPEmuCcdRun(PSPCCD hCcd);

#endif /* !__psp_ccd_h */
