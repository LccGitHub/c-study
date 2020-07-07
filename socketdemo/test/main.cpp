/**
 * Copyright @ 2020 -  Suntec Software(Shanghai) Co., Ltd.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are NOT permitted except as agreed by
 * Suntec Software(Shanghai) Co., Ltd.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */


// host compile command
/*
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./
g++ test/main.cpp -I./include -std=c++11 -L./ -lsocketif -lpthread  -o testsocket
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifdef ENABLE_JSONCPP
/*
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./
g++ test/main.cpp -I./include -I./jsoncpp/include  -std=c++11 -L./ -lsocketif -lpthread -ljsoncpp -DENABLE_JSONCPP -o testsocket
*/
#include <json/json.h>
#endif
#include "SocketIF.h"



namespace nutshell
{

std::string rootKey = "mr:v1";
std::string pnKey = "pn";
std::string uidKey = "uid";
std::string snKey = "sn";
std::string rndKey = "rnd";

class SocketTest: public SocketIF
{
public:
    SocketTest(const char* interface, const char* ip, int port)
        : SocketIF(interface, ip, port)
    {

    }


    virtual ~SocketTest()
    {

    }


    virtual void recvDataInd(const uint8_t* data, const uint32_t len) override
    {
        printf("%s,%d, len = %d\n", __func__, __LINE__, len);
        printf("\n%s\n\n", data);
#ifdef ENABLE_JSONCPP
        Json::Reader reader;
        Json::Value root;


        if(!reader.parse((char*)data, root)) {
            printf("%s,%d, reader.parse failed\n", __func__, __LINE__);
        }
        else if (!root.isMember(rootKey)) {
            printf("%s,%d, no key[%s]\n", __func__, __LINE__, rootKey.c_str());
        }
        else {
            std::string pn = root[rootKey.c_str()][pnKey.c_str()].asString();
            std::string uid = root[rootKey.c_str()][uidKey.c_str()].asString();
            std::string sn = root[rootKey.c_str()][snKey.c_str()].asString();
            std::string rnd = root[rootKey.c_str()][rndKey.c_str()].asString();
            printf("%s[%s]\n", pnKey.c_str(), pn.c_str());
            printf("%s[%s]\n", uidKey.c_str(), uid.c_str());
            printf("%s[%s]\n", snKey.c_str(), sn.c_str());
            printf("%s[%s]\n", rndKey.c_str(), rnd.c_str());

        }
#else
        static bool haveSend = false;
        if (!haveSend) {
            const uint8_t* temp = (uint8_t*)"Done";
            uint32_t len1 = strlen((char*)temp);
            sendDataReq(temp, len1);
            haveSend = true;
        }
#endif
    }
    virtual void connectStateInd(E_SOCKET_STATE result) override
    {
        printf("%s,%d, state = %d\n", __func__, __LINE__, result);
    }
    virtual void sendDataCfm(E_SOCKET_SEND_RES result) override
    {
        printf("%s,%d, result = %d\n", __func__, __LINE__, result);
    }


};
}

/**
 * @brief The funcs_t struct
 */
struct funcs_t
{
    int id; /* command id */
    char name[100]; /* comand name */
    // rtc_function test_func;
};

funcs_t test_funcs[]=
{
    { 0, "send data" },

    { 10, "stop" },


    { 100, "show_menu" },


    { -1, "NULL" }

};

void show_menu()
{
    int i  = 0;
    printf("######  Socket Test Menu ######\n");
    for (i = 0;; i++) {
        if (test_funcs[i].id == -1) {
            break;
        }
        printf("%d:\t%s\n", test_funcs[i].id, test_funcs[i].name);


    }
    printf("Please select cmd index(-1 exit):\n");
}

int main(int argc, char** argv)
{
    if (argc != 5) {
        printf("error input argc[%d], usgage as follow:\n", argc);
        printf("%s [interface] [ip] [port] [isClient:0 or 1]\n", argv[0]);
    }
    else {
        char interface[64] = { 0 };
        char ip[64] = { 0 };
        memcpy(interface, argv[1], sizeof(interface) -1);
        memcpy(ip, argv[2], sizeof(ip) -1);
        int port = atoi(argv[3]);
        nutshell::E_SOCKET_TYPE isClient = ((atoi(argv[4]) ==  nutshell::E_SOCKET_TYPE_SERVER)? nutshell::E_SOCKET_TYPE_SERVER:nutshell::E_SOCKET_TYPE_CLIENT);

        nutshell::SocketTest test(interface, ip, port);
        test.start(isClient);


        bool loop_flag = true;
        while (loop_flag) {
            int option;
            scanf("%d", &option);

            if (-1 == option) {
                /* will exit loop */
                loop_flag = false;
                break;
            }
            show_menu();

            switch (option) {
            case 0:
            {
                printf("Go to >>%s \n", "sendDataReq");
#ifdef ENABLE_JSONCPP
                Json::Value json_temp;
                Json::Value root;
                Json::FastWriter fast_writer;
	            json_temp[nutshell::pnKey.c_str()] = Json::Value("Craker");
                json_temp[nutshell::uidKey.c_str()] = Json::Value("1111111122222222333333334444444455555555666666667777777788888888");
                json_temp[nutshell::snKey.c_str()] = Json::Value("123456799124!@#$JJ");
	            json_temp[nutshell::rndKey.c_str()] = Json::Value("78124");
	            root[nutshell::rootKey.c_str()] = json_temp;

                std::string jsonStr = fast_writer.write(root);
                const uint8_t* temp = (uint8_t*)(jsonStr.c_str());
                uint32_t len = strlen((char*)temp);

#else
                const uint8_t* temp = (uint8_t*)"hello world";
                uint32_t len = strlen((char*)temp);
#endif
                printf("send data: \n");
                printf("%s \n", temp);
                test.sendDataReq(temp, len);
                break;
            }
            case 10:
            {
                printf("Go to >>%s \n", "stop");
                test.stop();
                break;
            }
            default:
            {
                break;
            }
            }
        } 
    }
    return 0;
}

/* EOF */
