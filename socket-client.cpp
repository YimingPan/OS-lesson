#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

void init();
void start();
DWORD WINAPI recv_thread(LPVOID lpParameter);

SOCKET sockClient;

int main()
{
    init();
    start();
    system("pause");

    return 0;
}

DWORD WINAPI recv_thread(LPVOID lpParameter)
{
    char recvBuf[100] = {0};
    for(;;) {
        if(recv(sockClient , recvBuf, 100, 0) <= 0)
            return 1;
        printf("Server: %s\n", recvBuf);
    }
    return 0;
}

void init()
{
    char ip[20];
    printf("Server's ip: ");
    scanf("%s", ip);
    getchar();

    WORD wVersionRequested;
    WSADATA wsaData;        //用于接收windows socket的结构信息

    wVersionRequested = MAKEWORD(1,1);

    if(WSAStartup(wVersionRequested, &wsaData) ) {
        printf("WSAStartup error!\n");
        exit(EXIT_FAILURE);
    }
    if(LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
        printf("Socket Edition Wrong!\n");
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    //创建socket操作，建立流式套接字，返回套接字号sockClient
    sockClient = socket(AF_INET, SOCK_STREAM, 0);

    // 将套接字sockClient与远程主机相连
    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = inet_addr(ip);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6000);
    if(connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) != 0) {
        printf("%d\n", WSAGetLastError());
    }
    else printf("Connection succeeded\n");
}

void start()
{
    HANDLE hThread_recv = CreateThread(NULL, 0, recv_thread, NULL, 0, NULL);
    CloseHandle(hThread_recv);
    char sendBuf[100] = {0};

    while(1) {
        gets(sendBuf);
        if(strcmp(sendBuf, "quit") == 0)
            break;
        send(sockClient, sendBuf, strlen(sendBuf)+1, 0);
    }
    sprintf(sendBuf, "Client has ended the talk\n");
    send(sockClient, sendBuf, strlen(sendBuf)+1, 0);
    printf("End linking...\n");
    closesocket(sockClient);
    WSACleanup();   // 终止对套接字库的使用
}
