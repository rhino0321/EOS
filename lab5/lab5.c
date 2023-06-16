#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h> // Network related functions, e.g. gethostbyname()
#include <netinet/in.h> // struct sockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> // system socket define
#include <sys/types.h> // system types
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#define errexit(format,arg...) exit(printf(format,##arg))
#define BUFSIZE 1024

pid_t childpid;

int sockfd, connfd, flag = 0;

struct sockaddr_in addr_cln;
socklen_t sLen = sizeof(addr_cln);

/*Create server*/
int passivesock(const char *service, const char *transport, int qlen);

/*Connect to server*/
int connectsock(const char *host, const char *service, const char *transport);

void handler(int signum)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    char snd[BUFSIZE], rcv[BUFSIZE];
    if (argc != 2)
        errexit("Usage: %s port\n", argv[0]);
    sockfd = passivesock(argv[1], "tcp", 5);
    while(1)
    {
        /*waiting for connection*/

        signal(SIGCHLD, handler);
        connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);
        childpid = fork();
        if (childpid == 0) {
            dup2(connfd,STDOUT_FILENO);
            execlp("sl", "sl", "-l", NULL);
            close(connfd);
            exit(0);
        }else{
            printf("Train ID: %d\n",childpid);
            //dup2(connfd,STDOUT_FILENO);
            //execlp("sl", "sl", "-l", NULL);
            //close(connfd);
        }
    }


    return 0;
}




int passivesock(const char* service, const char* transport, int qlen)
{
    // Store service entry return from getservbyname()
    struct servent *pse;
    // Service-end socket
    struct sockaddr_in sin;
    // Service-end socket descriptor and service type
    int s, type;
    memset(&sin, 0, sizeof(sin));
    // TCP/IP suite
    sin.sin_family = AF_INET;
    // Use any local IP, need translate to internet byte order
    sin.sin_addr.s_addr = INADDR_ANY;
    // Get port number
    // service is service name
    if (pse = getservbyname(service, transport))
        // sin.sin_port = htons(ntohs((unsigned short)pse->s_port) + portbase);
         sin.sin_port = htons(ntohs((unsigned short)pse->s_port));
    // service is port number
    else if((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
        errexit("can't get \"%s\" service entry\n", service);
    // Tranport type
    if (strcmp(transport, "udp") == 0 || strcmp(transport, "UDP") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    s = socket(PF_INET, type, 0);
    int yes=1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (s < 0)
        errexit("can't create socket: %s \n", strerror(errno));
    // Bind socket to service-end address
    if (bind(s, (struct sockaddr*)&sin, sizeof(sin)) < 0)
        errexit("can't bind to %s port: %s \n", service, strerror(errno));
    // For TCP socket, convert it to passive mode
    if (type == SOCK_STREAM && listen(s, qlen) < 0)
        errexit("can't listen on %s port: %s \n", service, strerror(errno));
    return s;
}

