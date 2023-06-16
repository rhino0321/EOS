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
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdatomic.h>
#include <pthread.h>
#define errexit(format,arg...) exit(printf(format,##arg))
#define BUFSIZE 1024
#define SEM_MODE 0666 /* rw(owner)-rw(group)-rw(other) permission */
#define SEM_KEY 1122334455
#define SEM_KEY1 11223344
#define SEM_KEY2 11223355

pid_t childpid, childpid2, sleeppid, sleeppid1;

int sockfd, sem, sem1, sem2, flag[2], car1_old_sleep, car2_old_sleep;

int shmid1;

struct sockaddr_in addr_cln;
socklen_t sLen = sizeof(addr_cln);

/*Create server*/
int passivesock(const char *service, const char *transport, int qlen);

/*Connect to server*/
int connectsock(const char *host, const char *service, const char *transport);

typedef struct waitlist
{
    int location[100];
    int totalwait;
    pid_t childpid2[50];
    atomic_int car_num[2];
    int sleepwaitlist[50];
    int sleepwait;
    int sleep_index;
    int people[9][7];
} waitlist;

waitlist *shm_wait;

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
void* car1(void* arg)
{
    int k;
    printf("car 1 sleep %d\n", shm_wait->sleepwaitlist[car1_old_sleep]);
    sleep(shm_wait->sleepwaitlist[car1_old_sleep]);
    //P(sem);
    int bus1_k = 0;
    for (k=0; k<shm_wait->totalwait; k++)
    {
        if (shm_wait->sleepwaitlist[car1_old_sleep] == shm_wait->location[k])
        {
            kill(shm_wait->childpid2[k], SIGUSR1);
        }
        else
        {
            shm_wait->location[bus1_k] = shm_wait->location[k];
            shm_wait->childpid2[bus1_k] = shm_wait->childpid2[k];
            bus1_k++;
        }
    }
    shm_wait->sleepwaitlist[car1_old_sleep] = 0;
    shm_wait->totalwait = bus1_k;
    //V(sem);
    flag[0]=1;
    return NULL;
}
void* car2(void *arg)
{
    int k;
    printf("car 2 sleep %d\n", shm_wait->sleepwaitlist[car2_old_sleep]);
    sleep(shm_wait->sleepwaitlist[car2_old_sleep]);
    //P(sem);
    int bus2_k = 0;
    for (k=0; k<shm_wait->totalwait; k++)
    {
        if (shm_wait->sleepwaitlist[car2_old_sleep] == shm_wait->location[k])
        {
            kill(shm_wait->childpid2[k], SIGUSR1);
        }
        else
        {
            shm_wait->location[bus2_k] = shm_wait->location[k];
            shm_wait->childpid2[bus2_k] = shm_wait->childpid2[k];
            bus2_k++;
        }
    }
    shm_wait->sleepwaitlist[car2_old_sleep] = 0;
    shm_wait->totalwait = bus2_k;
    //V(sem);
    flag[1]=1;
    return NULL;
}
void cars()
{
    int i, j, k;
    pthread_t tid[2];
    pid_t id;
    while (1)
    {
        if (flag[0] && shm_wait->sleepwaitlist[shm_wait->sleep_index] > 0)
        {
            printf("before car_num %d car 1 sleep %d\n", flag[0], shm_wait->sleepwaitlist[shm_wait->sleep_index]);
            flag[0] = 0;
            //P(sem1);
            //shm_wait->car_num[0] = 0;
            printf("after car_num %d car 1 sleep %d\n", flag[0], shm_wait->sleepwaitlist[shm_wait->sleep_index]);
            car1_old_sleep = shm_wait->sleep_index;
            shm_wait->sleep_index++;
            printf("sleep index %d\n", shm_wait->sleep_index);
            //printf("END car_num %d\n", shm_wait->car_num[0]);
            //V(sem1);
            pthread_create(&tid[0], NULL, car1, NULL);
            //while (shm_wait->car_num[0] > 0);
        }
        else if (flag[1] && shm_wait->sleepwaitlist[shm_wait->sleep_index] > 0)
        {
            //usleep(1000);
            //P(sem1);
            printf("car_num %d car 2 sleep %d\n", flag[1], shm_wait->sleepwaitlist[shm_wait->sleep_index]);
            flag[1]=0;
            //shm_wait->car_num[1] = 0;
            printf("car_num %d car 2 sleep %d\n", flag[1], shm_wait->sleepwaitlist[shm_wait->sleep_index]);
            car2_old_sleep = shm_wait->sleep_index;
            shm_wait->sleep_index++;
            printf("sleep index %d\n", shm_wait->sleep_index);
            //V(sem1);
            pthread_create(&tid[1], NULL, car2, NULL);
            //while (shm_wait->car_num[1] > 0);
        }
    }
}

