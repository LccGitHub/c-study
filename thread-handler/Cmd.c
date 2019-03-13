
/* ------------------------------- includes --------------------------------- */

#include "Cmd.h"
#include "Handler.h"


void Cmd1Req(void)
{
    MNG_REQ_MESSAGE_T msg;
    msg.opcode = CMD1_REQ;
    MessageSend(&msg);

}



void Cm2Req(uint32_t strLen, const uint8_t* str)
{
    printf("funtion:%s, line: %d \n", __FUNCTION__, __LINE__);

        MNG_REQ_MESSAGE_T msg;
        msg.opcode = CMD2_REQ;
        memset(msg.req.a_cmd2_para.str, 0, sizeof(msg.req.a_cmd2_para.str));
        msg.req.a_cmd2_para.strLen = (uint32_t)MIN(strLen, (sizeof(msg.req.a_cmd2_para.str) - 1));


        strncpy((char*)msg.req.a_cmd2_para.str, (const char*)str, (size_t)msg.req.a_cmd2_para.strLen);

        printf("ssid =%s,ssidlen=%d\n", msg.req.a_cmd2_para.str, msg.req.a_cmd2_para.strLen);
        MessageSend(&msg);
}




