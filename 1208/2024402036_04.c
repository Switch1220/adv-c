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

// 외부회사 정보 구조체 추가
typedef struct {
    char* name;
    MenuItem* items;
    int item_count;
} Vendor;

Vendor* vendors = NULL;
int vendor_count = 0;
int total_asset = 0;

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

        menu.drinks[menu.drink_count].id = menu.drink_count + 1;

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

        menu.snacks[menu.snack_count].id = menu.snack_count + 1;

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

// 메뉴 상태(재고)를 파일에 저장하는 함수
void save_menu_status() {
    FILE *drink_file = fopen("inputs/drink2.txt", "w");
    FILE *snack_file = fopen("inputs/snack2.txt", "w");

    if (!drink_file || !snack_file) {
        printf("Error: Unable to save menu status!\n");
        return;
    }

    // 음료 메뉴 상태 저장
    for (int i = 0; i < menu.drink_count; i++) {
        fprintf(drink_file, "%s\t%d\t%d\n", 
            menu.drinks[i].name, 
            menu.drinks[i].price, 
            menu.drinks[i].stock);
    }

    // 스낵 메뉴 상태 저장
    for (int i = 0; i < menu.snack_count; i++) {
        fprintf(snack_file, "%s\t%d\t%d\n", 
            menu.snacks[i].name, 
            menu.snacks[i].price, 
            menu.snacks[i].stock);
    }

    fclose(drink_file);
    fclose(snack_file);
}

