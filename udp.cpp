#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib,"WS2_32.lib")
#define BUF_SIZE    64

int main(void)
{
    WSADATA wsd;   // initialize socket resource
    SOCKET  s;

    if(WSAStartup(MAKEWORD(2,2),&wsd) != 0)
    {
        printf("WSAStartup failed !/n");
        return 1;
    }

    // setup socket
    s = socket(AF_INET,SOCK_DGRAM,0);
    if(s == INVALID_SOCKET)
    {
        printf("socket() failed, Error Code:%d/n",WSAGetLastError());
        WSACleanup();
        return 1;
    }

    char        buf[BUF_SIZE];  // buld buffer for data
    SOCKADDR_IN servAddr;       // define socket address
    SOCKET      sockClient = socket(AF_INET,SOCK_DGRAM,0);
    int         nRet;

    ZeroMemory(buf,BUF_SIZE);
    strcpy(buf,"UDP Hello World !");

    // set the address
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.S_un.S_addr = inet_addr("192.168.1.177");
    servAddr.sin_port = htons(8888);

    // send data to server
    int nServAddLen = sizeof(servAddr);
    if(sendto(sockClient,buf,BUF_SIZE,0,(sockaddr *)&servAddr,nServAddLen) == SOCKET_ERROR)
    {
        printf("recvfrom() failed:%d/n",WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }
    nRet = recvfrom(sockClient,buf,BUF_SIZE,0,(sockaddr *)&servAddr,&nServAddLen);
    if(SOCKET_ERROR == nRet)
    {
        printf("recvfrom failed !/n");
        closesocket(s);
        WSACleanup();
        return -1;
    }

    // printout the data 
    printf("Recv From Server:%s/n",buf);
    closesocket(s);
    WSACleanup();
    return 0;
}

