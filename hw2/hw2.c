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

char Report_Compare[9][50] = {" Baseball Stadium ", " Big City ", " Neiwan Old Street ", " NYCU ", " Smangus ",
" 17 km of Splendid Coastline ", " 100 Tastes ", " Science Park ", " City God Temple "};

char Report_Compare2[9][50] = {" Baseball Stadium", " Big City", " Neiwan Old Street", " NYCU", " Smangus",
" 17 km of Splendid Coastline", " 100 Tastes", " Science Park", " City God Temple"};

char Report_Compare3[9][50] = {"Baseball Stadium ", "Big City ", "Neiwan Old Street ", "NYCU ", "Smangus ",
"17 km of Splendid Coastline ", "100 Tastes ", "Science Park ", "City God Temple "};

char Report_To_Client[9][50] = {"Baseball Stadium", "Big City", "Neiwan Old Street", "NYCU", "Smangus",
"17 km of Splendid Coastline", "100 Tastes", "Science Park", "City God Temple"};

char Age_Compare[3][10] = {"Child", "Adult", "Elder"};
char menu[4][10]= {"Search", "Report", "Exit", "Menu"};
int sockfd, connfd, flag = 1, temp=0;

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
    char buffer[BUFSIZE], *tok;
    int n, i, j, search_report_exit, location, age, Report_Index[10][2], max;
    if (argc != 2)
        errexit("Usage: %s port\n", argv[0]);
    sockfd = passivesock(argv[1], "tcp", 5);
    while(1)
    {
        int people[9][7] = {0}, index, k=0;
        for (i=0; i<10; i++)
        {
            for (j=0; j<2; j++)
            {
                Report_Index[i][j] = -1;
            }
        }
        connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);
        strcpy(buffer, "1. Search\n2. Report\n3. Exit\n");
        write(connfd, buffer, strlen(buffer)+1);
        if (flag == 1)
        {
            n = read(connfd, buffer, BUFSIZE);
            tok = strtok(buffer, " |");
            flag = 0;
        }
        while(1)
        {
            n = read(connfd, buffer, BUFSIZE);
            printf("Title : %s\n", buffer);
            tok = strtok(buffer, " ");
            printf("menu : %s\n", tok);
            for (i=0; i<4; i++)
            {
                if(strcmp(tok, menu[i]) == 0)
                    search_report_exit = i;
            }
            if (search_report_exit == 1)
            {
                index = 0;
                while (tok != NULL)
                {
                    tok = strtok(NULL, "|");
                    printf("location : %s\n", tok);
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
                    tok = strtok(NULL, " ");
                    printf("age : %s\n", tok);
                    for (i=0; i<3; i++)
                    {
                        if(strcmp(tok, Age_Compare[i]) == 0)
                            {
                                Report_Index[index][1] = i;
                                break;
                            }
                    }
                    tok = strtok(NULL, " ");
                    people[Report_Index[index][0]][Report_Index[index][1]] = atoi(tok);
                    people[Report_Index[index][0]][3] += atoi(tok);
                    people[Report_Index[index][0]][4+Report_Index[index][1]] += atoi(tok);
                    printf("number : %d\n", people[Report_Index[index][0]][Report_Index[index][1]]);
                    fflush(stdout);
                    tok = strtok(NULL, " ");
                    index++;
                }
                printf("index : %d\n", index);
                max = 0;
                for (i=0; i<index; i++)
                {
                    if (Report_Index[i][0]+1 > max)
                        max = Report_Index[i][0]+1;
                }
                strcpy(buffer, "Please wait a few seconds...\n");
                write(connfd, buffer, strlen(buffer)+1);
                printf("sleep %d seconds\n\n", max);
                sleep(max+1);
                fflush(stdout);
                memset(buffer, 0, sizeof(buffer));
                for (i=0; i<index; i++)
                {
                    sprintf(buffer + strlen(buffer), "%s | %s %d\n", Report_To_Client[Report_Index[i][0]], Age_Compare[Report_Index[i][1]], people[Report_Index[i][0]][Report_Index[i][1]]);
                    printf("location index: %d age index : %d\n", Report_Index[i][0], Report_Index[i][1]);
                    fflush(stdout);
                }
                write(connfd, buffer, strlen(buffer)+1);
            }
            else if (search_report_exit == 0)
            {
                index = 0;
                tok = strtok(NULL, "|");
                if (tok == NULL)
                {
                    sprintf(buffer, "%s %d\n%s %d\n%s %d\n%s %d\n%s %d\n%s %d\n%s %d\n%s %d\n%s %d\n", "1. Baseball Stadium :", people[0][3], "2. Big City :", people[1][3]
                        , "3. Neiwan Old Street :", people[2][3], "4. NYCU :", people[3][3], "5. Smangus :", people[4][3], "6. 17 km of Splendid Coastline :", people[5][3]
                        , "7. 100 Tastes :", people[6][3], "8. Science Park :", people[7][3], "9. City God Temple :", people[8][3]);
                    write(connfd, buffer, strlen(buffer)+1);
                }
                else
                {
                    for (i=0; i<9; i++)
                    {
                        if(strcmp(tok, Report_Compare2[i]) == 0)
                            {
                                Report_Index[index][0]= i;
                                break;
                            }
                    }
                    sprintf(buffer, "%s - %s : %d %s : %d %s : %d\n", Report_To_Client[Report_Index[index][0]], Age_Compare[0], people[Report_Index[index][0]][4],Age_Compare[1],
                            people[Report_Index[index][0]][5], Age_Compare[2], people[Report_Index[index][0]][6]);
                    write(connfd, buffer, strlen(buffer)+1);
                }
            }
            else if (search_report_exit == 3)
            {
                strcpy(buffer, "1. Search\n2. Report\n3. Exit\n");
                write(connfd, buffer, strlen(buffer)+1);
            }
            else if (search_report_exit == 2)//exit
            {
                flag = 1;
                break;
            }
        }


    }

    close(connfd);
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

