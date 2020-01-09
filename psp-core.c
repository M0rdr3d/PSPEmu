/** @file
 * PSP Emulator - Core API (interfacing with unicorn engine).
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
#include <psp-svc.h>

#define IN_PSP_EMULATOR
#include <psp-core.h>
#include <psp-svc.h>

/**
 * A datum read/written.
 */
typedef union PSPDATUM
{
    uint8_t   u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    uint8_t  ab[8];
} PSPDATUM;
typedef PSPDATUM *PPSPDATUM;

/** Pointer to a PSP core instance. */
typedef struct PSPCORE *PPSPCORE;
/** Pointer to a const PSP core instance. */
typedef const struct PSPCORE *PCPSPCORE;

/**
 * Cached x86 memory mapping
 */
typedef struct PSPX86MEMCACHEDMAPPING
{
    /** Pointer to the owning PSP core instance. */
    PCPSPCORE           pPspCore;
    /** X86 Mapped base address, NIL_X86PADDR if mapping is not used. */
    X86PADDR            PhysX86AddrBase;
    /** 4K aligned base address of the mapping (for unicorn). */
    PSPADDR             PspAddrBase4K;
    /** PSP base address of the mapping. */
    PSPADDR             PspAddrBase;
    /** Highest cached address so far (exclusive, defines the memory span initialized). */
    PSPADDR             PspAddrCached;
    /** Highest address written so far (exclusive, defines range of memory we have to sync back on unmap). */
    PSPADDR             PspAddrHighestWritten;
    /** Size of mapped area. */
    size_t              cbMapped;
    /** 4K aligned mapping size (for unicorn). */
    size_t              cbMapped4K;
    /** Amount of memory allocated. */
    size_t              cbAlloc;
    /** Pointer to the memory caching the mapping. */
    void                *pvMapping;
} PSPX86MEMCACHEDMAPPING;
typedef PSPX86MEMCACHEDMAPPING *PPSPX86MEMCACHEDMAPPING;

/**
 * A single PSP core executing.
 */
typedef struct PSPCOREINT
{
    /** The emulation mode. */
    PSPCOREMODE             enmMode;
    /** The unicorn engine pointer. */
    uc_engine               *pUcEngine;
    /** The SRAM region. */
    void                    *pvSram;
    /** Size of the SRAM region. */
    size_t                  cbSram;
    /** The CCD ID. */
    uint32_t                idCcd;
    /** The supervisor emulation instance if app emulation is used. */
    PSPSVCSTATE             hSvcState;
    /** The next address to execute instructions from. */
    PSPADDR                 PspAddrExecNext;
    /** The x86 mapping for the privileged DRAM region where the SEV app state is saved. */
    PSPX86MEMCACHEDMAPPING  X86MappingPrivState;
    /** Size of the state region. */
    uint32_t                cbStateRegion;
    /** Cached temporary x86 mappings. */
    PSPX86MEMCACHEDMAPPING  aX86Mappings[8];
} PSPCOREINT;
/** Pointer to a single PSP core instance. */
typedef PSPCOREINT *PPSPCOREINT;


/**
 * PSP Core register name to unicorn mapping.
 */
static const int g_aUcRegs[] =
{
    0,
    UC_ARM_REG_R0,
    UC_ARM_REG_R1,
    UC_ARM_REG_R2,
    UC_ARM_REG_R3,
    UC_ARM_REG_R4,
    UC_ARM_REG_R5,
    UC_ARM_REG_R6,
    UC_ARM_REG_R7,
    UC_ARM_REG_R8,
    UC_ARM_REG_R9,
    UC_ARM_REG_R10,
    UC_ARM_REG_R11,
    UC_ARM_REG_R12,
    UC_ARM_REG_SP,
    UC_ARM_REG_LR,
    UC_ARM_REG_PC
};


/**
 * Converts the PSP core register enum to the unicorn equivalent.
 *
 * @returns Unicorn register number.
 * @param   enmReg                  The register.
 */
static int pspEmuCoreReg2Uc(PSPCOREREG enmReg)
{
    return g_aUcRegs[enmReg];
}


/**
 * Converts a unicorn error to a general status code.
 *
 * @returns Status code.
 * @param   rcUc                    The unicorn status code to convert.
 */
static int pspEmuCoreErrConvertFromUcErr(uc_err rcUc)
{
    if (rcUc == UC_ERR_OK)
        return 0;

    return -1; /** @todo */
}


