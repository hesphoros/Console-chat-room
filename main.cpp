#include "main.h"


//转码
void GBKToUTF8(string& strGBK)
{
	int len = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	wchar_t* wszUtf8 = new wchar_t[len];
	memset(wszUtf8, 0, len);
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, wszUtf8, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL);
	char* szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL, NULL);
	strGBK = szUtf8; 
	delete[] szUtf8;
	delete[] wszUtf8;
}

void gotoxy(int x,int y)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);//获取当前窗口句柄
	COORD pos = { x,y };
	SetConsoleCursorPosition(hOut,pos);

}

void uiInit() 
{
	system("mode con lines=36 cols=110");
	system("cls");
	gotoxy(0,33);

	for (int i = 0; i < 110; i++)line1[i] = '-';
	line1[110] = 0;
	for (int i = 0; i < 110; i++)line2[i] = ' ';
	line2[110] = 0;
	printf("%s\n\n", line1);
	 

}
/*
* 
*   mciSendString("open ./images/小情歌.mp3 alias bgm ", NULL, 0, NULL);
	mciSendString("play bgm", NULL, 0, NULL);
*/

void login() 
{
	system("mode con lines=5 cols=30 \n ");
	printf("   欢迎进入奇牛QQ聊天室 \n \n");
	printf("       昵称：");
	scanf_s("%s", nickName, sizeof(nickName));

	while (getchar() != '\n');//清空输入缓冲区
	string name = nickName;
	GBKToUTF8(name );
	send(serverSocket,name.c_str()/*c++字符串 转成c语言*/, strlen(name.c_str()) + 1, 0);//send niskname to server 如果中文，可能回出问题
	//做send检查
}

string UTF8ToGBK(const char* strUTF8)
{ 
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1]; 
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK); 
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK; 
	return strTemp; 
}

void  printMsg(const char* msg)
{
	//上锁(申请互斥锁)
	WaitForSingleObject(hMutex, INFINITE/*等待时间 永远等*/);
	//没有申请到 就一直等待 直到等到为止
	 static POINT pos = { 0,0 };
	 gotoxy(pos.x, pos.y);
	 //printf("%s \n", msg);
	  static int color = 31;

	 printf("\033[0;%d;40m%s\033[0m\n",color++,msg);
	 if (color > 36)
	 {
		 color = 31;
	 }
	
	 HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	 CONSOLE_SCREEN_BUFFER_INFO info;
	 GetConsoleScreenBufferInfo(hOut,&info);
	 pos.x = info.dwCursorPosition.X;
	 pos.y = info.dwCursorPosition.Y;
	 if (pos.y >= 33)
	 {
		 printf("%s\n", line2);
		 printf("\n\n");
		 gotoxy(0, 33);
		 printf("%s\n ",line1);
		 pos.y -= 1;
	 }
	 gotoxy(1,34);
	//释放锁
	ReleaseMutex(hMutex);
}

void editPrint(int col,int ch)
{
	WaitForSingleObject(hMutex, INFINITE);

	gotoxy(col, 34);
	printf("%c",ch);
	ReleaseMutex(hMutex);
 }

void editPrint(int col,const char * str)
{
	WaitForSingleObject(hMutex, INFINITE);

	gotoxy(col, 34);
	printf("%s", str);
	ReleaseMutex(hMutex);
}

bool init()
{
	WSADATA data;//初始化结果
	//1.网络服务初始化
	int ret = WSAStartup(MAKEWORD(1, 1), &data);
	if (ret != 0)
	{
		return false;
	}
	//2.网络套接字socket
			
	serverSocket = socket(PF_INET/*网络 数据流 数据报 类型*/, SOCK_STREAM/*数据流*/,IPPROTO_TCP/*TCP协议*/);

	//3.配置物理地址
	sockAddr.sin_family = PF_INET;//网络地址
	sockAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);//IP地址
	//端口号 最大为65535  转字节序
	sockAddr.sin_port = htons(QUN_LIAO_PROT);

	CreateMutex(0, 0, L"console");

	return true;
}

//创建线程
DWORD WINAPI threadFuncRecy(LPVOID pram) {

	char buff[4096];//接收信息
	while (1) {
		int ret = recv(serverSocket, buff, sizeof(buff),0);
		if (ret <= 0) {
			printf("服务器关闭或故障\n");
			break;
		}
		
		//打印接收到的信息
		
		printMsg(UTF8ToGBK(buff).c_str());

	}
	return NULL;
}

bool isHZ(char str[], int index) {
	//一个汉字占两个字节 第一个字节< 0; 第二个字节 有可能小于零 也可能大于0
	//一个英文字符，只有一个字节， 》0 

	int i = 0;
	while (i < index) {
		if (str[i] > 0) {
			i++;
		}
		else {
			i += 2;
		}

	}
	if (i == index)
	{
		return false;
	}
	else {
		return true;
	}
}

int main(void)
{
	 
	if (init() == false) {
		printf("Initialization failed\n");
		getchar();
		return -1;
	}
	else {
		printf("Initialization succeeded \n");
		 
	}

	 
	// 连接服务器（发起网络连接）
	int ret = connect(serverSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	if (ret != 0) {
		printf("Initialization failed Failed to connect to the server. Please check the network \n");
		 
		return 1;
	}
	else 
	{
		printf("connect succeeded \n");
	 
	}
 
	//登录聊天室
	login();
	
	uiInit();//初始化界面

	//创建线程回返回一个线程句柄
	HANDLE	hThread =  CreateThread(0, 0, threadFuncRecy,0,0,0);
	CloseHandle(hThread);//关闭句柄 没关闭线程
	char buff[1024];//保存用户输入的字符串
	//内容设置为零
	memset(buff, 0,sizeof(buff));
	//编辑信息
	while (1)
	{
		editPrint(0,'>');
		int len = 0;
		while (1){
			if (_kbhit())
			{
				char c = getch();	
				if (c == '\r')
				{
					//按下回车
					break;
				}
				else if(c == 8)//退格键
				{
					if (len == 0)
					{
						continue;
					}
					//删除  
					if (isHZ(buff, len - 1)  ) {
						//printf("\b\b \b\b");
						editPrint(len + 1, "\b\b  \b\b");
						buff[len - 1] = 0;
						buff[len - 2] = 0;
						len -= 2;

					}
					else {
						editPrint(len + 1, "\b \b");
						buff[len - 1] = 0;
						len -= 1;
					}

					continue;
				}

				WaitForSingleObject(hMutex, INFINITE);
				do {
					printf("%c",c);
					buff[len++] = c;
				} while (_kbhit()&& (c=getch()));

				ReleaseMutex(hMutex);

				//editPrint(len + 1,c);
				//buff[len++] = c ;
			}
		}
		if (len == 0)
		{
			continue;
		}

		//清楚编辑区的信息
		char buff2[1024];
		sprintf_s(buff2,sizeof(buff2),"%s\n",line2);
		editPrint(0, buff2);

		//把用户自己说的话，输出到聊天室
		sprintf_s(buff2, sizeof(buff2), "【localHost@%s】%s", nickName, buff);
		
		printMsg(buff2);
		//发送编辑好的字符串

		send(serverSocket,buff, strlen(buff)+1,0);
	}

	getchar();

	return 0;
}

 