// 매출 정보를 저장하는 함수 추가
void update_asset(int order_total_price) {
    FILE *sales_file = fopen("inputs/asset.txt", "r+");
    int current_total_sales = 0;

    // 기존 총 매출액 읽기
    fscanf(sales_file, "%d", &current_total_sales);
    rewind(sales_file);

    // 새로운 총 매출액 계산 및 저장
    current_total_sales += order_total_price;
    fprintf(sales_file, "%d", current_total_sales);
    fclose(sales_file);
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

// 외부회사 정보 로드 함수
void load_vendors() {
    char* vendor_files[] = {"inputs/coupang.txt", "inputs/11st.txt", "inputs/gmarket.txt"};
    vendor_count = sizeof(vendor_files) / sizeof(vendor_files[0]);
    vendors = (Vendor*)malloc(vendor_count * sizeof(Vendor));

    for (int i = 0; i < vendor_count; i++) {
        FILE* file = fopen(vendor_files[i], "r");
        if (!file) {
            printf("Error: Cannot open vendor file %s\n", vendor_files[i]);
            continue;
        }

        vendors[i].name = strdup(vendor_files[i]);
        vendors[i].items = (MenuItem*)malloc((menu.drink_count + menu.snack_count) * sizeof(MenuItem));
        vendors[i].item_count = 0;

        char buffer[BUFFER_LENGTH];
        while (fgets(buffer, sizeof(buffer), file)) {
            buffer[strcspn(buffer, "\n")] = 0;

            if (vendors[i].item_count >= menu.drink_count + menu.snack_count) {
                vendors[i].items = (MenuItem*)realloc(vendors[i].items, (vendors[i].item_count + 1) * sizeof(MenuItem));
            }

            char* token = strtok(buffer, "\t");
            vendors[i].items[vendors[i].item_count].name = strdup(token);

            token = strtok(NULL, "\t");
            vendors[i].items[vendors[i].item_count].price = atoi(token);

            token = strtok(NULL, "\t");
            vendors[i].items[vendors[i].item_count].stock = atoi(token);

            vendors[i].item_count++;
        }

        fclose(file);
    }
}

// 재고 부족 확인 함수
int count_out_of_stock_items() {
    int out_of_stock_count = 0;

    for (int i = 0; i < menu.drink_count; i++) {
        if (menu.drinks[i].stock == 0) out_of_stock_count++;
    }

    for (int i = 0; i < menu.snack_count; i++) {
        if (menu.snacks[i].stock == 0) out_of_stock_count++;
    }

    return out_of_stock_count;
}

// 최저가 공급업체 선택 함수
Vendor* select_cheapest_vendor() {
    Vendor* selected_vendor = NULL;
    int lowest_total_cost = -1;  // 초기값을 -1로 설정

    for (int v = 0; v < vendor_count; v++) {
        int total_cost = 0;
        int can_supply_all = 1;

        // 모든 품절된 메뉴에 대해 공급 가능성 확인
        for (int i = 0; i < menu.drink_count; i++) {
            if (menu.drinks[i].stock == 0) {
                int found = 0;
                for (int j = 0; j < vendors[v].item_count; j++) {
                    if (strcmp(menu.drinks[i].name, vendors[v].items[j].name) == 0) {
                        total_cost += vendors[v].items[j].price * 5;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    can_supply_all = 0;
                    break;
                }
            }
        }

        for (int i = 0; i < menu.snack_count; i++) {
            if (menu.snacks[i].stock == 0) {
                int found = 0;
                for (int j = 0; j < vendors[v].item_count; j++) {
                    if (strcmp(menu.snacks[i].name, vendors[v].items[j].name) == 0) {
                        total_cost += vendors[v].items[j].price * 5;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    can_supply_all = 0;
                    break;
                }
            }
        }

        // 모든 품목을 공급할 수 있고, 
        // 첫 번째 공급 가능한 벤더이거나 기존 최저가보다 저렴한 경우
        if (can_supply_all && (lowest_total_cost == -1 || total_cost < lowest_total_cost)) {
            lowest_total_cost = total_cost;
            selected_vendor = &vendors[v];
        }
    }

    return selected_vendor;
}

void order_print(Vendor* vendor, int order_items, int order_total_cost) {
    printf("===== Order Results =====\n");
    printf("Vendor: %s\n", vendor->name);
    printf("Total Items Ordered: %d\n", order_items);
    printf("Total Order Cost: %d\n", order_total_cost);
    printf("Remaining Asset: %d\n", total_asset);

    // 주문한 상품 목록 출력
    printf("\nOrdered Items:\n");

    for (int i = 0; i < menu.drink_count; i++) {
        if (menu.drinks[i].stock == 5) {
            printf("%s (drink): 5 added\n", menu.drinks[i].name);
        }
    }
    for (int i = 0; i < menu.snack_count; i++) {
        if (menu.snacks[i].stock == 5) {
            printf("%s (snack): 5 added\n", menu.snacks[i].name);
        }
    }
}

// 재고 주문 함수
void order_stock() {
    // 재고 부족 아이템 개수 확인
    int out_of_stock_count = count_out_of_stock_items();
    if (out_of_stock_count < 3) return;

    // 최저가 공급업체 선택
    Vendor* selected_vendor = select_cheapest_vendor();
    if (!selected_vendor) {
        printf("No vendor can supply all out-of-stock items.\n");
        return;
    }

    // 주문 로직
    int order_items = 0;
    int order_total_cost = 0;

    // 음료수 재고 보충
    for (int i = 0; i < menu.drink_count; i++) {
        if (menu.drinks[i].stock == 0) {
            for (int j = 0; j < selected_vendor->item_count; j++) {
                if (strcmp(menu.drinks[i].name, selected_vendor->items[j].name) == 0) {
                    // 5개 단위로 주문
                    menu.drinks[i].stock += 5;
                    order_total_cost += selected_vendor->items[j].price * 5;
                    order_items++;
                    break;
                }
            }
        }
    }

    // 스낵 재고 보충
    for (int i = 0; i < menu.snack_count; i++) {
        if (menu.snacks[i].stock == 0) {
            for (int j = 0; j < selected_vendor->item_count; j++) {
                if (strcmp(menu.snacks[i].name, selected_vendor->items[j].name) == 0) {
                    // 5개 단위로 주문
                    menu.snacks[i].stock += 5;
                    order_total_cost += selected_vendor->items[j].price * 5;
                    order_items++;
                    break;
                }
            }
        }
    }

    // 보유금 차감
    int actual_order_cost = (order_total_cost <= total_asset) ? order_total_cost : total_asset;
    total_asset -= actual_order_cost;

    // 결과 출력
    order_print(selected_vendor, order_items, actual_order_cost);

    // asset.txt 파일 업데이트
    FILE* asset_file = fopen("inputs/asset.txt", "w");
    if (asset_file) {
        fprintf(asset_file, "%d", total_asset);
        fclose(asset_file);
    }
}

// 부족한 재고 출력 함수
void print_out_of_stock_items() {
    printf("======= Sold out Menu =======\n");
    for (int i = 0; i < menu.drink_count; i++) {
        if (menu.drinks[i].stock == 0) {
            printf("%d : %s\n", menu.drinks[i].id, menu.drinks[i].name);
        }
    }
    for (int i = 0; i < menu.snack_count; i++) {
        if (menu.snacks[i].stock == 0) {
            printf("%d : %s\n", menu.snacks[i].id, menu.snacks[i].name);
        }
    }
}

void admin_mode() {
    // 외부회사 정보 로드
    load_vendors();

    // 부족한 재고 출력
    print_out_of_stock_items();

    // 재고 주문 수행
    order_stock();

    save_menu_status();

    // 외부회사 정보 메모리 해제
    for (int i = 0; i < vendor_count; i++) {
        free(vendors[i].name);
        for (int j = 0; j < vendors[i].item_count; j++) {
            free(vendors[i].items[j].name);
        }
        free(vendors[i].items);
    }
    free(vendors);
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

    // 재고 상태 저장
    save_menu_status();

    int total_order_price = 0;
    for (int i = 0; i < order_count; i++)
    {
        total_order_price += orders[i]->price;
    }

    // 매출 정보 업데이트
    update_asset(total_order_price); 

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
        admin_mode();
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