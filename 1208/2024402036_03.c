#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LENGTH 50

typedef struct {
    int id;
    char* name;
    int price;
    int stock;
} MenuItem;

typedef struct {
    MenuItem *drinks;
    int drink_count;

    MenuItem *snacks;
    int snack_count;
} Menu;


Menu menu = {NULL, 0, NULL, 0};

MenuItem **orders = NULL;
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
    FILE *file = fopen("inputs/drink2.txt", "r");
    int capacity = 10;

    menu.drinks = (MenuItem *)malloc(capacity * sizeof(MenuItem));
    char buffer[BUFFER_LENGTH];

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (menu.drink_count >= capacity)
        {
            capacity *= 2;
            menu.drinks = (MenuItem *)realloc(menu.drinks, capacity * sizeof(MenuItem));
        }

        // 개행 제거
        buffer[strcspn(buffer, "\n")] = 0;

        // strok 함수를 사용하여 메뉴 이름과 가격, 재고를 분리
        char* token = strtok(buffer, "\t");
        menu.drinks[menu.drink_count].name = strdup(token);

        token = strtok(NULL, "\t");
        menu.drinks[menu.drink_count].price = atoi(token);

        token = strtok(NULL, "\t");
        menu.drinks[menu.drink_count].stock = atoi(token);

        menu.drink_count++;
    }

    fclose(file);
}

// 스낵 메뉴 리셋 함수
void reset_snack_menu()
{
    FILE *file = fopen("inputs/snack2.txt", "r");
    int capacity = 10;

    menu.snacks = (MenuItem *)malloc(capacity * sizeof(MenuItem));
    char buffer[BUFFER_LENGTH];

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (menu.snack_count >= capacity)
        {
            capacity *= 2;
            menu.snacks = (MenuItem *)realloc(menu.snacks, capacity * sizeof(MenuItem));
        }

        // 개행 제거
        buffer[strcspn(buffer, "\n")] = 0;

        // strok 함수를 사용하여 메뉴 이름과 가격, 재고를 분리
        char* token = strtok(buffer, "\t");
        menu.snacks[menu.snack_count].name = strdup(token);

        token = strtok(NULL, "\t");
        menu.snacks[menu.snack_count].price = atoi(token);

        token = strtok(NULL, "\t");
        menu.snacks[menu.snack_count].stock = atoi(token);

        menu.snack_count++;
    }

    fclose(file);
}

void print_receipt()
{
    // Save to file
    FILE *file = fopen("receipt.txt", "w");

    int total_price = 0;

    printf("======== Receipt ========\n");
    for (int i = 0; i < order_count; i++)
    {
        printf("%s\t%d\n", orders[i]->name, orders[i]->price);
        fprintf(file, "%s\t%d\n", orders[i]->name, orders[i]->price);

        total_price += orders[i]->price;
    }

    printf("Total : %d\n", total_price);
    fprintf(file, "Total : %d", total_price);
    
    fclose(file);
}

void service_mode()
{
    int input;
    int menu_index;

    int capacity = 5;
    orders = (MenuItem **)malloc(capacity * sizeof(MenuItem *));
    order_count = 0;

    // Print menu
    printf("======== drink menu ========\n");
    for (int i = 0; i < menu.drink_count; i++)
    {
        printf("%d: %s\t%d\t%d\n", i + 1, menu.drinks[i].name, menu.drinks[i].price, menu.drinks[i].stock);
    }

    printf("======== snack menu ========\n");
    for (int i = 0; i < menu.snack_count; i++)
    {
        printf("%d: %s\t%d\t%d\n", i + 1, menu.snacks[i].name, menu.snacks[i].price, menu.snacks[i].stock);
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

                if (menu_index < 1 || menu_index > menu.drink_count)
                {
                    printf("Error : Not enough ID for drink.\n");
                    continue;
                }

                if (menu.drinks[menu_index - 1].stock <= 0)
                {
                    printf("Error : Out of stock for drink.\n");
                    continue;
                }

                break;
            }

            if (order_count >= capacity)
            {
                capacity *= 2;
                orders = (MenuItem **)realloc(orders, capacity * sizeof(MenuItem *));
            }

            menu.drinks[menu_index - 1].stock--;

            orders[order_count++] = &menu.drinks[menu_index - 1];
        }
        else if (input == 2)
        {
            while (1)
            {
                printf("Type snack id to buy : ");
                scanf("%d", &menu_index);

                if (menu_index < 1 || menu_index > menu.snack_count)
                {
                    printf("Error : Not enough ID for snack.\n");
                    continue;
                }

                if (menu.snacks[menu_index - 1].stock <= 0)
                {
                    printf("Error : Out of stock for snack.\n");
                    continue;
                }

                break;
            }

            if (order_count >= capacity)
            {
                capacity *= 2;
                orders = (MenuItem **)realloc(orders, capacity * sizeof(MenuItem *));
            }

            if (menu.snacks[menu_index - 1].stock <= 0)
            {
                printf("Error : Out of stock for snack.\n");
                continue;
            }
            menu.snacks[menu_index - 1].stock--;

            orders[order_count++] = &menu.snacks[menu_index - 1];
        }
        else
        {
            printf("Error : Wrong input for type select!\n");
            continue;
        }
    }
    
    // 영수증 발행
    print_receipt();

    // Free orders
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

    // Free menu
    free(menu.drinks);
    free(menu.snacks);

    return 0;
}