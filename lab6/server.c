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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/time.h>
#define errexit(format,arg...) exit(printf(format,##arg))
#define BUFSIZE 1024
#define SEM_MODE 0666 /* rw(owner)-rw(group)-rw(other) permission */
#define SEM_KEY 1122334455
pid_t childpid;

int sockfd, connfd, flag, count = 0;
int *shm, s, i, num, sum = 0;
int retval, shmid, sem;
char buffer[BUFSIZE];
struct sockaddr_in addr_cln;
socklen_t sLen = sizeof(addr_cln);

/*Create server*/
int passivesock(const char *service, const char *transport, int qlen);

/*Connect to server*/
int connectsock(const char *host, const char *service, const char *transport);

void singalHandler(int signum)
{
        semctl(sem, 0, IPC_RMID, 0);
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
        close(sockfd);
        exit(1);
}

    /* P () - returns 0 if OK; -1 if there was a problem */
    int P (int m)
    {
	struct sembuf sop; /* the operation parameters */
	sop.sem_num = 0;
	/* access the 1st (and only) sem in the array */
	sop.sem_op = -1;
	/* wait..*/
	sop.sem_flg = 0;
	/* no special options needed */
	if (semop (m, &sop, 1) < 0) 
	{
	    fprintf(stderr,"P(): semop failed: %s\n",strerror(errno));
	    return -1;
	} 
	else 
	{
	    return 0;
	}
    }
	/* V() - returns 0 if OK; -1 if there was a problem */
    int V(int m)
    {
	struct sembuf sop; /* the operation parameters */
	sop.sem_num = 0;
	/* the 1st (and only) sem in the array */
	sop.sem_op = 1;
	/* signal */
	sop.sem_flg = 0;
	/* no special options needed */
	if (semop(m, &sop, 1) < 0) 
	{
	    fprintf(stderr,"V(): semop failed: %s\n",strerror(errno));
	    return -1;
	} 
	else 
	{
            return 0;
        }
    }

void increment(int num, int time)
{
    
    while(time)
    {
 	P(sem);
        if ( num > 0){
        	*(shm+1) += num;
        	printf("After deposit: %d\n", *(shm+1));
        	}
        else{
        	if ((*(shm+1) + num) < 0)
        	{
        	    V(sem);
        	    usleep(10);
        	    continue;
        	}
        	*(shm+1) += num;
        	printf("After withdraw: %d\n", *(shm+1));
        	}
        V(sem);
        usleep(1000);
        time--;
    }
}
int main(int argc, char *argv[])
{
    signal(SIGINT, singalHandler);
    int num, time;
    sem = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
    if (sem < 0)
    {
	fprintf(stderr, "Sem %d creation failed: %s\n", SEM_KEY,
	strerror(errno));
	exit(-1);
    }
    /* initial semaphore value to 1 (binary semaphore) */
    if ( semctl(sem, 0, SETVAL, 1) < 0 )
    {
	fprintf(stderr, "Unable to initialize Sem: %s\n", strerror(errno));
	exit(0);
    }
    
    //char buffer[BUFSIZE];
    key_t key = 1234;
    if ((shmid = shmget(key, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
    	perror("shmget");
    	exit(1);
    }
    
    if((shm = shmat(shmid, NULL, 0)) == (int*) -1)
    {
    	perror("shmat");
    	exit(1);
    }
    *(shm+1) = 0;
    
    if (argc != 2)
        errexit("Usage: %s port\n", argv[0]);
    sockfd = passivesock(argv[1], "tcp", 5);
    while(1)
    {
        /*waiting for connection*/

        connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);
        childpid = fork();
        if (childpid == 0) {
        	num = *shm;
        	time = *(shm+2);
        	increment(num, time);
        }else{
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
    int type;
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

