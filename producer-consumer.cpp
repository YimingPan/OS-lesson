#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

const int BUFFER_SIZE = 10;
const int SLEEP_TIME = 100;

HANDLE ghMutex;    // buf互斥访问量
HANDLE ghSemFull;  // buf满 信号量
HANDLE ghSemEmpty; // buf空 信号量
char buf[BUFFER_SIZE];
int buf_f, buf_r;

DWORD WINAPI ThreadProd( LPVOID );
DWORD WINAPI ThreadCons( LPVOID );

int main()
{
    const int PRODUCER_NUM = 3;
    const int CONSUMER_NUM = 3;
    HANDLE aThreadProd[PRODUCER_NUM]; // 生产者线程
    HANDLE aThreadCons[CONSUMER_NUM]; // 消费着线程

    buf_f = buf_r = 0;

    // Create a mutex with no initial owner
    ghMutex = CreateMutex(
        NULL,          // default security attributes
        FALSE,         // initially not owned
        NULL);         // unnamed mutex

    if (ghMutex == NULL)
    {
        printf("CreateMutex error: %d\n", GetLastError());
        return 1;
    }

    ghSemFull = CreateSemaphore(
        NULL,         //default security attributes
        0,            //initial count
        BUFFER_SIZE,  //maximum count
        NULL);        //unnamed semaphore

    if(ghSemFull == NULL)
    {
        printf("CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }

    ghSemEmpty = CreateSemaphore(
        NULL,         //default security attributes
        BUFFER_SIZE,  //initial count
        BUFFER_SIZE,  //maximum count
        NULL);        //unnamed semaphore

    if(ghSemEmpty == NULL)
    {
        printf("CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }

    //Create producer threads
    for(int i=0; i<PRODUCER_NUM; i++)
    {
        aThreadProd[i] = CreateThread(
            NULL,       // default security attributes
            0,          // default stack size
            ThreadProd,
            NULL,       // no thread function arguments
            0,          // default creation flags
            NULL);      // receive thread identifier

        if(aThreadProd[i] == NULL)
        {
            printf("CreateThread error: %d\n", GetLastError());
            return 1;
        }
    }

    //Create consumer threads
    for(int i=0; i<CONSUMER_NUM; i++)
    {
        aThreadCons[i] = CreateThread(
            NULL,
            0,
            ThreadCons,
            NULL,
            0,
            NULL);

        if(aThreadCons[i] == NULL)
        {
            printf("CreateThread error: %d\n", GetLastError());
            return 1;
        }
    }

    _getch();

    // Close thread and semaphore handles
    for(int i=0; i<PRODUCER_NUM; i++)
        CloseHandle(aThreadProd[i]);
    for(int i=0; i<CONSUMER_NUM; i++)
        CloseHandle(aThreadCons[i]);

    CloseHandle(ghMutex);
    CloseHandle(ghSemEmpty);
    CloseHandle(ghSemFull);

    return 0;
}

DWORD WINAPI ThreadProd( LPVOID lpParam )
{
    while(TRUE)
    {
        WaitForSingleObject(ghSemEmpty, INFINITE);
        WaitForSingleObject(ghMutex, INFINITE);
        printf("Producer %d: produce one puduct in %d\n", GetCurrentThreadId(), buf_r);
        buf_r = (buf_r+1)%BUFFER_SIZE;
        Sleep(SLEEP_TIME);
        ReleaseMutex(ghMutex);
        ReleaseSemaphore(ghSemFull, 1, NULL);
    }
    return TRUE;
}

DWORD WINAPI ThreadCons( LPVOID lpParam )
{
    while(TRUE)
    {
        WaitForSingleObject(ghSemFull, INFINITE);
        WaitForSingleObject(ghMutex, INFINITE);
        printf("Consumer %d: consume one puduct in %d\n", GetCurrentThreadId(), buf_f);
        buf_f = (buf_f+1)%BUFFER_SIZE;
        Sleep(SLEEP_TIME);
        ReleaseMutex(ghMutex);
        ReleaseSemaphore(ghSemEmpty, 1, NULL);
    }
    return TRUE;
}
