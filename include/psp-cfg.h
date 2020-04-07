/** @file
 * PSP Emulator - PSP system config descriptor.
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
#ifndef __psp_cfg_h
#define __psp_cfg_h

#include <common/types.h>


/**
 * Emulation mode.
 */
typedef enum PSPEMUMODE
{
    /** Invalid mode, do not use. */
    PSPEMUMODE_INVALID = 0,
    /** A single usermode application is executed and the svc interface is emulated. */
    PSPEMUMODE_APP,
    /** Full system emulation mode with the supervisor code being executed as well. */
    PSPEMUMODE_SYSTEM,
    /** Full system emulation mode with the supervisor and on chip bootloader code being executed as well. */
    PSPEMUMODE_SYSTEM_ON_CHIP_BL
} PSPEMUMODE;


/**
 * Micro architecture the PSP emulated for.
 */
typedef enum PSPEMUMICROARCH
{
    /** Invalid value. */
    PSPEMUMICROARCH_INVALID = 0,
    /** Original Zen. */
    PSPEMUMICROARCH_ZEN,
    /* Zen+ */
    PSPEMUMICROARCH_ZEN_PLUS,
    /* Zen2 */
    PSPEMUMICROARCH_ZEN2,
    /** 32bit hack. */
    PSPEMUMICROARCH_32BIT_HACK = 0x7fffffff
} PSPEMUMICROARCH;


/**
 * AMD CPU segment.
 */
typedef enum PSPEMUAMDCPUSEGMENT
{
    /** Invalid segment. */
    PSPEMUAMDCPUSEGMENT_INVALID = 0,
    /** Ryzen (Consumer). */
    PSPEMUAMDCPUSEGMENT_RYZEN,
    /** Ryzen Pro (Business). */
    PSPEMUAMDCPUSEGMENT_RYZEN_PRO,
    /** Threadripper (HEDT). */
    PSPEMUAMDCPUSEGMENT_THREADRIPPER,
    /** Epyc (Server). */
    PSPEMUAMDCPUSEGMENT_EPYC,
    /** 32bit hack. */
    PSPEMUAMDCPUSEGMENT_32BIT_HACK = 0x7fffffff
} PSPEMUAMDCPUSEGMENT;


/**
 * ACPI sleep state.
 */
typedef enum PSPEMUACPISTATE
{
    /** Invalid sleep state. */
    PSPEMUACPISTATE_INVALID = 0,
    /** S0 state: Working. */
    PSPEMUACPISTATE_S0,
    /** S1 state: Sleeping with Processor context maintained. */
    PSPEMUACPISTATE_S1,
    /** S2 state: */
    PSPEMUACPISTATE_S2,
    /** S3 state: */
    PSPEMUACPISTATE_S3,
    /** S4 state: */
    PSPEMUACPISTATE_S4,
    /** S5 state: Soft off */
    PSPEMUACPISTATE_S5,
    /** 32bit hack. */
    PSPEMUACPISTATE_32BIT_HACK = 0x7fffffff
} PSPEMUACPISTATE;


/**
 * PSP emulator config.
 */
typedef struct PSPEMUCFG
{
    /** Emulation mode. */
    PSPEMUMODE              enmMode;
    /** The micro architecture we are emulating. */
    PSPEMUMICROARCH         enmMicroArch;
    /** The CPU segment we are emulating. */
    PSPEMUAMDCPUSEGMENT     enmCpuSegment;
    /** ACPI system state the emulator starts from. */
    PSPEMUACPISTATE         enmAcpiState;
    /** The flash ROM path. */
    const char              *pszPathFlashRom;
    /** Path to the on chip bootloader if in appropriate mode. */
    const char              *pszPathOnChipBl;
    /** Binary to load, if NULL we get one from the flash image depending on the mode. */
    const char              *pszPathBinLoad;
    /** Path to the boot rom service page to inject (for system and app emulation mode). */
    const char              *pszPathBootRomSvcPage;
    /** Preloads the given "app" binary. */
    const char              *pszAppPreload;
    /** Flag whether overwritten binaries have the 256 byte header prepended (affects the load address). */
    bool                    fBinContainsHdr;
    /** Flag whether to load the PSP directory from the flash image into the boot rom service page. */
    bool                    fLoadPspDir;
    /** Flag whether to enable the debug mode inside the PSP firmware disabling signature checks etc. */
    bool                    fPspDbgMode;
    /** Flag whether to intercept svc 6 in on chip bootloader and system mode. */
    bool                    fIncptSvc6;
    /** Flag whether to to trace all svc calls in on chip bootloader and system mode. */
    bool                    fTraceSvcs;
    /** Flag whether the timer should tick in real time. */
    bool                    fTimerRealtime;
    /** Debugger port to listen on, 0 means debugger is disabled. */
    uint16_t                uDbgPort;
    /** Pointer to the read flash rom content. */
    void                    *pvFlashRom;
    /** Size of the flash ROM in bytes. */
    size_t                  cbFlashRom;
    /** Pointer to the on chip bootloader ROM content. */
    void                    *pvOnChipBl;
    /** Size of the on chip bootloader ROM in bytes. */
    size_t                  cbOnChipBl;
    /** Pointer to the binary content if pszPathBinLoad is not NULL. */
    void                    *pvBinLoad;
    /** Number of bytes of the binary loaded. */
    size_t                  cbBinLoad;
    /** Pointer to the binary content if pszAppPreload is not NULL. */
    void                    *pvAppPreload;
    /** Number of bytes of the app loaded. */
    size_t                  cbAppPreload;
    /** The proxy address if configured. */
    const char              *pszPspProxyAddr;
    /** Path to the trace log to write if enabled. */
    const char              *pszTraceLog;
    /** UART remtoe address. */
    const char              *pszUartRemoteAddr;
    /** Flash EM100 emulator emulator port. */
    uint16_t                uEm100FlashEmuPort;
    /** Number of sockets in the system to emulate. */
    uint32_t                cSockets;
    /** Number of CCDs per socket to emulate. */
    uint32_t                cCcdsPerSocket;
    /** Pointer to an array of strings for devices which should be instantiated, temrinated by a NULL entry.
     *NULL means default with everything emulated. */
    const char              *papszDevs;
} PSPEMUCFG;
/** Pointer to a PSPEmu config. */
typedef PSPEMUCFG *PPSPEMUCFG;
/** Pointer to a const PSPEmu config. */
typedef const PSPEMUCFG *PCPSPEMUCFG;

#endif /* __psp_cfg_h */

