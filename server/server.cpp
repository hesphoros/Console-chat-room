#include <locale.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <list>
using namespace std;
 
#define _PORT_ 2024
#define _BACKLOG_ 10
 
// 客服连接到服务器后，需要先登录，发送登录消息：
// name字符串，不能含有空格
 
struct client {
public:
        int socket;
        char name[32];
        char ip[16];
 
        bool operator==(const client &other) const {
                return other.socket == socket;
        }
};
 
list<client> clients;
 
void showClients() {
        printf("\n\n***********show all clients********\n");
        int k=0;
        for(auto it=clients.begin(); it != clients.end(); it++) {
                printf("NO.%d name:[%s] socket:[%d]\n", ++k, it->name, it->socket);
        }
        printf("\n*********************************** \n\n");
}
 
void* client_func(void *arg) {
        client *p = (client*)arg;
 
        // regist
        char buf[1024];
        char buff2[1024];
        memset(buf,'0',sizeof(buf));//跟前面的初始化对比
        if (read(p->socket, buf, sizeof(buf)) <= 0) {
                delete  p;
                return NULL;
        }
 
        printf("Receive register info: %s\n",buf);
        sscanf(buf, "%s", p->name);
        sprintf(buf, "【%s@%s】进入聊天室", p->ip, p->name);
 
        clients.push_back(*p);
        printf("Add a new client. count=%d\n", clients.size());
        //showClients();
 
        printf("clients size = %d\n", clients.size());
        int k=0;
 
        for(auto it=clients.begin(); it != clients.end(); it++) {
                printf("send no.%d client\n", ++k);
                write(it->socket, buf, strlen(buf)+1);
        }
 
        //clients.push_back(*p);
 
        while(1) {
                int ret = read(p->socket, buf, sizeof(buf));
                if (ret <= 0) {
                        break;
                }
                printf("client :# %s\n",buf);
 
                sprintf(buff2, "【%s@%s】%s", p->ip, p->name, buf);
 
                for(auto it=clients.begin(); it != clients.end(); it++) {
                        if (it->socket != p->socket) {
                                write(it->socket, buff2, strlen(buff2)+1);
                        }
                }
    }
 
        clients.remove(*p);
        delete p;
        return NULL;
}
 
 
int main()
{   
	setlocale(LC_ALL, "UTF-8");

    int sock = socket(AF_INET,SOCK_STREAM,0);
 
    struct sockaddr_in server_socket;
    struct sockaddr_in client_socket;
    bzero(&server_socket,sizeof(server_socket));
    server_socket.sin_family = AF_INET;
    server_socket.sin_port = htons(_PORT_);
    server_socket.sin_addr.s_addr = htonl(INADDR_ANY);
 
    bind(sock,(struct sockaddr *)&server_socket,sizeof(struct sockaddr_in));
    listen(sock,_BACKLOG_);
 
    while(1) {
        socklen_t len = sizeof(client_socket);
        printf("wait accept...\n");
        int client_sock = accept(sock,(struct sockaddr *)&client_socket,&len);
        if(client_sock < 0) {
            printf("accept error, error is %d,errstring is %s\n",errno,strerror(errno));
            close(sock);
            return 3;
        }
 
        client *c = new client;
        strcpy(c->ip, inet_ntoa(client_socket.sin_addr));
        printf("get connect,ip is %s\n", c->ip);
        c->socket = client_sock;
 
 
        pthread_t tid;
        pthread_create(&tid, NULL, client_func, (void*)c);
    }
    close(sock);
    return 0;
}
