#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LENGTH 50

char **drink_menu = NULL;
int drink_count = 0;

char **snack_menu = NULL;
int snack_count = 0;

char **orders = NULL;
int order_count = 0;

int check_ID()
{
    char input[30];

    while (1)
    {
        printf("Mode Select (admin or user) : ");
        scanf("%s", input);

        if (strcmp(input, "admin") == 0)
        {
            // 2번째 문제의 예시에선 로그인 후 상태를 print하지 않음.
            return 1;
        }
        else if (strcmp(input, "user") == 0)
        {
            return 0;
        }
        else
        {
            printf("Error : Wrong Input!\n");
        }
    }
}

void reset_drink_menu()
{
    FILE *file = fopen("inputs/drink1.txt", "r");
    int capacity = 10;

    drink_menu = (char **)malloc(capacity * sizeof(char *));
    char buffer[BUFFER_LENGTH];

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (drink_count >= capacity)
        {
            capacity *= 2;
            drink_menu = (char **)realloc(drink_menu, capacity * sizeof(char *));
        }

        // 개행 제거
        buffer[strcspn(buffer, "\n")] = 0;

        drink_menu[drink_count++] = strdup(buffer);
    }

    fclose(file);
}

// 스낵 메뉴 리셋 함수
void reset_snack_menu()
{
    FILE *file = fopen("inputs/snack1.txt", "r");
    int capacity = 10;

    snack_menu = (char **)malloc(capacity * sizeof(char *));
    char buffer[BUFFER_LENGTH];

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (snack_menu >= capacity)
        {
            capacity *= 2;
            snack_menu = (char **)realloc(snack_menu, capacity * sizeof(char *));
        }

        // 개행 제거
        buffer[strcspn(buffer, "\n")] = 0;

        snack_menu[snack_count++] = strdup(buffer);
    }

    fclose(file);
}

void service_mode()
{
    int input;
    int menu_index;

    int capacity = 5;
    orders = (char **)malloc(capacity * sizeof(char *));
    order_count = 0;

    // Print menu
    printf("======== drink menu ========\n");
    for (int i = 0; i < drink_count; i++)
    {
        printf("%d: %s\n", i + 1, drink_menu[i]);
    }

    printf("======== snack menu ========\n");
    for (int i = 0; i < snack_count; i++)
    {
        printf("%d: %s\n", i + 1, snack_menu[i]);
    }

    while (1)
    {
        printf("Type 1 for drink, 2 for snack, 3 for buy : ");
        scanf("%d", &input);

        // Buy
        if (input == 3)
            break;

        if (input == 1)
        {
            while (1)
            {
                printf("Type drink id to buy : ");
                scanf("%d", &menu_index);

                if (menu_index < 1 || menu_index > drink_count)
                    printf("Error : Not enough ID for drink.\n");
                else
                    break;
            }

            if (order_count >= capacity)
            {
                capacity *= 2;
                orders = (char **)realloc(orders, capacity * sizeof(char *));
            }

            orders[order_count] = (char *)malloc(strlen(drink_menu[menu_index - 1]) + 1);
            orders[order_count++] = drink_menu[menu_index - 1];
        }
        else if (input == 2)
        {
            while (1)
            {
                printf("Type snack id to buy : ");
                scanf("%d", &menu_index);

                if (menu_index < 1 || menu_index > snack_count)
                    printf("Error : Not enough ID for snack.\n");
                else
                    break;
            }

            if (order_count >= capacity)
            {
                capacity *= 2;
                orders = (char **)realloc(orders, capacity * sizeof(char *));
            }

            orders[order_count] = (char *)malloc(strlen(snack_menu[menu_index - 1]) + 1);
            orders[order_count++] = snack_menu[menu_index - 1];
        }
        else
        {
            printf("Error : Wrong input for type select!\n");
            continue;
        }
    }

    printf("======== Receipt ========\n");
    for (int i = 0; i < order_count; i++)
    {
        printf("%s\n", orders[i]);
    }

    // Free orders
    for (int i = 0; i < order_count; i++)
    {
        free(orders[i]);
    }
    free(orders);
}

int main()
{
    reset_drink_menu();
    reset_snack_menu();

    int mode = check_ID();

    if (mode == 1)
    {
        // todo
    }
    else
    {
        service_mode();
    }

    // Free drink, snack menus
    for (int i = 0; i < drink_count; i++)
    {
        free(drink_menu[i]);
    }
    free(drink_menu);

    for (int i = 0; i < snack_count; i++)
    {
        free(snack_menu[i]);
    }
    free(snack_menu);

    return 0;
}