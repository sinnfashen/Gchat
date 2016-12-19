#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <winsock.h>
#include <winsock2.h>
#include <thread>
#include <mutex>
#include <cstring>
#include <windows.h>
#include <iostream>
#define  PORT 2378
#define  IP_ADDRESS "123.207.93.47"
std::mutex gb_mutex;
COORD outps = {0,5},inps = {0,1};
void gotoxy(int x, int y) //光标定位
{
    COORD pos = {x, y};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOut, pos);
}
void gotoxy(COORD pos) //光标定位
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOut, pos);
}
COORD getps()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    GetConsoleScreenBufferInfo(hOut, &bInfo);
    return bInfo.dwCursorPosition;
}
int getsz()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    GetConsoleScreenBufferInfo(hOut, &bInfo);
    return bInfo.srWindow.Top;
}
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void inputsquard(int p)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT rcScroll, rcClip;
    COORD crDest = {0, p};
    CHAR_INFO chFill;
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    GetConsoleScreenBufferInfo( hOut, &bInfo );
    rcScroll.Left = 0;
    rcScroll.Top = inps.Y-1;
    rcScroll.Right = bInfo.dwSize.X - 1;
    rcScroll.Bottom = inps.Y+2;
    rcClip = rcScroll;
    chFill.Attributes = bInfo.wAttributes;
    chFill.Char.AsciiChar = ' ';
    ScrollConsoleScreenBuffer(hOut, &rcScroll, &rcClip, crDest, &chFill);
    inps.Y = p+1;
}
void reader(SOCKET sockfd)
{
    char buffer[256];
    while(true)
    {
        memset(buffer, '\0',sizeof(buffer));
        recv(sockfd,buffer,255,0);
        gb_mutex.lock();
        gotoxy(outps);
        printf("%s\n",buffer);
        outps = getps();
        inputsquard(getsz());
        gotoxy(inps);
        gb_mutex.unlock();
    }
}
void writer(SOCKET sockfd)
{
    //std::string buffer;
    char buffer[256];
    while(true)
    {
        //std::cin>>buffer;
        fgets(buffer,255,stdin);
        gb_mutex.lock();
        gotoxy(inps);
        printf("                                                                                      ");
        gotoxy(inps);
        gb_mutex.unlock();
        int i = strlen(buffer)-1;
        if(buffer[i]=='\n')
            buffer[i]=0;
        printf("\b\r");
        send(sockfd,buffer,strlen(buffer),0);
    }
}
int main(int argc, char *argv[])
{
    WSADATA wsaData;
    WSAStartup(0x202,&wsaData);
    SOCKET sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    serv_addr.sin_port = htons(PORT);
    memset(serv_addr.sin_zero,0x00,8);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) == SOCKET_ERROR)
        error("ERROR connecting");
    printf("----------------------------------------------------------------------------\n\n----------------------------------------------------------------------------");
    gotoxy(inps);
    std::thread Reader(reader,sockfd);
    std::thread Writer(writer,sockfd);
    //while(true);
    Reader.join();
    Writer.join();
    //close(sockfd);
    return 0;
}
