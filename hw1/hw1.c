#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>

#define gpio_12_export "/sys/class/gpio/export"
#define gpio_1_export "/sys/class/gpio/export"
#define gpio_13_export "/sys/class/gpio/export"
#define gpio_19_export "/sys/class/gpio/export"
#define gpio_26_export "/sys/class/gpio/export"
#define gpio_16_export "/sys/class/gpio/export"
#define gpio_21_export "/sys/class/gpio/export"
#define gpio_25_export "/sys/class/gpio/export"
#define gpio_8_export "/sys/class/gpio/export"
#define gpio_7_export "/sys/class/gpio/export"

#define gpio_12_dir "/sys/class/gpio/gpio12/direction"
#define gpio_1_dir "/sys/class/gpio/gpio1/direction"
#define gpio_13_dir "/sys/class/gpio/gpio13/direction"
#define gpio_19_dir "/sys/class/gpio/gpio19/direction"
#define gpio_26_dir "/sys/class/gpio/gpio26/direction"
#define gpio_16_dir "/sys/class/gpio/gpio16/direction"
#define gpio_21_dir "/sys/class/gpio/gpio21/direction"
#define gpio_25_dir "/sys/class/gpio/gpio25/direction"
#define gpio_8_dir "/sys/class/gpio/gpio8/direction"
#define gpio_7_dir "/sys/class/gpio/gpio7/direction"
#define out2 "out"

#define gpio_12 "/sys/class/gpio/gpio12/value"
#define gpio_1 "/sys/class/gpio/gpio1/value"
#define gpio_13 "/sys/class/gpio/gpio13/value"
#define gpio_19 "/sys/class/gpio/gpio19/value"
#define gpio_26 "/sys/class/gpio/gpio26/value"
#define gpio_16 "/sys/class/gpio/gpio16/value"
#define gpio_21 "/sys/class/gpio/gpio21/value"
#define gpio_25 "/sys/class/gpio/gpio25/value"
#define gpio_8 "/sys/class/gpio/gpio8/value"
#define gpio_7 "/sys/class/gpio/gpio7/value"


void set_gpio()
{
    int fd;

    fd = open(gpio_12_export, O_WRONLY);
    write(fd, "12", sizeof("12"));
    fd = open(gpio_12_dir, O_WRONLY);
    write(fd, out2, sizeof(out2));

    fd = open(gpio_1_export, O_WRONLY);
    write(fd, "1", sizeof("1"));
    close(fd);
    fd = open(gpio_1_dir, O_WRONLY);
    write(fd, out2, sizeof(out2));

    fd = open(gpio_13_export, O_WRONLY);
    write(fd, "13", sizeof("13"));
    fd = open(gpio_13_dir, O_WRONLY);
    write(fd, out2, sizeof(out2));

    fd = open(gpio_19_export, O_WRONLY);
    write(fd, "19", sizeof("19"));
    fd = open(gpio_19_dir, O_WRONLY);
    write(fd, out2, sizeof(out2));

    fd = open(gpio_26_export, O_WRONLY);
    write(fd, "26", sizeof("26"));
    fd = open(gpio_26_dir, O_WRONLY);
    write(fd, out2, sizeof(out2));

    fd = open(gpio_16_export, O_WRONLY);
    write(fd, "16", sizeof("16"));
    fd = open(gpio_16_dir, O_WRONLY);
    write(fd, out2, sizeof(out2));

    fd = open(gpio_21_export, O_WRONLY);
    write(fd, "21", sizeof("21"));
    fd = open(gpio_21_dir, O_WRONLY);
    write(fd, out2, sizeof(out2));

    fd = open(gpio_25_export, O_WRONLY);
    write(fd, "25", sizeof("25"));
    fd = open(gpio_25_dir, O_WRONLY);
    write(fd, out2, sizeof(out2));

    fd = open(gpio_8_export, O_WRONLY);
    write(fd, "8", sizeof("8"));
    fd = open(gpio_8_dir, O_WRONLY);
    write(fd, out2, sizeof(out2));

    fd = open(gpio_7_export, O_WRONLY);
    write(fd, "7", sizeof("7"));
    fd = open(gpio_7_dir, O_WRONLY);
    write(fd, out2, sizeof(out2));

}
char sevenseg[11][8] = {
	"1111110", //0
	"0110000", //1
	"1101101", //2
	"1111001", //3
	"0110011", //4
	"1011011", //5
	"1011111", //6
	"1110000", //7
	"1111111", //8
	"1111011", //9
	"0000000"  //dark
};