char Report_Compare[9][50] = {" Baseball Stadium ", " Big City ", " Neiwan Old Street ", " NYCU ", " Smangus ",
" 17 km of Splendid Coastline ", " 100 Tastes ", " Science Park ", " City God Temple "};

char Report_Compare2[9][50] = {" Baseball Stadium\n", " Big City\n", " Neiwan Old Street\n", " NYCU\n", " Smangus\n",
" 17 km of Splendid Coastline\n", " 100 Tastes\n", " Science Park\n", " City God Temple\n"};

char Report_Compare3[9][50] = {"Baseball", "Big", "Neiwan", "NYCU", "Smangus",
"17", "100", "Science", "City"};

char Report_To_Client[9][50] = {"Baseball Stadium", "Big City", "Neiwan Old Street", "NYCU", "Smangus",
"17 km of Splendid Coastline", "100 Tastes", "Science Park", "City God Temple"};

char Age_Compare[3][10] = {"Child", "Adult", "Elder"};
char menu[4][10]= {"Search", "Report", "Exit", "Menu"};


int compare(const void* a, const void* b) {
    int num1 = *(int*)a;
    int num2 = *(int*)b;

    if (num1 < num2)
    {
        return -1;
    }
    else if (num1 > num2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void singalHandler(int signum)
{
        semctl(sem, 0, IPC_RMID, 0);
        semctl(sem1, 0, IPC_RMID, 0);
        semctl(sem2, 0, IPC_RMID, 0);
        shmdt(shm_wait);
        shmctl(shmid1, IPC_RMID, NULL);
        kill(childpid, SIGTERM);
        close(sockfd);
        exit(0);
}

void signal_handler(int signum)
{

}

void core(int connfd)
{
    int flag = 1, n, i, j, search_report_exit, Report_Index[20][2], max, index, k=0;
    char buffer[BUFSIZE], buffer1[BUFSIZE], buffer2[10][BUFSIZE];
    char *tok;
    for (i=0; i<10; i++)
    {
        for (j=0; j<2; j++)
        {
            Report_Index[i][j] = -1;
        }
    }
    while(1)
    {
        n = read(connfd, buffer, BUFSIZE);
        /*if (strlen(buffer) == 0)
            break;*/
        printf("title: %s pid : %d\n", buffer, getpid());
        tok = strtok(buffer, " |");
        if (strcmp(tok, "Menu\n") == 0)
        {
            //printf("Title : %s\n", tok);
            strcpy(buffer, "1. Search\n2. Report\n3. Exit\n");
            write(connfd, buffer, strlen(buffer)+1);
            continue;
        }
        //printf("menu : %s\n", tok);
        for (i=0; i<4; i++)
        {
            if(strcmp(tok, menu[i]) == 0)
                search_report_exit = i;
        }
        if (search_report_exit == 1)
        {
            index = 0;
            flag = 1;
            for (i=0; i<10; i++)
            {
                memset(buffer2[i], 0, sizeof(buffer2[i]));
            }
            while (tok != NULL)
            {
                int same = 0, same_location[3];
                same_location[0] = 0;
                same_location[1] = 0;
                same_location[2] = 0;
                if (flag)
                {
                    tok = strtok(NULL, "|");
                    flag = 0;
                }
                for (i=0; i<9; i++)
                {
                    if (index == 0)
                    {
                        if(strcmp(tok, Report_Compare[i]) == 0)
                        {
                            Report_Index[index][0]= i;
                            break;
                        }
                    }
                    else
                    {
                        if(strcmp(tok, Report_Compare3[i]) == 0)
                        {
                            Report_Index[index][0]= i;
                            break;
                        }
                    }
                }
                sprintf(buffer2[index] + strlen(buffer2[index]), "%s", Report_To_Client[Report_Index[index][0]]);
                out1:;
                int flag2 = 1;
                while (flag2)
                {
                    tok = strtok(NULL, " ");
                    for (i=0; i<3; i++)
                    {
                        if (strcmp(tok, Age_Compare[i]) == 0)
                            flag2 = 0;
                    }
                }
                for (i=0; i<3; i++)
                {
                    int goto_location = 0;
                    if(strcmp(tok, Age_Compare[i]) == 0)
                    {
                        sprintf(buffer2[index] + strlen(buffer2[index]), " | %s ", Age_Compare[i]);
                        tok = strtok(NULL, " ");
                        same_location[i] += atoi(tok);
                        shm_wait->people[Report_Index[index][0]][i] = atoi(tok);
                        sprintf(buffer2[index] + strlen(buffer2[index]), "%d", shm_wait->people[Report_Index[index][0]][i]);
                        shm_wait->people[Report_Index[index][0]][3] += atoi(tok);
                        shm_wait->people[Report_Index[index][0]][4+i] += atoi(tok);
                        tok = strtok(NULL, " ");
                        if (tok == NULL)
                        {
                            goto out;
                        }
                        tok = strtok(NULL, " ");
                        for (j=0 ;j<3; j++)
                        {
                            if (strcmp(tok, Age_Compare[j]) == 0)
                            {
                                same = 1;
                                tok = strtok(NULL, " ");
                                same_location[j] += atoi(tok);
                                tok = strtok(NULL, " ");
                                if (tok == NULL)
                                {
                                    goto out;
                                }
                                tok = strtok(NULL, " ");
                                printf("%s\n", tok);
                            }
                        }
                        for (i=0; i<9; i++)
                        {
                            if (strcmp(tok, Report_Compare3[i]) == 0)
                            {
                                if (i == Report_Index[index][0])
                                {
                                    same = 1;
                                    goto out1;
                                }
                            }
                        }
                        goto out;
                    }
                }
                out:;
                if (same == 1)
                {
                    memset(buffer2[index], 0, sizeof(buffer2));
                    sprintf(buffer2[index] + strlen(buffer2[index]), "%s", Report_To_Client[Report_Index[index][0]]);
                    for (i=0; i<3; i++)
                    {
                        if (same_location[i] != 0)
                        {
                            sprintf(buffer2[index] + strlen(buffer2[index]), " | %s ", Age_Compare[i]);
                            sprintf(buffer2[index] + strlen(buffer2[index]), "%d", same_location[i]);
                        }
                    }
                }
                sprintf(buffer2[index] + strlen(buffer2[index]), "\n");
                index++;
            }
            P(sem);
            max = 0;
            for (i=0; i<index; i++)
            {
                if (Report_Index[i][0]+1 > max)
                    max = Report_Index[i][0]+1;
            }
            shm_wait->location[shm_wait->totalwait] = max;
            if (shm_wait->sleepwait == 0)
            {
                shm_wait->sleepwaitlist[shm_wait->sleepwait] = max;
                printf("waitlist[%d] = %d\n", shm_wait->sleepwait, shm_wait->sleepwaitlist[shm_wait->sleepwait]);
                shm_wait->sleepwait++;
            }
            for (i=0; i<(shm_wait->sleepwait); i++)
            {
                if (shm_wait->sleepwaitlist[i] == max)
                    break;
                if (i == (shm_wait->sleepwait) - 1)
                {
                    shm_wait->sleepwaitlist[shm_wait->sleepwait] = max;
                    printf("waitlist[%d] = %d\n", shm_wait->sleepwait, shm_wait->sleepwaitlist[shm_wait->sleepwait]);
                    (shm_wait->sleepwait)++;
                    break;
                }
            }
            shm_wait->childpid2[shm_wait->totalwait] = getpid();
            shm_wait->totalwait++;
            V(sem);
            pause();
            strcpy(buffer1, "The shuttle bus is on it's way...\n");
            write(connfd, buffer1, strlen(buffer1)+1);
            memset(buffer1, 0, sizeof(buffer1));
            fflush(stdout);
            memset(buffer, 0, sizeof(buffer));
            for (i=0; i<index; i++)
            {
                int temp;
                int min = Report_Index[i][0];
                char str[100];
                for (j=i+1; j<index; j++)
                {
                    if (Report_Index[j][0] < Report_Index[i][0])
                    {
                        strcpy(str, buffer2[i]);
                        strcpy(buffer2[i], buffer2[j]);
                        strcpy(buffer2[j], str);
                        memset(str, 0, sizeof(str));
                    }
                }
            }
            for (i=0; i<index; i++)
            {
                strcat(buffer1, buffer2[i]);
            }
            printf("pid : %d write to client %s\n", getpid(), buffer1);
            write(connfd, buffer1, strlen(buffer1)+1);
        }
        else if (search_report_exit == 0)
        {
            index = 0;
            tok = strtok(NULL, "|");
            if (tok == NULL)
            {
                sprintf(buffer, "%s %d\n%s %d\n%s %d\n%s %d\n%s %d\n%s %d\n%s %d\n%s %d\n%s %d\n", "1. Baseball Stadium :", shm_wait->people[0][3], "2. Big City :", shm_wait->people[1][3]
                    , "3. Neiwan Old Street :", shm_wait->people[2][3], "4. NYCU :", shm_wait->people[3][3], "5. Smangus :", shm_wait->people[4][3], "6. 17 km of Splendid Coastline :", shm_wait->people[5][3]
                    , "7. 100 Tastes :", shm_wait->people[6][3], "8. Science Park :", shm_wait->people[7][3], "9. City God Temple :", shm_wait->people[8][3]);
                write(connfd, buffer, strlen(buffer)+1);
            }
            else
            {
                printf("tok6%s6\n", tok);
                for (i=0; i<9; i++)
                {
                    if(strcmp(tok, Report_Compare2[i]) == 0)
                        {
                            Report_Index[index][0]= i;
                            break;
                        }
                }
                printf("search index %d\n", Report_Index[index][0]);
                sprintf(buffer, "%s - %s : %d | %s : %d | %s : %d\n", Report_To_Client[Report_Index[index][0]], Age_Compare[0], shm_wait->people[Report_Index[index][0]][4],Age_Compare[1],
                        shm_wait->people[Report_Index[index][0]][5], Age_Compare[2], shm_wait->people[Report_Index[index][0]][6]);
                write(connfd, buffer, strlen(buffer)+1);
            }
        }

        else//exit
        {
            break;
        }
    }
}
int main(int argc, char *argv[])
{
    int connfd, m;
    signal(SIGINT, singalHandler);
    signal(SIGUSR1, signal_handler);
    sem = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
    sem1 = semget(SEM_KEY1, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
    sem2 = semget(SEM_KEY2, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
    sockfd = passivesock(argv[1], "tcp", 5);
    semctl(sem, 0, SETVAL, 1);
    semctl(sem1, 0, SETVAL, 1);
    semctl(sem2, 0, SETVAL, 1);
    key_t key2 = 5678;
    shmid1 = shmget(key2, sizeof(waitlist), IPC_CREAT | 0666);
    shm_wait = shmat(shmid1, NULL, 0);
    shm_wait->totalwait = 0;
    shm_wait->sleepwait = 0;
    atomic_store(&shm_wait->car_num[0], 1);
    atomic_store(&shm_wait->car_num[1], 1);
    shm_wait->sleep_index = 0;
    shm_wait->people[9][0] = 0;
    shm_wait->people[9][1] = 0;
    shm_wait->people[9][2] = 0;
    shm_wait->people[9][3] = 0;
    shm_wait->people[9][4] = 0;
    shm_wait->people[9][5] = 0;
    shm_wait->people[9][6] = 0;
    flag[0] = 1;
    flag[1] = 1;
    for(m=0; m<50; m++)
    {
        shm_wait->sleepwaitlist[m] = 0;
    }
    sleeppid = fork();
    if (sleeppid == 0)
    {
        cars();
        exit(0);
    }
    while(1)
    {
        connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);
        childpid = fork();
        if(childpid == 0)
        {
            core(connfd);
        }
    }
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
