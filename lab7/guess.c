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
#include <time.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

int gamepid, upper, old_upper, shmid;

typedef struct{
    int guess;
    char result[8];
}input;
input *data;

void handler (int signum)
{
    if (signum == SIGUSR2)
    {
        if (strcmp(data->result, "smaller") == 0)
        {
            old_upper = data->guess;
            data->guess/=2;
        }
        else if (strcmp(data->result, "bigger") == 0)
            data->guess = (old_upper + data->guess) / 2;
        else
        {
            shmdt(data);
            exit(0);
        }
    }
}

void timer_handler(int signum)
{
    fflush(stdout);
    printf("[game] Guess: %d\n", data->guess);
    kill(gamepid, SIGUSR1);
}

int main(int argc, char *argv[])
{
    key_t key = atoi(argv[1]);
    upper = atoi(argv[2]);
    gamepid = atoi(argv[3]);
    shmid = shmget(key, sizeof(data), 0666);
    data = shmat(shmid, NULL, 0);
    data->guess = upper / 2;
    //Handler
    struct sigaction sa1;
    memset(&sa1, 0, sizeof (sa1));
    sa1.sa_handler = handler;
    sigaction (SIGUSR2, &sa1, NULL);
    //TIMER
    struct sigaction sa;
    struct itimerval timer;
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &timer_handler;
    sigaction (SIGVTALRM, &sa, NULL);
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    setitimer (ITIMER_VIRTUAL, &timer, NULL);
    //
    fflush(stdout);
    printf("[game] Guess: %d\n", data->guess);
    kill(gamepid, SIGUSR1);
    while(1);
    return 0;
}
