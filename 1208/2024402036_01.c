#include <stdio.h>
#include <string.h>

// 로그인 아이디 확인 함수
int check_ID()
{
    char input[30];

    while (1)
    {
        printf("Mode Select (admin or user) : ");
        scanf("%s", input);

        // admin 또는 user 아이디 확인
        if (strcmp(input, "admin") == 0)
        {
            printf("Admin mode\n");
            return 1;
        }
        else if (strcmp(input, "user") == 0)
        {
            printf("User mode\n");
            return 0;
        }
        else
        {
            printf("Error : Wrong Input!\n");
        }
    }
}

int main()
{
    int mode;

    mode = check_ID();

    return 0;
}