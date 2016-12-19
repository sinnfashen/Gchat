#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <thread>
#include <cstring>
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void reader(int sockfd)
{
    char buffer[256];
    while(true)
    {
        bzero(buffer,256);
        read(sockfd,buffer,255);
        printf("%s\n",buffer);
    }
}
void writer(int sockfd)
{
    //std::string buffer;
    char buffer[256];
    while(true)
    {
        //std::cin>>buffer;
	fgets(buffer,255,stdin);
	int i = strlen(buffer)-1;
	if(buffer[i]=='\n')
	    buffer[i]=0;
        printf("\033[1A");
        write(sockfd,buffer,strlen(buffer));
    }
}
int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    std::thread Reader(reader,sockfd);
    std::thread Writer(writer,sockfd);
    while(true);
    close(sockfd);
    return 0;
}

