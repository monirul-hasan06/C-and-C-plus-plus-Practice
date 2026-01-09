#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#define clrscr() system("cls")
#define sleep(x) Sleep(x * 1000)
#else
#include <unistd.h>
#define clrscr() printf("\e[1;1H\e[2J")
#endif

int main()
{
    int hour, minute, second;
    printf("Enter Hour Minute Second: ");
    scanf("%d %d %d", &hour, &minute, &second);

    clrscr();
    int h = 0, m = 0, s = 0;

    while (1)
    {
        printf("%.2d:%.2d:%.2d\n", h, m, s);

        if (h == hour && m == minute && s == second)
        {
            break;
        }

        sleep(1);
        clrscr();

        s++;
        if (s == 60)
        {
            s = 0;
            m++;
        }
        if (m == 60)
        {
            m = 0;
            h++;
        }
        if (h == 24)
        {
            h = 0;
        }
    }

    printf("Reached Time: %.2d:%.2d:%.2d\n", hour, minute, second);
    return 0;
}
