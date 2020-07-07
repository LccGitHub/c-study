/************  Copyright 2004-2016 FICOSA CORPORATIVE ELECTRONICS  ************
 | Language:       |  MISRA C
 | Controller:     |  AR755X
 | Spec. Document: |
 |                 |
 |                 |
 |                 |
 |-----------------|------------------------------------------------------------
 | Project:        |
 | Reference:      |
 |------------------------------------------------------------------------------
 | MODIFICATIONS HISTORY
 | Date     - Coder -    Description
 | 1/10/16   GG     Fixing code style.
 |------------------------------------------------------------------------------
 | DESCRIPTION:
 |
 ******************************************************************************/

#ifndef FWUPDATEMNG_H
#define FWUPDATEMNG_H

/* -------------------------------- Includes -------------------------------- */

#include "WebInterfaceCommon.h"

/* -------------------------------- Defines --------------------------------- */
#define SECONDARY_SIZE 4096
#define SIGNATURE 2048
#define VERSION_SIZE 512
#define MAX_VERS_BYTES  257

#define CACERT_FILE_PATH "/home/root/cacert.pem"
/* ----------------------------- Data Types ------------------------------- */


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    UPDATE_NONE = 0,
    UPDATE_VERIFYING = 1,
    UPDATE_VERIFYED = 2,
    UPDATE_DOENLOADING = 3,
    UPDATE_DOENLOADED = 4
} FwUpdate_staus;

typedef enum
{
    VERIFY_SUCCESS = 0,
    VERIFY_VERSION_FAILED = 1,
    VERIFY_SIGNATURE_FAILED = 2,
    VERIFY_FAILED = 3
} FwUpdate_Verify_result;

typedef void (*ConnectServiceFunc_t)
(
    void
);

/* ---------------------------- Global Variables ---------------------------- */

/* --------------------------- Routine prototypes --------------------------- */


/*---------------------------------------------------------------------------
| Portability: FwUpdateVerifyReq
|----------------------------------------------------------------------------
| Routine description:
| Firware Update verify req
|1.Extracting secondary certificate
|     1> open updat file
|     2>read secondary.64 from file
|     3>decode base64 secondary.64 to secondary.crt
|2.Validating secondary certificate:
|3.Extracting secondary keys:
|4.Extracting signature
|     1>read  signature.64 from file
|     1>decode base64 signature.64 to signature
|5.Validating signature
|---------------------------------------------------------------------------
| Parameters description:
| param_name: *fw_version
| result:FwUpdate_Verify_result
/---------------------------------------------------------------------------- */
LE_SHARED FwUpdate_Verify_result FwUpdateVerifyReq(char fw_version[]);

/*---------------------------------------------------------------------------
| Portability: FwUpdateUpgradeReq
|----------------------------------------------------------------------------
| Routine description:
| Firware Update upgrade req
|---------------------------------------------------------------------------
| Parameters description:
| param_name:
| result:BOOL
/---------------------------------------------------------------------------- */
LE_SHARED BOOL FwUpdateUpgradeReq(void);

/*---------------------------------------------------------------------------
| Portability: FwUpdateGetStatus
|----------------------------------------------------------------------------
| Routine description:
| Firware Update get status
|---------------------------------------------------------------------------
| Parameters description:
| param_name: fw_verison
| result:Updat_status
/---------------------------------------------------------------------------- */
LE_SHARED FwUpdate_staus FwUpdateGetStatus(char fw_verison[]);

/*---------------------------------------------------------------------------
| Portability: FwUpdateDiscardReq
|----------------------------------------------------------------------------
| Routine description:
| Firware Update discard req
|---------------------------------------------------------------------------
| Parameters description:
| param_name:
| result:
/---------------------------------------------------------------------------- */
LE_SHARED BOOL FwUpdateDiscardReq(void);

/*---------------------------------------------------------------------------
| Portability: FwUpdageQueryVersion
|----------------------------------------------------------------------------
| Routine description:
| Firware Update query version
|---------------------------------------------------------------------------
| Parameters description:
| param_name:*firmware_ver, *bootloader_ver, *linux_ver, len >MAX_VERS_BYTES
| result:le_result_t
/---------------------------------------------------------------------------- */
LE_SHARED le_result_t FwUpdageQueryVersion(char *firmware_ver, char *bootloader_ver, char *linux_ver);

#ifdef __cplusplus
}
#endif

#endif