int PSPEmuCoreCreate(PPSPCORE phCore, PSPCOREMODE enmMode)
{
    int rc = 0;
    PPSPCORE pThis = (PPSPCORE)calloc(1, sizeof(*pThis));

    if (pThis)
    {
        uc_err err;

        pThis->enmMode = enmMode;
        pThis->cbSram  = _256K;
        pThis->pvSram  = calloc(1, pThis->cbSram);
        if (pThis->pvSram)
        {
            /* Initialize unicorn engine in ARM mode. */
            err = uc_open(UC_ARCH_ARM, UC_MODE_ARM, &pThis->pUcEngine);
            if (!err)
            {
                if (!rc)
                {
                    uc_mem_map_ptr(pPspCore->pUcEngine, 0x0, pThis->cbSram, UC_PROT_READ | UC_PROT_WRITE, pThis->pvSram);
                    *phCore = pThis;
                    return 0;
                }

                uc_close(pThis->pUcEngine);
            }
            else
            {
                printf("not ok - Failed on uc_open() with error: %s\n", uc_strerror(err));
                rc = EINVAL;
            }

            free(pThis->pvSram);
        }
        else
            rc = ENOMEM;

        free(pThis);
    }
    else
        rc = ENOMEM;

    return rc;
}

void PSPEmuCoreDestroy(PSPCORE hCore)
{
    PPSPCOREINT pThis = hCore;

    uc_close(pThis->pUcEngine);
    free(pThis->pvSram);
    free(pThis);
}

int PSPEmuCoreCcdSet(PSPCORE hCore, uint32_t idCcd)
{
    PPSPCOREINT pThis = hCore;

    pThis->idCcd = idCcd;
    return 0;
}

int PSPEmuCoreQueryCcd(PSPCORE hCore, uint32_t *pidCcd)
{
    PPSPCOREINT pThis = hCore;

    *pidCcd = pThis->idCcd;
    return 0;
}

int PSPEmuCoreMemWrite(PSPCORE hCore, PSPADDR AddrPspWrite, void *pvData, size_t cbData)
{
    PPSPCOREINT pThis = hCore;

    uc_err rcUc = uc_mem_write(pThis->pUcEngine, AddrPspWrite, pvData, cbData);
    return pspEmuCoreErrConvertFromUcErr(rcUc);
}

int PSPEmuCoreMemRead(PSPCORE hCore, PSPADDR AddrPspRead, void *pvDst, size_t cbDst)
{
    PPSPCOREINT pThis = hCore;

    uc_err rcUc = uc_mem_read(pThis->pUcEngine, AddrPspRead, pvDst, cbDst);
    return pspEmuCoreErrConvertFromUcErr(rcUc);
}

int PSPEmuCoreMemAddRegion(PSPCORE hCore, PSPADDR AddrStart, size_t cbRegion)
{
    return -1; /** @todo */
}

int PSPEmuCoreSetReg(PSPCORE hCore, PSPREG enmReg, uint32_t uVal)
{
    PPSPCOREINT pThis = hCore;

    uint32_t uTmp = uVal;
    uc_err rcUc = uc_reg_write(pThis->pUcEngine, pspEmuCoreReg2Uc(enmReg), &uTmp);
    return pspEmuCoreErrConvertFromUcErr(rcUc);
}

int PSPEmuCoreQueryReg(PSPCORE hCore, PSPCOREREG enmReg, uint32_t *puVal)
{
    PPSPCOREINT pThis = hCore;

    uc_err rcUc = uc_reg_read(pThis->pUcEngine, pspEmuCoreReg2Uc(enmReg), puVal);
    return pspEmuCoreErrConvertFromUcErr(rcUc);
}

int PSPEmuCoreExecSetStartAddr(PSPCORE hCore, PSPADDR AddrExecStart)
{
    PPSPCOREINT pThis = hCore;

    pThis->PspAddrExecNext = AddrExecStart;
    return 0;
}

int PSPEmuCoreExecRun(PSPCORE hCore, uint32_t cInsnExec, uint32_t msExec)
{
    PPSPCOREINT pThis = hCore;

    uc_err rcUc = uc_emu_start(pThis->pUcEngine, pThis->PspAddrExecNext, 0xffffffff /** @todo */, msExec, cInsnExec);
    return pspEmuCoreErrConvertFromUcErr(rcUc);
}

int PSPEmuCoreExecStop(PSPCORE hCore)
{
    return -1; /** @todo */
}
