#include <thread>
#include <mutex>
#include <sys/types.h>
#include <unistd.h> 
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <csignal>
#include <fstream>
const int USER_LIMIT = 50,M_PORT = 2385;
std::mutex cout_mutex;
int user[USER_LIMIT],current_user = 0;
char username[USER_LIMIT][20];
bool user_alive[USER_LIMIT];
std::thread threads[USER_LIMIT];
char divide[] = "-----";
void announce(char *buffer)
{
    for(int i = 0;i<current_user;i++)
        if(user_alive[i])
            write(user[i],buffer,strlen(buffer));

}
void thread_server(int id)
{
    int sockfd = user[id];
    printf("thread %d opened\n",sockfd);
    write(sockfd,"what's your name:",17);
    char buffer[256] = "Welcome to the smallest gay dating platform(ofcourse not)\n    made by FSOL    \n--current users:";
    read(sockfd,username[id],9);
    for(int i = 0;i<current_user;i++)
	if(user_alive[i])
	{
	    strcat(buffer,username[i]);
	    strcat(buffer,"  ,  ");
	}
    strcat(buffer,"\n");
    write(sockfd,buffer,strlen(buffer));
    strcpy(buffer,username[id]);strcat(buffer," is online!\n");
    announce(buffer);
    while(true)
    {
        bzero(buffer,256);
        int n = read(sockfd,buffer,255);
	if(n <= 0)
	{
	    printf("user %d exit\n",id);
	    user_alive[id]=false;
	    return;
	}	
	strcat(buffer,divide);strcat(buffer,username[id]);
        cout_mutex.lock();
        announce(buffer);
        cout_mutex.unlock();
    }
    printf("%d is going to die!\n",sockfd);
    close(sockfd);
}
int main(int argc,char *argv[])
{
    signal(SIGPIPE,SIG_IGN);
    printf("server on\n");
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    if(atoi(argv[1])>=2000)
        serv_addr.sin_port = htons(atoi(argv[1]));
    else
   	serv_addr.sin_port = htons(M_PORT);
	
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	printf("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while(true)
    {
	printf("get in circle\n");
        int t = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	cout_mutex.lock();
        user[current_user] = t;
	user_alive[current_user] = true;
        printf("new connection----ID:%d \n current user:%d\n",t,current_user+1);
        threads[current_user] = std::thread(thread_server,current_user);
	current_user++;
        cout_mutex.unlock();
    }
    printf("server end\n");
    for(int i = 0;i<current_user;i++)
	threads[i].join();
}