void *seg(void *threadid)
{
    int fd, num;
    num = *(int *)threadid;
    fd=open(gpio_12, O_RDWR);
    if (sevenseg[num][0]%2)
            write(fd, "1", sizeof("1"));
    else
            write(fd, "0", sizeof("0"));
    fd=open(gpio_1, O_RDWR);
    if (sevenseg[num][1]%2)
            write(fd, "1", sizeof("1"));
    else
            write(fd, "0", sizeof("0"));

    fd=open(gpio_13, O_RDWR);
    if (sevenseg[num][2]%2)
            write(fd, "1", sizeof("1"));
    else
            write(fd, "0", sizeof("0"));

    fd=open(gpio_19, O_RDWR);
    if (sevenseg[num][3]%2)
            write(fd, "1", sizeof("1"));
    else
            write(fd, "0", sizeof("0"));
    fd=open(gpio_26, O_RDWR);
    if (sevenseg[num][4]%2)
            write(fd, "1", sizeof("1"));
    else
            write(fd, "0", sizeof("0"));
    fd=open(gpio_16, O_RDWR);
    if (sevenseg[num][5]%2)
            write(fd, "1", sizeof("1"));
    else
            write(fd, "0", sizeof("0"));
    fd=open(gpio_21, O_RDWR);
    if (sevenseg[num][6]%2)
            write(fd, "1", sizeof("1"));
    else
            write(fd, "0", sizeof("0"));
    pthread_exit(0);
}

void led(int n1,int n2,int n3)
{
    int fd;
    fd=open(gpio_25, O_RDWR);
    if (n1)
        write(fd, "1", sizeof("1"));
    else
        write(fd, "0", sizeof("0"));
    fd=open(gpio_8, O_RDWR);
    if (n2)
        write(fd, "1", sizeof("1"));
    else
        write(fd, "0", sizeof("0"));
    fd=open(gpio_7, O_RDWR);
    if (n3)
        write(fd, "1", sizeof("1"));
    else
        write(fd, "0", sizeof("0"));
}

int digits_of_ten(int num)
{
    int digits=0;
    while(num/10 != 0)
    {
        digits++;
        num/=10;
    }
    return digits;
}


