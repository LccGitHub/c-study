

#ifndef HANDLER_H
#define HANDLER_H

#include "Cmd.h"


/* polyspace< JUST-MISRA-18.4 : we need union. : Justify > */
typedef union {
    CMD1_REQ_T a_cmd1_para;
    CMD2_REQ_T a_cmd2_para;
} MNG_REQ_U;

typedef struct {
    uint16_t        opcode;
    MNG_REQ_U req;
} MNG_REQ_MESSAGE_T;


/*---------------------------------------------------------------------------
| Portability: WifiMngInitMessage
|----------------------------------------------------------------------------
| Routine description:
| Bt Mng Free Message
|---------------------------------------------------------------------------
| Parameters description:
| param_name: void
| result:
/---------------------------------------------------------------------------- */
void MessageInit(void);

/*---------------------------------------------------------------------------
| Portability: WifiMngReleaseMessage
|----------------------------------------------------------------------------
| Routine description:
| Bt Mng Release Message
|---------------------------------------------------------------------------
| Parameters description:
| param_name: void
| result:
/---------------------------------------------------------------------------- */
void MessageRelease( void );

/*---------------------------------------------------------------------------
| Portability: MessageLoop
|----------------------------------------------------------------------------
| Routine description:
|
|---------------------------------------------------------------------------
| Parameters description:
| param_name: void
| result:
/---------------------------------------------------------------------------- */
MNG_REQ_MESSAGE_T* MessageLoop(void);

/*---------------------------------------------------------------------------
| Portability: WifiMngFreeMessage
|----------------------------------------------------------------------------
| Routine description:
| Bt Mng Free Message
|---------------------------------------------------------------------------
| Parameters description:
| param_name: void
| result:
/---------------------------------------------------------------------------- */
void MessageFree(void);


/*---------------------------------------------------------------------------
| Portability: MessageLoopExit
|----------------------------------------------------------------------------
| Routine description:
|
|---------------------------------------------------------------------------
| Parameters description:
| param_name: void
| result:
/---------------------------------------------------------------------------- */
void MessageLoopExit(void);

/*---------------------------------------------------------------------------
| Portability: MessageSend
|----------------------------------------------------------------------------
| Routine description:
|
|---------------------------------------------------------------------------
| Parameters description:
| param_name: void
| result:
/---------------------------------------------------------------------------- */
bool MessageSend(MNG_REQ_MESSAGE_T* _msg);


#endif
