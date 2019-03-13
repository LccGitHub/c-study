

#ifndef CMD_H
#define CMD_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* -------------------------------- Includes -------------------------------- */


/* -------------------------------- Defines --------------------------------- */

/* ----------------------------- Data Types ------------------------------- */


#ifdef __cplusplus
extern "C" {
#endif


#define MIN(x,y) (((x)>=(y))?(y):(x))


#define CMD1_REQ                       0x0001
#define CMD2_REQ                      0x0002


typedef struct {
    uint8_t dummy;
} CMD1_REQ_T;

typedef struct {
    uint32_t strLen;
    uint8_t str[100];
} CMD2_REQ_T;




typedef void (*CALLBACK)(uint16_t opcode, void* msg);

/* ---------------------------- Global Variables ---------------------------- */

/* --------------------------- Routine prototypes --------------------------- */

/*---------------------------------------------------------------------------
| Portability: Cmd1Req
|----------------------------------------------------------------------------
| Routine description:
| Cmd1 Req
|---------------------------------------------------------------------------
| Parameters description:
| param_name:
| result:
/---------------------------------------------------------------------------- */
void Cmd1Req(void);

/*---------------------------------------------------------------------------
| Portability: Cm2Req
|----------------------------------------------------------------------------
| Routine description:
| Cmd2 Req
|---------------------------------------------------------------------------
| Parameters description:
| param_name: strLen str
| result:
/---------------------------------------------------------------------------- */
void Cm2Req(uint32_t strLen, const uint8_t* str);




#ifdef __cplusplus
}
#endif

#endif