int main()
{
    set_gpio();
    int Baseball_Stadium_People[3]={0}, Big_City[3]={0}, Zoo[3]={0}, dark=11, Choose_Area, Search_Report_Exit=0, Number_Passenger, k, i, pow, flag, fd, i1=0, i2=0, i3=0;
    char aqec, Baseball_Big_Zoo, Age_Group, Add_Reduce, Exit_Continue;
    while(1)
    {
        void *status;
        printf("1. Search\n");
        printf("2. Report\n");
        printf("3. Exit\n\n");
        scanf("%d", &Search_Report_Exit);
        pthread_t thread;
        out:;
        if (Search_Report_Exit == 1)
        {
            printf("1. Baseball Stadium : %d\n", Baseball_Stadium_People[0]+Baseball_Stadium_People[1]+Baseball_Stadium_People[2]);
            printf("2. Big City : %d\n", Big_City[0]+Big_City[1]+Big_City[2]);
            printf("3. Zoo : %d\n\n", Zoo[0]+Zoo[1]+Zoo[2]);
            led(Baseball_Stadium_People[0]+Baseball_Stadium_People[1]+Baseball_Stadium_People[2], Big_City[0]+Big_City[1]+Big_City[2], Zoo[0]+Zoo[1]+Zoo[2]);
            i = Baseball_Stadium_People[0]+Baseball_Stadium_People[1]+Baseball_Stadium_People[2]+Big_City[0]+Big_City[1]+Big_City[2]+Zoo[0]+Zoo[1]+Zoo[2];
            for(k=0, pow=1; k<digits_of_ten(i); k++)
            {
                pow*=10;
            }
            for(k=digits_of_ten(i); k>=0; k--)
            {
                flag= i/pow;
                pthread_create(&thread, NULL, seg, &flag);
                pthread_join(thread, &status);
                sleep(1);
                i%=pow;
                pow/=10;
            }
            if (Baseball_Stadium_People[0]+Baseball_Stadium_People[1]+Baseball_Stadium_People[2]+Big_City[0]+Big_City[1]+Big_City[2]+Zoo[0]+Zoo[1]+Zoo[2] >= 10)
            {
                pthread_create(&thread, NULL, seg, &dark);
                pthread_join(thread, &status);
            }
            out1:;
            scanf(" %c", &Baseball_Big_Zoo);
            if (Baseball_Big_Zoo == '1')
            {
                printf("Child : %d\n", Baseball_Stadium_People[0]);
                printf("Adult : %d\n", Baseball_Stadium_People[1]);
                printf("Elder : %d\n\n", Baseball_Stadium_People[2]);
                fd=open(gpio_7, O_RDWR);
                write(fd, "0", sizeof("0"));
                fd=open(gpio_8, O_RDWR);
                write(fd, "0", sizeof("0"));
                for (k=0; k<3; k++)
                {
                    fd=open(gpio_25, O_RDWR);
                    write(fd, "0", sizeof("0"));
                    usleep(500*1000);
                    write(fd, "1", sizeof("1"));
                    usleep(500*1000);
                };
                i1=Baseball_Stadium_People[0]+Baseball_Stadium_People[1]+Baseball_Stadium_People[2];
                for(k=0, pow=1; k<digits_of_ten(i1); k++)
                {
                    pow*=10;
                }
                for(k=digits_of_ten(i1); k>=0; k--)
                {
                    flag= i1/pow;
                    pthread_create(&thread, NULL, seg, &flag);
                    pthread_join(thread, &status);
                    sleep(1);
                    i1%=pow;
                    pow/=10;
                }
                if (Baseball_Stadium_People[0]+Baseball_Stadium_People[1]+Baseball_Stadium_People[2] >= 10)
                {
                    pthread_create(&thread, NULL, seg, &dark);
                    pthread_join(thread, &status);
                }
                scanf(" %c", &aqec);
                goto out;
            }
            else if (Baseball_Big_Zoo == '2')
            {
                printf("Child : %d\n", Big_City[0]);
                printf("Adult : %d\n", Big_City[1]);
                printf("Elder : %d\n\n", Big_City[2]);
                fd=open(gpio_25, O_RDWR);
                write(fd, "0", sizeof("0"));
                fd=open(gpio_7, O_RDWR);
                write(fd, "0", sizeof("0"));
                for (k=0; k<3; k++)
                {
                    fd=open(gpio_8, O_RDWR);
                    write(fd, "0", sizeof("0"));
                    usleep(500*1000);
                    write(fd, "1", sizeof("1"));
                    usleep(500*1000);
                }
                i2=Big_City[0]+Big_City[1]+Big_City[2];
                for(k=0, pow=1; k<digits_of_ten(i2); k++)
                {
                    pow*=10;
                }
                for(k=digits_of_ten(i2); k>=0; k--)
                {
                    flag= i2/pow;
                    pthread_create(&thread, NULL, seg, &flag);
                    pthread_join(thread, &status);
                    sleep(1);
                    i2%=pow;
                    pow/=10;
                }
                if (Big_City[0]+Big_City[1]+Big_City[2] >= 10)
                {
                    pthread_create(&thread, NULL, seg, &dark);
                    pthread_join(thread, &status);
                }
                scanf(" %c", &aqec);
                goto out;
            }
            else if (Baseball_Big_Zoo == '3')
            {
                printf("Child : %d\n", Zoo[0]);
                printf("Adult : %d\n", Zoo[1]);
                printf("Elder : %d\n\n", Zoo[2]);
                fd=open(gpio_8, O_RDWR);
                write(fd, "0", sizeof("0"));
                fd=open(gpio_25, O_RDWR);
                write(fd, "0", sizeof("0"));
                for (k=0; k<3; k++)
                {
                    fd=open(gpio_7, O_RDWR);
                    write(fd, "0", sizeof("0"));
                    usleep(500*1000);
                    write(fd, "1", sizeof("1"));
                    usleep(500*1000);
                }
                i3=Zoo[0]+Zoo[1]+Zoo[2];
                for(k=0, pow=1; k<digits_of_ten(i3); k++)
                {
                    pow*=10;
                }
                for(k=digits_of_ten(i3); k>=0; k--)
                {
                    flag= i3/pow;
                    pthread_create(&thread, NULL, seg, &flag);
                    pthread_join(thread, &status);
                    sleep(1);
                    i3%=pow;
                    pow/=10;
                }
                if (Zoo[0]+Zoo[1]+Zoo[2] >= 10)
                {
                    pthread_create(&thread, NULL, seg, &dark);
                    pthread_join(thread, &status);
                }
                scanf(" %c", &aqec);
                goto out;
            }
            else if (Baseball_Big_Zoo == 'q')
            {
                continue;
            }
            else
                goto out1;
        }
        else if (Search_Report_Exit == 2)
        {
            printf("Area (1~3) : ");
            scanf(" %d", &Choose_Area);
            printf("Add or Reduce ('a' or 'r') : ");
            scanf(" %c", &Add_Reduce);
            printf("Age group ('c' or 'a' or 'e') : ");
            scanf(" %c", &Age_Group);
            printf("The number of passenger : ");
            scanf(" %d", &Number_Passenger);
            printf("\n(Exit or Continue)");
            switch(Choose_Area)
            {
                case 1:
                if (Add_Reduce == 'a' && Age_Group == 'c')
                    Baseball_Stadium_People[0]+=Number_Passenger;
                else if (Add_Reduce == 'a' && Age_Group == 'a')
                    Baseball_Stadium_People[1]+=Number_Passenger;
                else if (Add_Reduce == 'a' && Age_Group == 'e')
                    Baseball_Stadium_People[2]+=Number_Passenger;
                else if (Add_Reduce == 'r' && Age_Group == 'c')
                    Baseball_Stadium_People[0]-=Number_Passenger;
                else if (Add_Reduce == 'r' && Age_Group == 'a')
                    Baseball_Stadium_People[1]-=Number_Passenger;
                else if (Add_Reduce == 'r' && Age_Group == 'e')
                    Baseball_Stadium_People[2]-=Number_Passenger;
                break;
                case 2:
                if (Add_Reduce == 'a' && Age_Group == 'c')
                    Big_City[0]+=Number_Passenger;
                else if (Add_Reduce == 'a' && Age_Group == 'a')
                    Big_City[1]+=Number_Passenger;
                else if (Add_Reduce == 'a' && Age_Group == 'e')
                    Big_City[2]+=Number_Passenger;
                else if (Add_Reduce == 'r' && Age_Group == 'c')
                    Big_City[0]-=Number_Passenger;
                else if (Add_Reduce == 'r' && Age_Group == 'a')
                    Big_City[1]-=Number_Passenger;
                else if (Add_Reduce == 'r' && Age_Group == 'e')
                    Big_City[2]-=Number_Passenger;
                break;
                case 3:
                if (Add_Reduce == 'a' && Age_Group == 'c')
                    Zoo[0]+=Number_Passenger;
                else if (Add_Reduce == 'a' && Age_Group == 'a')
                    Zoo[1]+=Number_Passenger;
                else if (Add_Reduce == 'a' && Age_Group == 'e')
                    Zoo[2]+=Number_Passenger;
                else if (Add_Reduce == 'r' && Age_Group == 'c')
                    Zoo[0]-=Number_Passenger;
                else if (Add_Reduce == 'r' && Age_Group == 'a')
                    Zoo[1]-=Number_Passenger;
                else if (Add_Reduce == 'r' && Age_Group == 'e')
                    Zoo[2]-=Number_Passenger;
                break;
            }
            if (Choose_Area == 1)
            {
                fd=open(gpio_25, O_RDWR);
                write(fd, "1", sizeof("1"));
                fd=open(gpio_8, O_RDWR);
                write(fd, "0", sizeof("0"));
                fd=open(gpio_7, O_RDWR);
                write(fd, "0", sizeof("0"));
            }
            else if (Choose_Area == 2)
            {

                fd=open(gpio_25, O_RDWR);
                write(fd, "0", sizeof("0"));
                fd=open(gpio_8, O_RDWR);
                write(fd, "1", sizeof("1"));
                fd=open(gpio_7, O_RDWR);
                write(fd, "0", sizeof("0"));
            }
            else
            {
                fd=open(gpio_25, O_RDWR);
                write(fd, "0", sizeof("0"));
                fd=open(gpio_8, O_RDWR);
                write(fd, "0", sizeof("0"));
                fd=open(gpio_7, O_RDWR);
                write(fd, "1", sizeof("1"));
            }
            i = Baseball_Stadium_People[0]+Baseball_Stadium_People[1]+Baseball_Stadium_People[2]+Big_City[0]+Big_City[1]+Big_City[2]+Zoo[0]+Zoo[1]+Zoo[2];
            for(k=0, pow=1; k<digits_of_ten(i); k++)
            {
                pow*=10;
            }
            for(k=digits_of_ten(i); k>=0; k--)
            {
                flag= i/pow;
                pthread_create(&thread, NULL, seg, &flag);
                pthread_join(thread, &status);
                sleep(1);
                i%=pow;
                pow/=10;
            }
            if (Baseball_Stadium_People[0]+Baseball_Stadium_People[1]+Baseball_Stadium_People[2]+Big_City[0]+Big_City[1]+Big_City[2]+Zoo[0]+Zoo[1]+Zoo[2] >= 10)
            {
                pthread_create(&thread, NULL, seg, &dark);
                pthread_join(thread, &status);
            }
            scanf(" %c", &Exit_Continue);
            if (Exit_Continue == 'e')
            {
                    continue;
            }
            else
            {
                    goto out;
            }
        }
        else
        {
            led(0, 0, 0);
            pthread_create(&thread, NULL, seg, &dark);
            pthread_join(thread, &status);
            break;
        }

        }
        return 0;
}

