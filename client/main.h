#pragma once
#include <stdio.h>
#include <string>
 
#include <conio.h>
#include <WinSock2.h>//������ط���ͷ�ļ�
#pragma comment (lib,"WS2_32.lib")//������ؿ��ļ�
 
using namespace std;
HANDLE hMutex;//�������������߳�֮��Ļ��⣩
#define SERVER_IP		"118.126.117.125"//������IP �ַ���
#define QUN_LIAO_PROT  2022
char line1[111];//up
char line2[111];//�հ��ַ��ָ���
char nickName[32];//�ǳ�
SOCKET serverSocket;//�����׽���
sockaddr_in sockAddr;//�����ַ
void GBKToUTF8(string& strGBK);
void gotoxy(int x, int y);
string UTF8ToGBK(const char* strUTF8);