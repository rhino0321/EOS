#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFSIZE 1024
#define SEM_MODE 0666 /* rw(owner)-rw(group)-rw(other) permission */
#define SEM_KEY 1122

/* P () - returns 0 if OK; -1 if there was a problem */


int connectsock(const char *host, const char *service, const char *protocol);

int main(int argc, char *argv[])
{
    int connfd;/* socket descriptor*/
    int shmid, i;
    int *shm;
    int *s;
    char buf[BUFSIZE];
    key_t key = 1234;
    
    if((shmid = shmget(key, sizeof(int), 0666)) < 0)
    {
	perror("shmget");
	exit(1);    
    }

    if ((shm = shmat(shmid, NULL, 0)) == (int*) -1)
    {
	perror("shmmat");
	exit(1); 
    }
    
     
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
        exit(1);
    }

    connfd = connectsock(argv[1], argv[2], "tcp");
    s = shm;
    printf("Connected to %s %s %s %s %d.\n", argv[1], argv[2], argv[3], argv[4], atoi(argv[5]));
    fflush(stdout);
    /*for (i=0; i<atoi(argv[5]); i++)
    {
    	if ((strcmp(argv[3], "withdraw") == 0) && (strcmp(argv[4], "1") == 0))
    		write(connfd, "3", 1);
   	else if ((strcmp(argv[3], "withdraw") == 0) && (strcmp(argv[4], "2") == 0))
    		write(connfd, "4", 1);
    	else if ((strcmp(argv[3], "deposit") == 0) && (strcmp(argv[4], "1") == 0))
    		write(connfd, "1", 1);
    	else if ((strcmp(argv[3], "deposit") == 0) && (strcmp(argv[4], "2") == 0))
    		write(connfd, "2", 1);
    }*/
    *(s+2) = atoi(argv[5]);
    if ((strcmp(argv[3], "withdraw") == 0) && (strcmp(argv[4], "1") == 0))
    	*s = -1;
    else if ((strcmp(argv[3], "withdraw") == 0) && (strcmp(argv[4], "2") == 0))
    	*s = -2;
    else if ((strcmp(argv[3], "deposit") == 0) && (strcmp(argv[4], "1") == 0))
    	*s = 1;
    else if ((strcmp(argv[3], "deposit") == 0) && (strcmp(argv[4], "2") == 0))
    	*s = 2;
    shmdt(shm);
    close(connfd);

    return 0;
}


int connectsock(const char *host, const char *service, const char *transport)
{
    struct hostent *phe;    // 主機條目
    struct servent *pse;    // 服務條目
    struct sockaddr_in sin; // Internet端點地址
    int s, type;            // 套接字描述符和套接字類型

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    // 查找服務條目
    if ((pse = getservbyname(service, transport)))
        sin.sin_port = pse->s_port;
    else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
        return -1;

    // 使用協議創建套接字
    if (strcmp(transport, "udp") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    if ((s = socket(PF_INET, type, 0)) < 0)
        return -1;

    // 連接到服務器
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        return -1;

    return s;
}
