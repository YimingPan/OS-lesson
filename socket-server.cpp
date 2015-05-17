#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#define PORT 6000

void init();
void start();
DWORD WINAPI recv_thread(LPVOID lpParameter);

SOCKET sockConnect, sockSrv;

int main()
{
    init();
    start();
    system("pause");

    return 0;
}

void init()
{
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
    //创建socket操作，建立流式套接字，返回套接字号sockSrv
    sockSrv = socket(AF_INET, SOCK_STREAM, 0);

    //套接字与本地地址相连
    SOCKADDR_IN addrSrv;

    //将INADDR_ANY转换为网络字节序，调用 htonl(long型)或htons(整型)
    //INADDR_ANY表示任意地址
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(PORT);

    bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

    //第一个参数指定需要设置的套接字，第二个参数为等待连接队列的最大长度
    listen(sockSrv, 10);

    printf("Socket initialization done.\n");
}

void start()
{
    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);
    sockConnect = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
    if(sockConnect == INVALID_SOCKET) {
        printf("%d\n", WSAGetLastError());
    }

    //创建一个用于接收Client消息的线程
    HANDLE hThread_recv = CreateThread(NULL, 0, recv_thread, NULL, 0, NULL);
    CloseHandle(hThread_recv);

    /**char* inet_ntoa (structin_addrin)
     * 将一个IP转换成一个互联网标准点分格式的字符串
     */
    char sendBuf[100] = {0};
    sprintf(sendBuf, "Welcome %s to the server", inet_ntoa(addrClient.sin_addr));
    send(sockConnect, sendBuf, strlen(sendBuf)+1, 0);
    while(1) {
        gets(sendBuf);
        if(strcmp(sendBuf, "quit") == 0)
            break;
        send(sockConnect, sendBuf, strlen(sendBuf)+1, 0);
    }
    printf("End linking...\n");
    sprintf(sendBuf, "Server has ended the talk\n");
    send(sockConnect, sendBuf, strlen(sendBuf)+1, 0);
    closesocket(sockConnect);
}

DWORD WINAPI recv_thread(LPVOID lpParameter)
{
    char recvBuf[100] = {0};
    for(;;) {
        if(recv(sockConnect , recvBuf, 100, 0) <= 0)
            return 1;
        printf("Client: %s\n", recvBuf);
    }
    return 0;
}
