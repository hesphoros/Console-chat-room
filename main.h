#pragma once
#include <stdio.h>
#include <string>
 
#include <conio.h>
#include <WinSock2.h>//网络相关服务头文件
#pragma comment (lib,"WS2_32.lib")//网络相关库文件
 
using namespace std;
HANDLE hMutex;//互斥锁（用于线程之间的互斥）
#define SERVER_IP		"118.126.117.125"//服务器IP 字符串
#define QUN_LIAO_PROT  2022
char line1[111];//up
char line2[111];//空白字符分割线
char nickName[32];//昵称
SOCKET serverSocket;//网络套接字
sockaddr_in sockAddr;//网络地址
void GBKToUTF8(string& strGBK);
void gotoxy(int x, int y);
string UTF8ToGBK(const char* strUTF8);