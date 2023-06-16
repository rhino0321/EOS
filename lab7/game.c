#include <arpa/inet.h>
#include <signal.h>
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

int ans, shmid;

typedef struct{
    int guess;
    char result[8];
}input;

input *data;

void singalHandler(int signum)
{
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
        exit(0);
}

void handler(int signo, siginfo_t *info, void *context)
{
    if (data->guess == ans)
        strcpy(data->result, "bingo");
    else if (data->guess > ans)
        strcpy(data->result, "smaller");
    else
        strcpy(data->result, "bigger");
    printf("[game] Guess %d, %s\n", data->guess, data->result);
    kill(info->si_pid, SIGUSR2);
}


int main(int argc, char *argv[])
{
    printf("[game] Game PID: %d\n", getpid());
    key_t key = atoi(argv[1]);
    ans = atoi(argv[2]);
    signal(SIGINT, singalHandler);
    struct sigaction my_action;
    memset(&my_action, 0, sizeof(struct sigaction));
    my_action.sa_flags = SA_SIGINFO;
    my_action.sa_sigaction = handler;
    sigaction(SIGUSR1, &my_action, NULL);
    shmid = (shmget(key, sizeof(data), IPC_CREAT | 0666));
    data = shmat(shmid, NULL, 0);
    while(1);
    return 0;
}
