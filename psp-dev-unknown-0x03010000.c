/** @file
 * PSP Emulator - Unknown device residing at 0x03200000.
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

#include <stdio.h>

#include <psp-devs.h>


/**
 * Unknown device instance data.
 */
typedef struct PSPDEVUNK
{
    uint8_t uDummy;
} PSPDEVUNK;
/** Pointer to the device instance data. */
typedef PSPDEVUNK *PPSPDEVUNK;

static int pspDevUnkInit(PPSPMMIODEV pDev)
{
    /* Nothing to do. */
    return 0;
}

static void pspDevUnkDestruct(PPSPMMIODEV pDev)
{
    /* Nothing to do so far. */
}

static void pspDevUnkMmioRead(PPSPMMIODEV pDev, PSPADDR offMmio, size_t cbRead, void *pvVal)
{
    printf("%s: offMmio=%#x cbRead=%zu\n", __FUNCTION__, offMmio, cbRead);

    switch (offMmio)
    {
        case 0x104:
            /* The on chip bootloader waits in on_chip_bl_main() until bit 8 is set. */
            *(uint32_t *)pvVal = 0x100;
            break;
    }
}

static void pspDevUnkMmioWrite(PPSPMMIODEV pDev, PSPADDR offMmio, size_t cbWrite, const void *pvVal)
{
    printf("%s: offMmio=%#x cbWrite=%zu\n", __FUNCTION__, offMmio, cbWrite);

    switch (cbWrite)
    {
        case 4:
            printf("    u32Val=%#x\n", *(uint32_t *)pvVal);
            break;
    }
}


/**
 * Device registration structure.
 */
const PSPMMIODEVREG g_MmioDevRegUnk0x03010000 =
{
    /** pszName */
    "unk-0x030100000",
    /** pszDesc */
    "Unknown device starting at 0x030100000",
    /** cbInstance */
    sizeof(PSPDEVUNK),
    /** cbMmio */
    4096,
    /** pfnInit */
    pspDevUnkInit,
    /** pfnDestruct */
    pspDevUnkDestruct,
    /** pfnMmioRead */
    pspDevUnkMmioRead,
    /** pfnMmioWrite */
    pspDevUnkMmioWrite
};
