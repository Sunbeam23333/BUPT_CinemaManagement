
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // 新增头文件用于字符检测
#include <time.h>
#include <math.h>
#include <limits.h>  // For INT_MAX and SIZE_MAX
// 常量定义
#define MAX_NAME_LENGTH 50
#define MAX_EMAIL_LENGTH 100
#define MAX_PHONE_LENGTH 12
#define MAX_PASSWORD_LENGTH 50
#define MAX_ID_LENGTH 10
#define MAX_USERS 100
#define MAX_ORDERS 1000
#define EMBEDDING_DIM 100            // 嵌入向量的维度
#define MAX_RECOMMENDATIONS 10     // 最大推荐数量
#define ROWS 17
#define COLS 17

// 数据结构定义和全局变量

typedef struct {
    char adminID[MAX_ID_LENGTH];
    char cinemaName[MAX_NAME_LENGTH];
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char passwordHash[MAX_PASSWORD_LENGTH];
} Admin;

typedef struct {
    char userID[MAX_ID_LENGTH];
    char name[MAX_NAME_LENGTH];
    char gender;
    char phone[MAX_PHONE_LENGTH];
    char passwordHash[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    float accountBalance;
    char preferredGenre[MAX_NAME_LENGTH];   // 用户偏好的电影题材
    char preferredTime[6];                 // 用户偏好的观看时间
    float maxPrice;                        // 用户可接受的最高票价
    char preferredScreenType[MAX_NAME_LENGTH]; // 用户偏好的影厅荧幕类型
} User;

typedef struct {
    char title[MAX_NAME_LENGTH];
    char genre[MAX_NAME_LENGTH];
    int duration; // 时长，分钟
    char director[MAX_NAME_LENGTH];
    // 其他电影相关属性...
} Movie;

typedef struct {
    Movie movie;            // 使用 Movie 结构体
    char showID[MAX_ID_LENGTH];
    char cinemaName[MAX_NAME_LENGTH];
    char hallID[MAX_ID_LENGTH];
    char movieName[MAX_NAME_LENGTH];
    char startTime[6];
    char endTime[6];
    int duration;
    int availableTickets;
    double price;
    char showType[10];
    char discountInfo[100];
    char genre[MAX_NAME_LENGTH];           // 电影题材
    float popularity;                      // 电影热度
    char screenType[MAX_NAME_LENGTH];      // 影厅荧幕类型
} Show;

typedef struct {
    int showIndex;       // 推荐的场次索引
    float similarity;    // 推荐的相似度分数
} Recommendation;

typedef struct {
    char orderID[MAX_ID_LENGTH];
    char userID[MAX_ID_LENGTH];
    char showID[MAX_ID_LENGTH];
    int seatNumbers[3];
    float totalPrice;
    int isPaid;
    time_t orderTime;  // 新增：订单创建时间
} Order;
typedef struct {
    int row;
    int col;
    char movie_id[20];
    char movie_cinema[20];
    char movie_hall[20];
    char purchaser_ID[20];
    char purchase_time[100];
} movie_seat;
typedef struct AVLNode {
    Show show;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

typedef struct HashTableEntry {
    char key[MAX_ID_LENGTH];
    void* value;
    struct HashTableEntry* next;
} HashTableEntry;

typedef struct HashTable {
    HashTableEntry** table;
    int size;
} HashTable;

// 通用的比较函数类型
typedef int (*CompareFunc)(const void*, const void*);
// 全局变量
HashTable* usersTable = NULL;  // Initialize to NULL
HashTable* adminsTable = NULL;  // Initialize to NULL
AVLNode* showsTree;
Order orders[MAX_ORDERS];
int orderCount = 0;
Show shows[MAX_ORDERS];  // 全局电影场次数组
int showCount = 0;       // 当前的电影场次数量

// 核心功能实现

unsigned int hashFunction(const char* key, int tableSize);
HashTable* initHashTable(int size);
void simpleHash(const char* input, char* outputHash);
void simpleEncrypt(const unsigned char* data, unsigned char* encryptedData, const unsigned char* key);
void addUser(User user);
void addAdmin(Admin admin);
void registerUser();
void login();
void loginUser();
void rechargeUser();
void userMenu(User* user);
void adminMenu(Admin* admin);
void recommendShows(User* user, Show* shows, int showCount, Recommendation* recommendations);
void recommendShowsMenu();
void modifyUserInfo(User* user);
void updatePreferences(User* user);
void queryAllUsers();
void modifyAdminInfo(Admin* admin);
void addNewShow();
void modifyShowInfo(Show* show);
void queryAllShows();
void logOperation(const char* operation);
void saveUserDataToFile();
void loadUserDataFromFile();
void saveShowDataToFile();
void loadShowDataFromFile();
void initializeDefaultShows();
void printAVLTree(AVLNode* node);
float calculatePopularity(Show* show);
void getUserEmbedding(const User* user, float* embedding);
void getShowEmbedding(const Show* show, float* embedding);
float calculateSimilarity(const float* embedding1, const float* embedding2);
void hybridRecommendShows(User* user, Show* shows, int showCount, Recommendation* recommendations);
User* findUserByID(const char* userID);
Show* findShowByID(const char* showID);

//购票查票
int createOrder(Order* order, const char* userID, const char* showID, int* seatNumbers, int seatCount, float totalPrice);
int updateSeatAvailability(Show* show, int* seatNumbers, int seatCount, int isAvailable);
int isSeatAvailable(Show* show, int seatNumber);
void viewAllOrders();
void viewOrdersSortedByTicketCount();
void viewOrdersSortedByOccupancyRate();
void viewOrdersSortedByRevenue();
// 补充的管理员查票功能
void queryShowsAndOrders(Admin* admin);
void ticketingZone(User* user);
void purchaseTicket(User* user);
void queryUserOrders(User* user);
void cancelOrder(User* user);

//座次
movie_seat* seat_ID = NULL;
int seats[ROWS][COLS] = { 0 };
int random_number_kind;
int random_number_range;
int random_number_high;
int num_of_selected_seats = 0;

User users[MAX_USERS];
int userCount = 0;
Show shows[MAX_ORDERS];
//int showCount = 0;
Order orders[MAX_ORDERS];
//int orderCount = 0;
void initialize_seat_settings();
void Seats(int seat_count);
int choose_seats();
void check_seats();
void recommend_seats(int rec_rows[], int rec_cols[], int* rec_count, int seat_count);
// 核心功能实现的代码保持不变
// 快速排序的划分函数
int partition(void* arr, int low, int high, int elementSize, CompareFunc compare) {
    char* pivot = (char*)arr + high * elementSize;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        char* element = (char*)arr + j * elementSize;
        if (compare(element, pivot) < 0) {
            i++;
            // 使用动态内存分配来代替VLA
            char* temp = (char*)malloc(elementSize);
            if (temp == NULL) {
                printf("Memory allocation failed\n");
                exit(1);
            }
            memcpy(temp, (char*)arr + i * elementSize, elementSize);
            memcpy((char*)arr + i * elementSize, (char*)arr + j * elementSize, elementSize);
            memcpy((char*)arr + j * elementSize, temp, elementSize);
            free(temp); // 释放内存
        }
    }

    char* temp = (char*)malloc(elementSize); // 动态分配
    if (temp == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    memcpy(temp, (char*)arr + (i + 1) * elementSize, elementSize);
    memcpy((char*)arr + (i + 1) * elementSize, (char*)arr + high * elementSize, elementSize);
    memcpy((char*)arr + high * elementSize, temp, elementSize);
    free(temp); // 释放内存
    return i + 1;
}


// 快速排序的主函数
void quickSort(void* arr, int low, int high, int elementSize, CompareFunc compare) {
    if (low < high) {
        int pi = partition(arr, low, high, elementSize, compare);
        quickSort(arr, low, pi - 1, elementSize, compare);
        quickSort(arr, pi + 1, high, elementSize, compare);
    }
}

unsigned int hashFunction(const char* key, int tableSize) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + *key++;
    }
    return hash % tableSize;
}

// 初始化哈希表
HashTable* initHashTable(int size) {
    HashTable* hashTable = (HashTable*)malloc(sizeof(HashTable));
    if (!hashTable) {  // 检查内存分配是否成功
        printf("Error: Unable to allocate memory for hash table.\n");
        exit(1);  // 退出程序
    }

    hashTable->size = size;
    hashTable->table = (HashTableEntry**)malloc(sizeof(HashTableEntry*) * size);
    if (!hashTable->table) {  // 检查内存分配是否成功
        printf("Error: Unable to allocate memory for hash table entries.\n");
        exit(1);  // 退出程序
    }

    for (int i = 0; i < size; i++) {
        hashTable->table[i] = NULL;
    }
    return hashTable;
}


// 简单哈希函数
void simpleHash(const char* input, char* outputHash) {
    unsigned long hash = 0;
    while (*input) {
        hash += (unsigned char)(*input);
        input++;
    }
    sprintf_s(outputHash, MAX_PASSWORD_LENGTH, "%08lx", hash % 0xFFFFFFFF);
}

// 简单加密函数（XOR）
void simpleEncrypt(const unsigned char* data, unsigned char* encryptedData, const unsigned char* key) {
    for (size_t i = 0; i < strlen((const char*)data); i++) {
        encryptedData[i] = data[i] ^ key[i % strlen((const char*)key)];
    }
    encryptedData[strlen((const char*)data)] = '\0';
}

// 添加用户
void addUser(User user) {
    if (usersTable == NULL) {
        printf("Error: usersTable is not initialized.\n");
        return;
    }

    unsigned int hashIndex = hashFunction(user.userID, usersTable->size);
    HashTableEntry* newEntry = (HashTableEntry*)malloc(sizeof(HashTableEntry));
    if (!newEntry) {
        printf("Error: Unable to allocate memory for hash table entry.\n");
        exit(1);
    }

    strcpy_s(newEntry->key, sizeof(newEntry->key), user.userID);
    newEntry->value = malloc(sizeof(User));
    if (!newEntry->value) {
        printf("Error: Unable to allocate memory for user data.\n");
        free(newEntry);
        exit(1);
    }
    memcpy(newEntry->value, &user, sizeof(User));
    newEntry->next = usersTable->table[hashIndex];
    usersTable->table[hashIndex] = newEntry;
}



// 添加管理员
void addAdmin(Admin admin) {
    unsigned int hashIndex = hashFunction(admin.adminID, adminsTable->size);
    HashTableEntry* newEntry = (HashTableEntry*)malloc(sizeof(HashTableEntry));

    // 检查malloc是否成功
    if (newEntry == NULL) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    strcpy_s(newEntry->key, sizeof(newEntry->key), admin.adminID);
    newEntry->value = malloc(sizeof(Admin));

    // 检查malloc是否成功
    if (newEntry->value == NULL) {
        printf("Error: Memory allocation failed for admin value.\n");
        free(newEntry);  // 清理先前分配的内存
        return;
    }

    memcpy(newEntry->value, &admin, sizeof(Admin));
    newEntry->next = adminsTable->table[hashIndex];
    adminsTable->table[hashIndex] = newEntry;
}


// 注册用户
void registerUser() {
    User user;
    memset(&user, 0, sizeof(User));  // 初始化 user 所有成员为 0
    /*char buffer[100];*/

    // 输入 User ID
    while (1) {
        printf("Enter User ID (only letters and digits, max 9 chars): ");
        scanf_s("%9s", user.userID, (unsigned)_countof(user.userID));

        // 检查 User ID 是否只包含字母和数字
        int isValid = 1;
        for (int i = 0; i < strlen(user.userID); i++) {
            if (!isalnum((unsigned char)user.userID[i])) {
                printf("Error: User ID must contain only letters and digits.\n");
                isValid = 0;
                break;
            }
        }
        if (isValid) break;  // 如果输入合法，退出循环
    }

    // 输入 Name
    printf("Enter Name: ");
    scanf_s("%49s", user.name, (unsigned)_countof(user.name));

    // 输入 Gender
    while (1) {
        printf("Enter Gender (M/F): ");
        scanf_s(" %c", &user.gender, 1);

        // 检查 Gender 是否为 'M' 或 'F'
        if (user.gender == 'M' || user.gender == 'F') {
            break;
        }
        else {
            printf("Error: Gender must be 'M' or 'F'.\n");
        }
    }

    // 输入 Phone
    while (1) {
        printf("Enter Phone: ");
        scanf_s("%11s", user.phone, (unsigned)_countof(user.phone));

        // 检查电话号码是否只包含数字
        int isValid = 1;
        for (int i = 0; i < strlen(user.phone); i++) {
            if (!isdigit((unsigned char)user.phone[i])) {
                printf("Error: Phone number must contain only digits.\n");
                isValid = 0;
                break;
            }
        }
        if (isValid) break;  // 如果输入合法，退出循环
    }

    // 输入 Email
    printf("Enter Email: ");
    scanf_s("%99s", user.email, (unsigned)_countof(user.email));

    // 输入 Password
    while (1) {
        printf("Enter Password: ");
        char password[MAX_PASSWORD_LENGTH + 1];
        scanf_s("%49s", password, (unsigned)_countof(password));
        password[MAX_PASSWORD_LENGTH] = '\0';

        int isValid = 1;
        for (int i = 0; i < strlen(password); i++) {
            if (!isalnum((unsigned char)password[i])) {
                printf("Error: Password must contain only letters and digits.\n");
                isValid = 0;
                break;
            }
        }
        if (isValid) {
            simpleHash(password, user.passwordHash);
            break;  // 如果输入合法，退出循环
        }
    }

    user.accountBalance = 0.0f;

    // 让用户选择他们的偏好
    printf("Enter Preferred Genre (e.g., Action, Comedy, Drama): ");
    scanf_s("%49s", user.preferredGenre, (unsigned)_countof(user.preferredGenre));

    printf("Enter Preferred Viewing Time (e.g., 18:00): ");
    scanf_s("%5s", user.preferredTime, (unsigned)_countof(user.preferredTime));

    printf("Enter Maximum Ticket Price: ");
    scanf_s("%f", &user.maxPrice);

    printf("Enter Preferred Screen Type (e.g., IMAX, 3D): ");
    scanf_s("%49s", user.preferredScreenType, (unsigned)_countof(user.preferredScreenType));

    addUser(user);
    printf("User registered successfully!\n");
}


// 用户登录后的菜单
void userMenu(User* user) {
    int choice;
    do {
        printf("\n=== User Menu ===\n");
        printf("1. View Personal Information\n");
        printf("2. Modify Personal Information\n");
        printf("3. Get Movie Recommendations\n");
        printf("4. Update Preferences\n");
        printf("5. Ticketing Zone\n");  // 新增购票专区选项
        printf("6. Logout\n");
        printf("Enter choice: ");
        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            printf("User Information:\n");
            printf("Name: %s\nPhone: %s\nEmail: %s\nPreferred Genre: %s\nPreferred Viewing Time: %s\nMax Price: %.2f\nPreferred Screen Type: %s\n",
                user->name, user->phone, user->email, user->preferredGenre, user->preferredTime, user->maxPrice, user->preferredScreenType);
            break;
        case 2:
            modifyUserInfo(user);
            break;
        case 3:
            recommendShowsMenu(user);
            break;
        case 4:
            updatePreferences(user);
            break;
        case 5:
            ticketingZone(user);  // 新增购票专区功能
            break;
        case 6:
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 6);
}

// 管理员登录后的菜单
void adminMenu(Admin* admin) {
    int choice;
    do {
        printf("\n=== Admin Menu ===\n");
        printf("1. View All Users\n");
        printf("2. Modify User Information\n");
        printf("3. Add New Show\n");
        printf("4. Modify Show Information\n");
        printf("5. Query Shows and Orders\n");  // 新增场次和订单查询模块
        printf("6. Logout\n");
        printf("Enter choice: ");
        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            queryAllUsers();
            break;
        case 2: {
            char userID[MAX_ID_LENGTH];
            printf("Enter User ID to modify info: ");
            scanf_s("%9s", userID, (unsigned)_countof(userID));
            User* user = findUserByID(userID);
            if (user) {
                modifyUserInfo(user);
            }
            else {
                printf("User ID not found.\n");
            }
            break;
        }
        case 3:
            addNewShow();
            break;
        case 4: {
            char showID[MAX_ID_LENGTH];
            printf("Enter Show ID to modify info: ");
            scanf_s("%9s", showID, (unsigned)_countof(showID));
            Show* show = findShowByID(showID);  // 使用 findShowByID 函数查找电影场次
            if (show) {
                modifyShowInfo(show);
            }
            else {
                printf("Show ID not found.\n");
            }
            break;
        }
        case 5:
            queryShowsAndOrders(admin);  // 新增场次和订单查询功能
            break;
        case 6:
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 6);
}
//购票专区
void ticketingZone(User* user) {
    int choice;
    do {
        printf("\n=== Ticketing Zone ===\n");
        printf("1. Purchase Ticket\n");
        printf("2. Query Orders\n");
        printf("3. Cancel Order\n");
        printf("4. Back to User Menu\n");
        printf("Enter choice: ");
        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            purchaseTicket(user);
            break;
        case 2:
            queryUserOrders(user);
            break;
        case 3:
            cancelOrder(user);
            break;
        case 4:
            return;  // 返回用户菜单
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 4);
}


// 登录用户
void loginUser() {
    char userID[MAX_ID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter User ID: ");
    scanf_s("%9s", userID, (unsigned)_countof(userID));

    printf("Enter Password: ");
    scanf_s("%19s", password, (unsigned)_countof(password));

    char hashedPassword[MAX_PASSWORD_LENGTH];
    simpleHash(password, hashedPassword);

    unsigned int hashIndex = hashFunction(userID, usersTable->size);
    HashTableEntry* entry = usersTable->table[hashIndex];
    while (entry) {
        User* user = (User*)entry->value;
        if (strcmp(user->userID, userID) == 0) {
            if (strcmp(user->passwordHash, hashedPassword) == 0) {
                printf("Login successful! Welcome, %s.\n", user->name);
                userMenu(user);  // 调用用户菜单
                return;
            }
            else {
                printf("Error: Incorrect password.\n");
                return;
            }
        }
        entry = entry->next;
    }
    printf("Error: User ID not found.\n");
}


// 用户充值
void rechargeUser() {
    char userID[MAX_ID_LENGTH];
    float amount;

    printf("Enter User ID: ");
    scanf_s("%9s", userID, (unsigned)_countof(userID));  // 使用 scanf_s 并指定缓冲区大小

    printf("Enter amount to recharge: ");
    scanf_s("%f", &amount);  // 读取浮点数时，不需要指定大小

    unsigned int hashIndex = hashFunction(userID, usersTable->size);
    HashTableEntry* entry = usersTable->table[hashIndex];
    while (entry) {
        User* user = (User*)entry->value;
        if (strcmp(user->userID, userID) == 0) {
            user->accountBalance += amount;
            printf("Recharge successful! New balance: %.2f\n", user->accountBalance);
            return;
        }
        entry = entry->next;
    }
    printf("Error: User ID not found.\n");
}

void purchaseTicket(User* user) {
    char showID[MAX_ID_LENGTH];
    int selectedSeats[3] = { -1, -1, -1 };
    int seatCount;

    printf("Enter Show ID to purchase ticket: ");
    scanf_s("%9s", showID, (unsigned)_countof(showID));

    Show* show = findShowByID(showID);
    if (!show) {
        printf("Error: Show ID not found.\n");
        return;
    }

    printf("Enter the number of seats you want to purchase (1-3): ");
    scanf_s("%d", &seatCount);
    if (seatCount < 1 || seatCount > 3) {
        printf("Error: Invalid number of seats. You can only purchase between 1 and 3 seats.\n");
        return;
    }

    // 使用float来计算总成本
    float totalCost = seatCount * show->price; // 将totalCost类型修改为float
    if (user->accountBalance < totalCost) {
        printf("Error: Insufficient balance. Please recharge your account.\n");
        return;
    }

    for (int i = 0; i < seatCount; i++) {
        int seatNumber;
        printf("Enter seat number #%d: ", i + 1);
        scanf_s("%d", &seatNumber);
        if (!isSeatAvailable(show, seatNumber)) {
            printf("Error: Seat %d is not available.\n", seatNumber);
            return;
        }
        selectedSeats[i] = seatNumber;
    }

    // 扣除用户余额
    user->accountBalance -= totalCost;

    // 记录订单并标记座位已被占用
    printf("Ticket purchased successfully! Total cost: %.2f\n", totalCost);
}





// 创建订单
int createOrder(Order* order, const char* userID, const char* showID, int* seatNumbers, int seatCount, float totalPrice) {
    if (orderCount >= MAX_ORDERS) {
        printf("Error: Maximum number of orders reached.\n");
        return 0; // 失败
    }

    // 设置订单的相关信息
    strcpy_s(order->orderID, sizeof(order->orderID), "Order001"); // 示例订单ID生成
    strcpy_s(order->userID, sizeof(order->userID), userID);
    strcpy_s(order->showID, sizeof(order->showID), showID);
    memcpy(order->seatNumbers, seatNumbers, sizeof(int) * seatCount);
    order->totalPrice = totalPrice;
    order->isPaid = 0; // 默认为未支付
    order->orderTime = time(NULL); // 记录订单创建时间

    orders[orderCount++] = *order;
    return 1; // 成功
}


void queryUserOrders(User* user) {
    printf("\n=== Order History for User %s ===\n", user->userID);
    int found = 0;
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].userID, user->userID) == 0) {
            found = 1;
            printf("OrderID: %s, ShowID: %s, Seats: [%d, %d, %d], Total Price: %.2f, Status: %s\n",
                orders[i].orderID, orders[i].showID, orders[i].seatNumbers[0],
                orders[i].seatNumbers[1], orders[i].seatNumbers[2],
                orders[i].totalPrice, orders[i].isPaid ? "Paid" : "Unpaid");
        }
    }
    if (!found) {
        printf("No orders found.\n");
    }
}
void cancelOrder(User* user) {
    char orderID[MAX_ID_LENGTH + 1]; // 确保有额外的空间用于零终止符

    printf("Enter Order ID to cancel: ");
    scanf_s("%9s", orderID, (unsigned)_countof(orderID)); // 读取最多9个字符，保留1个字符给零终止符

    // 手动确保零终止符
    orderID[MAX_ID_LENGTH] = '\0';

    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].orderID, orderID) == 0 && strcmp(orders[i].userID, user->userID) == 0) {
            if (orders[i].isPaid) {
                printf("Error: Cannot cancel a paid order.\n");
                return;
            }
            // 取消订单并更新座位状态
            Show* show = findShowByID(orders[i].showID);
            if (show) {
                updateSeatAvailability(show, orders[i].seatNumbers, 3, 1); // 1 表示座位可用
            }
            printf("Order canceled successfully.\n");
            return;
        }
    }
    printf("Error: Order ID not found or does not belong to this user.\n");
}
int updateSeatAvailability(Show* show, int* seatNumbers, int seatCount, int isAvailable) {
    // 实现座位更新逻辑，更新`show`中`availableTickets`或其他相关信息
    if (isAvailable) {
        show->availableTickets += seatCount; // 示例：座位可用时增加票数
    }
    else {
        show->availableTickets -= seatCount; // 示例：座位售出时减少票数
    }
    return 1; // 成功
}

// 查看所有订单
void viewAllOrders(const char* cinemaName) {
    printf("=== All Orders for Cinema %s ===\n", cinemaName);
    for (int i = 0; i < orderCount; i++) {
        printf("OrderID: %s, UserID: %s, ShowID: %s, Total Price: %.2f\n",
            orders[i].orderID, orders[i].userID, orders[i].showID, orders[i].totalPrice);
    }
}
// 推荐算法管理
void recommendShows(User* user, Show* shows, int showCount, Recommendation* recommendations) {
    int recommendationCount = 0;
    float userEmbedding[EMBEDDING_DIM] = { 0 }; // 初始化用户嵌入向量

    getUserEmbedding(user, userEmbedding);

    for (int i = 0; i < showCount; i++) {
        float showEmbedding[EMBEDDING_DIM] = { 0 };
        getShowEmbedding(&shows[i], showEmbedding);

        float similarity = calculateSimilarity(userEmbedding, showEmbedding);
        printf("Calculated similarity for show %s: %f\n", shows[i].movieName, similarity); // 调试输出

        if (similarity > 0.0) {
            recommendations[recommendationCount].showIndex = i;
            recommendations[recommendationCount].similarity = similarity;
            recommendationCount++;
        }

        if (recommendationCount >= MAX_RECOMMENDATIONS) break;
    }

    // 按相似度从高到低排序推荐
    for (int i = 0; i < recommendationCount - 1; i++) {
        for (int j = i + 1; j < recommendationCount; j++) {
            if (recommendations[i].similarity < recommendations[j].similarity) {
                Recommendation temp = recommendations[i];
                recommendations[i] = recommendations[j];
                recommendations[j] = temp;
            }
        }
    }

    printf("Total recommendations: %d\n", recommendationCount);
}


// 命令行打广
void recommendShowsMenu() {
    printf("Enter User ID for Recommendations: ");
    char userID[MAX_ID_LENGTH];
    scanf_s("%9s", userID, (unsigned)_countof(userID));

    unsigned int hashIndex = hashFunction(userID, usersTable->size);
    HashTableEntry* entry = usersTable->table[hashIndex];
    while (entry) {
        User* user = (User*)entry->value;
        if (strcmp(user->userID, userID) == 0) {
            Recommendation recommendations[MAX_RECOMMENDATIONS] = { 0 };
            recommendShows(user, shows, showCount, recommendations);

            printf("\n=== Recommended Shows for User %s ===\n", userID);
            printf("%-10s %-10s %-20s %-10s %-10s %-10s %-10s\n", "Show ID", "Cinema", "Movie", "Price", "Start", "End", "Screen Type");

            for (int i = 0; i < MAX_RECOMMENDATIONS; i++) {
                int index = recommendations[i].showIndex;
                if (index >= 0 && index < showCount) { // 确保索引有效
                    printf("%-10s %-10s %-20s %-10.2f %-10s %-10s %-10s\n",
                        shows[index].showID, shows[index].cinemaName, shows[index].movie.title,
                        shows[index].price, shows[index].startTime, shows[index].endTime, shows[index].screenType);
                }
            }
            printf("=====================================\n");
            return;
        }
        entry = entry->next;
    }
    printf("Error: User ID not found.\n");
}



// 管理员登录
void adminLogin() {
    char adminID[MAX_ID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter Admin ID: ");
    scanf_s("%9s", adminID, (unsigned)_countof(adminID));  // 使用 scanf_s 并指定缓冲区大小

    printf("Enter Password: ");
    scanf_s("%19s", password, (unsigned)_countof(password));  // 使用 scanf_s 并指定缓冲区大小

    char hashedPassword[MAX_PASSWORD_LENGTH];
    simpleHash(password, hashedPassword);
    unsigned int hashIndex = hashFunction(adminID, adminsTable->size);
    HashTableEntry* entry = adminsTable->table[hashIndex];
    while (entry) {
        Admin* admin = (Admin*)entry->value;
        if (strcmp(admin->adminID, adminID) == 0) {
            if (strcmp(admin->passwordHash, hashedPassword) == 0) {
                printf("Admin login successful! Welcome, %s.\n", admin->name);
                return;
            }
            else {
                printf("Error: Incorrect password.\n");
                return;
            }
        }
        entry = entry->next;
    }
    printf("Error: Admin ID not found.\n");
}

// 打印AVL树中的所有场次信息
void printAVLTree(AVLNode* node) {
    if (node != NULL) {
        printAVLTree(node->left);
        printf("ShowID: %s, Movie: %s, Time: %s - %s, Available Tickets: %d\n",
            node->show.showID, node->show.movieName, node->show.startTime,
            node->show.endTime, node->show.availableTickets);
        printAVLTree(node->right);
    }
}

// 树操作代码省略...
// 计算电影的热度得分，考虑票房、评分、观看人数等因素
float calculatePopularity(Show* show) {
    // 示例公式：热度 = (票房 + 评分 * 观众人数) / 2
    // 注意：这是一个简单的示例公式，实际的热度计算可以更加复杂
    float boxOfficeFactor = (float)(show->price * (float)(show->availableTickets - 100));// 假设卖了100张票
    float ratingFactor = show->popularity;  // 电影的评分
    float audienceFactor = (float)(100 + show->availableTickets); // 假设有100个观众基础
    return (boxOfficeFactor + ratingFactor * audienceFactor) / 2.0f;
}

// 获取用户的嵌入向量
void getUserEmbedding(const User* user, float* embedding) {
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        embedding[i] = 0.0f;
    }

    if (user->gender == 'M') {
        embedding[0] = 1.0f;
    }
    else if (user->gender == 'F') {
        embedding[1] = 1.0f;
    }
    // 更新其他特征...
}

// 获取电影场次的嵌入向量
void getShowEmbedding(const Show* show, float* embedding) {
    // 初始化嵌入向量为 0
    for (int i = 0; i < EMBEDDING_DIM; i++) {
        embedding[i] = 0.0f;
    }

    // 根据荧幕类型更新嵌入向量
    if (strcmp(show->screenType, "IMAX") == 0) {
        embedding[0] = 1.0f;
    }
    else if (strcmp(show->screenType, "3D") == 0) {
        embedding[1] = 1.0f;
    }
    else if (strcmp(show->screenType, "Standard") == 0) {
        embedding[2] = 1.0f;
    }

    // 根据电影题材更新嵌入向量
    if (strcmp(show->genre, "Science Fiction") == 0) {
        embedding[3] = 1.0f;
    }
    else if (strcmp(show->genre, "Crime") == 0) {
        embedding[4] = 1.0f;
    }
    else if (strcmp(show->genre, "Adventure") == 0) {
        embedding[5] = 1.0f;
    }

    // 根据价格区间更新嵌入向量
    if (show->price <= 10.0f) {
        embedding[6] = 1.0f;
    }
    else if (show->price > 10.0f && show->price <= 15.0f) {
        embedding[7] = 1.0f;
    }
    else {
        embedding[8] = 1.0f;
    }

    // 根据电影时长更新嵌入向量
    if (show->duration < 90) {
        embedding[9] = 1.0f;
    }
    else if (show->duration >= 90 && show->duration <= 120) {
        embedding[9] = 2.0f;  // 用不同的值区分不同时长
    }
    else {
        embedding[9] = 3.0f;
    }

    // 根据电影热度更新嵌入向量
    embedding[10] = show->popularity / 10.0f;  // 假设热度为 0-10，归一化到 0-1 的范围

    // 根据电影的放映时间更新嵌入向量
    int hour;
    char startTimeCopy[6];  // 假设时间格式为 HH:MM
    char* nextToken = NULL;
    strcpy_s(startTimeCopy, sizeof(startTimeCopy), show->startTime);  // 复制时间字符串
    hour = atoi(strtok_s(startTimeCopy, ":", &nextToken));  // 使用复制的可修改字符串

    if (hour < 12) {
        embedding[11] = 1.0f;  // 早晨
    }
    else if (hour >= 12 && hour < 18) {
        embedding[12] = 1.0f;  // 下午
    }
    else {
        embedding[13] = 1.0f;  // 晚上
    }
}

User* findUserByID(const char* userID) {
    unsigned int hashIndex = hashFunction(userID, usersTable->size);
    HashTableEntry* entry = usersTable->table[hashIndex];
    while (entry != NULL) {
        User* user = (User*)entry->value;
        if (strcmp(user->userID, userID) == 0) {
            return user;  // 找到用户并返回指针
        }
        entry = entry->next;
    }
    return NULL;  // 未找到用户
}
Show* findShowByID(const char* showID) {
    for (int i = 0; i < showCount; i++) {
        if (strcmp(shows[i].showID, showID) == 0) {
            return &shows[i];  // 找到匹配的场次并返回指针
        }
    }
    return NULL;  // 未找到匹配的场次
}
// 计算两个嵌入向量之间的相似度（余弦相似度）
float calculateSimilarity(const float* embedding1, const float* embedding2) {
    float dotProduct = 0.0f;
    float magnitude1 = 0.0f;
    float magnitude2 = 0.0f;

    for (int i = 0; i < EMBEDDING_DIM; i++) {
        dotProduct += embedding1[i] * embedding2[i];
        magnitude1 += embedding1[i] * embedding1[i];
        magnitude2 += embedding2[i] * embedding2[i];
    }

    if (magnitude1 == 0 || magnitude2 == 0) {
        return 0.0f;
    }

    // 使用1 - 余弦相似度来计算距离越近越好
    return 1- dotProduct / (sqrtf(magnitude1) * sqrtf(magnitude2));
}



// 混合推荐策略
void hybridRecommendShows(User* user, Show* shows, int showCount, Recommendation* recommendations) {
    int recommendationCount = 0;
    float userEmbedding[EMBEDDING_DIM] = { 0 };

    getUserEmbedding(user, userEmbedding);

    for (int i = 0; i < showCount; i++) {
        float showEmbedding[EMBEDDING_DIM] = { 0 };
        getShowEmbedding(&shows[i], showEmbedding);

        // 内容推荐：基于用户偏好与电影属性的相似度
        float contentBasedSimilarity = calculateSimilarity(userEmbedding, showEmbedding);

        // 协同过滤：基于用户历史和电影热度
        float collaborativeFilteringScore = calculatePopularity(&shows[i]); // 使用热度计算

        // 加权计算混合推荐得分
        float hybridScore = 0.7f * contentBasedSimilarity + 0.3f * collaborativeFilteringScore;

        recommendations[recommendationCount].showIndex = i;
        recommendations[recommendationCount].similarity = hybridScore;
        recommendationCount++;

        if (recommendationCount >= MAX_RECOMMENDATIONS) break;
    }

    // 按相似度排序推荐
    for (int i = 0; i < recommendationCount - 1; i++) {
        for (int j = i + 1; j < recommendationCount; j++) {
            if (recommendations[i].similarity < recommendations[j].similarity) {
                Recommendation temp = recommendations[i];
                recommendations[i] = recommendations[j];
                recommendations[j] = temp;
            }
        }
    }
}

// 修改用户个人信息
void modifyUserInfo(User* user) {
    printf("Modify User Info for User ID: %s\n", user->userID);
    printf("Enter new name (current: %s): ", user->name);
    scanf_s("%49s", user->name, (unsigned)_countof(user->name));

    printf("Enter new phone (current: %s): ", user->phone);
    scanf_s("%11s", user->phone, (unsigned)_countof(user->phone));

    printf("Enter new email (current: %s): ", user->email);
    scanf_s("%99s", user->email, (unsigned)_countof(user->email));

    printf("Enter new preferred genre (current: %s): ", user->preferredGenre);
    scanf_s("%49s", user->preferredGenre, (unsigned)_countof(user->preferredGenre));

    printf("Enter new preferred viewing time (current: %s): ", user->preferredTime);
    scanf_s("%5s", user->preferredTime, (unsigned)_countof(user->preferredTime));

    printf("Enter new max ticket price (current: %.2f): ", user->maxPrice);
    scanf_s("%f", &user->maxPrice);

    printf("Enter new preferred screen type (current: %s): ", user->preferredScreenType);
    scanf_s("%49s", user->preferredScreenType, (unsigned)_countof(user->preferredScreenType));

    printf("User info updated successfully!\n");
}

//允许用户偏好更新模块
void updatePreferences(User* user) {
    printf("Update Preferred Genre (current: %s): ", user->preferredGenre);
    scanf_s("%49s", user->preferredGenre, (unsigned)_countof(user->preferredGenre));

    printf("Update Preferred Viewing Time (current: %s): ", user->preferredTime);
    scanf_s("%5s", user->preferredTime, (unsigned)_countof(user->preferredTime));

    printf("Update Maximum Ticket Price (current: %.2f): ", user->maxPrice);
    scanf_s("%f", &user->maxPrice);

    printf("Update Preferred Screen Type (current: %s): ", user->preferredScreenType);
    scanf_s("%49s", user->preferredScreenType, (unsigned)_countof(user->preferredScreenType));

    printf("Preferences updated successfully!\n");
}


// 查询所有用户信息
void queryAllUsers() {
    printf("Querying all users:\n");
    for (int i = 0; i < usersTable->size; i++) {
        HashTableEntry* entry = usersTable->table[i];
        while (entry != NULL) {
            User* user = (User*)entry->value;
            printf("UserID: %s, Name: %s, Email: %s, Phone: %s, Balance: %.2f\n",
                user->userID, user->name, user->email, user->phone, user->accountBalance);
            entry = entry->next;
        }
    }
}

// 修改管理员信息
void modifyAdminInfo(Admin* admin) {
    printf("Modify Admin Info for Admin ID: %s\n", admin->adminID);
    printf("Enter new name (current: %s): ", admin->name);
    scanf_s("%49s", admin->name, (unsigned)_countof(admin->name));

    printf("Enter new email (current: %s): ", admin->email);
    scanf_s("%99s", admin->email, (unsigned)_countof(admin->email));

    printf("Admin info updated successfully!\n");
}

// 添加新电影场次
void addNewShow() {
    if (showCount >= MAX_ORDERS) {
        printf("Error: Maximum number of shows reached.\n");
        return;
    }

    Show newShow = { 0 };
    printf("Enter show ID: ");
    scanf_s("%9s", newShow.showID, (unsigned)_countof(newShow.showID));

    printf("Enter cinema name: ");
    scanf_s("%49s", newShow.cinemaName, (unsigned)_countof(newShow.cinemaName));

    printf("Enter hall ID: ");
    scanf_s("%9s", newShow.hallID, (unsigned)_countof(newShow.hallID));

    printf("Enter movie title: ");
    scanf_s("%49s", newShow.movie.title, (unsigned)_countof(newShow.movie.title));

    printf("Enter movie genre: ");
    scanf_s("%49s", newShow.movie.genre, (unsigned)_countof(newShow.movie.genre));

    printf("Enter movie duration: ");
    scanf_s("%d", &newShow.movie.duration);

    printf("Enter movie director: ");
    scanf_s("%49s", newShow.movie.director, (unsigned)_countof(newShow.movie.director));

    printf("Enter start time (HH:MM): ");
    scanf_s("%5s", newShow.startTime, (unsigned)_countof(newShow.startTime));

    printf("Enter end time (HH:MM): ");
    scanf_s("%5s", newShow.endTime, (unsigned)_countof(newShow.endTime));

    printf("Enter ticket price: ");
    scanf_s("%f", (float*)&newShow.price);  // Cast to (float *) to match %f

    printf("Enter available tickets: ");
    scanf_s("%d", &newShow.availableTickets);

    printf("Enter show type (e.g., 3D, IMAX): ");
    scanf_s("%9s", newShow.showType, (unsigned)_countof(newShow.showType));

    printf("Enter discount info: ");
    scanf_s("%99s", newShow.discountInfo, (unsigned)_countof(newShow.discountInfo));

    printf("Enter screen type: ");
    scanf_s("%49s", newShow.screenType, (unsigned)_countof(newShow.screenType));

    printf("Enter movie popularity: ");
    scanf_s("%f", &newShow.popularity);

    shows[showCount++] = newShow;
    printf("New show added successfully!\n");
}

// 修改电影场次信息
void modifyShowInfo(Show* show) {
    printf("Modify Show Info for Show ID: %s\n", show->showID);
    printf("Enter new cinema name (current: %s): ", show->cinemaName);
    scanf_s("%49s", show->cinemaName, (unsigned)_countof(show->cinemaName));

    printf("Enter new hall ID (current: %s): ", show->hallID);
    scanf_s("%9s", show->hallID, (unsigned)_countof(show->hallID));

    printf("Enter new movie title (current: %s): ", show->movie.title);
    scanf_s("%49s", show->movie.title, (unsigned)_countof(show->movie.title));

    printf("Enter new start time (current: %s): ", show->startTime);
    scanf_s("%5s", show->startTime, (unsigned)_countof(show->startTime));

    printf("Enter new end time (current: %s): ", show->endTime);
    scanf_s("%5s", show->endTime, (unsigned)_countof(show->endTime));

    printf("Enter new ticket price (current: %.2f): ", show->price);
    scanf_s("%f", (float*)&show->price);  // Cast to (float *) to match %f

    printf("Enter new available tickets (current: %d): ", show->availableTickets);
    scanf_s("%d", &show->availableTickets);

    printf("Enter new show type (current: %s): ", show->showType);
    scanf_s("%9s", show->showType, (unsigned)_countof(show->showType));

    printf("Enter new discount info (current: %s): ", show->discountInfo);
    scanf_s("%99s", show->discountInfo, (unsigned)_countof(show->discountInfo));

    printf("Enter new screen type (current: %s): ", show->screenType);
    scanf_s("%49s", show->screenType, (unsigned)_countof(show->screenType));

    printf("Show info updated successfully!\n");
}
// 票排序
// 按照票数排序
int compareByTicketCount(const void* a, const void* b) {
    const Order* orderA = (const Order*)a;
    const Order* orderB = (const Order*)b;
    int ticketCountA = 0, ticketCountB = 0;

    for (int i = 0; i < 3; i++) {
        if (orderA->seatNumbers[i] != -1) ticketCountA++;
        if (orderB->seatNumbers[i] != -1) ticketCountB++;
    }

    return ticketCountB - ticketCountA; // 降序排序
}
// 按上座率排序
int compareByOccupancyRate(const void* a, const void* b) {
    const Order* orderA = (const Order*)a;
    const Order* orderB = (const Order*)b;

    Show* showA = findShowByID(orderA->showID);
    Show* showB = findShowByID(orderB->showID);

    if (showA && showB) {
        float occupancyRateA = 1.0f - ((float)showA->availableTickets / 100); // 假设100为影厅容量
        float occupancyRateB = 1.0f - ((float)showB->availableTickets / 100);
        return (occupancyRateA < occupancyRateB) - (occupancyRateA > occupancyRateB); // 降序排序
    }
    return 0;
}
void queryShowsAndOrders(Admin* admin) {
    int choice;
    do {
        printf("\n=== Query Shows and Orders ===\n");
        printf("1. View All Shows\n");
        printf("2. View Orders by Ticket Count\n");
        printf("3. View Orders by Occupancy Rate\n");
        printf("4. View Orders by Total Revenue\n");
        printf("5. Back to Admin Menu\n");
        printf("Enter choice: ");
        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            queryAllShows();
            break;
        case 2:
            viewOrdersSortedByTicketCount(admin->cinemaName);
            break;
        case 3:
            viewOrdersSortedByOccupancyRate(admin->cinemaName);
            break;
        case 4:
            viewOrdersSortedByRevenue(admin->cinemaName);
            break;
        case 5:
            return;  // 返回管理员菜单
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 5);
}

// 按收入排序
int compareByRevenue(const void* a, const void* b) {
    const Order* orderA = (const Order*)a;
    const Order* orderB = (const Order*)b;

    return (orderB->totalPrice > orderA->totalPrice) - (orderB->totalPrice < orderA->totalPrice); // 降序排序
}

// 按时间排序
int compareByTime(const void* a, const void* b) {
    const Order* orderA = (const Order*)a;
    const Order* orderB = (const Order*)b;

    return (int)(orderA->orderTime - orderB->orderTime); // 升序排序
}

// 查看order
void viewOrdersSortedByTicketCount(const char* cinemaName) {
    quickSort(orders, 0, orderCount - 1, sizeof(Order), compareByTicketCount);
    printf("=== Orders Sorted by Ticket Count for Cinema %s ===\n", cinemaName);
    for (int i = 0; i < orderCount; i++) {
        printf("OrderID: %s, UserID: %s, ShowID: %s, Total Price: %.2f\n",
            orders[i].orderID, orders[i].userID, orders[i].showID, orders[i].totalPrice);
    }
}

void viewOrdersSortedByOccupancyRate(const char* cinemaName) {
    quickSort(orders, 0, orderCount - 1, sizeof(Order), compareByOccupancyRate);
    printf("=== Orders Sorted by Occupancy Rate for Cinema %s ===\n", cinemaName);
    for (int i = 0; i < orderCount; i++) {
        printf("OrderID: %s, UserID: %s, ShowID: %s, Total Price: %.2f\n",
            orders[i].orderID, orders[i].userID, orders[i].showID, orders[i].totalPrice);
    }
}

void viewOrdersSortedByRevenue(const char* cinemaName) {
    quickSort(orders, 0, orderCount - 1, sizeof(Order), compareByRevenue);
    printf("=== Orders Sorted by Revenue for Cinema %s ===\n", cinemaName);
    for (int i = 0; i < orderCount; i++) {
        printf("OrderID: %s, UserID: %s, ShowID: %s, Total Price: %.2f\n",
            orders[i].orderID, orders[i].userID, orders[i].showID, orders[i].totalPrice);
    }
}

void viewOrdersSortedByTime(const char* cinemaName) {
    quickSort(orders, 0, orderCount - 1, sizeof(Order), compareByTime);
    printf("=== Orders Sorted by Time for Cinema %s ===\n", cinemaName);
    for (int i = 0; i < orderCount; i++) {
        char buffer[26];
        ctime_s(buffer, sizeof(buffer), &orders[i].orderTime); // 使用 ctime_s
        printf("OrderID: %s, UserID: %s, ShowID: %s, Total Price: %.2f, Time: %s",
            orders[i].orderID, orders[i].userID, orders[i].showID, orders[i].totalPrice, buffer);
    }
}
void queryShowsByTitle(const char* title) {
    int found = 0;  // 标记是否找到电影
    printf("Querying shows for movie: %s\n", title);
    for (int i = 0; i < showCount; i++) {
        if (strcmp(shows[i].movie.title, title) == 0) {
            Show* show = &shows[i];
            printf("ShowID: %s, Movie: %s, Cinema: %s, Hall: %s, Start: %s, End: %s, Price: %.2f, Tickets: %d, Screen: %s\n",
                show->showID, show->movie.title, show->cinemaName, show->hallID, show->startTime,
                show->endTime, show->price, show->availableTickets, show->screenType);
            found = 1;
        }
    }
    if (!found) {
        printf("No shows found for movie: %s\n", title);
    }
}


// 查询所有电影场次信息
void queryAllShows() {
    printf("Querying all shows:\n");
    for (int i = 0; i < showCount; i++) {
        Show* show = &shows[i];
        printf("ShowID: %s, Movie: %s, Cinema: %s, Hall: %s, Start: %s, End: %s, Price: %.2f, Tickets: %d, Screen: %s\n",
            show->showID, show->movie.title, show->cinemaName, show->hallID, show->startTime, show->endTime, show->price, show->availableTickets, show->screenType);
    }
}

void manageOrders(Admin* admin) {
    int choice;
    do {
        printf("\n=== Manage Orders for Cinema %s ===\n", admin->cinemaName);
        printf("1. View All Orders\n");
        printf("2. View Orders Sorted by Ticket Count\n");
        printf("3. View Orders Sorted by Occupancy Rate\n");
        printf("4. View Orders Sorted by Total Revenue\n");
        printf("5. View Orders Sorted by Time\n");  // 新增
        printf("6. Exit\n");
        printf("Enter choice: ");
        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            viewAllOrders(admin->cinemaName);
            break;
        case 2:
            viewOrdersSortedByTicketCount(admin->cinemaName);
            break;
        case 3:
            viewOrdersSortedByOccupancyRate(admin->cinemaName);
            break;
        case 4:
            viewOrdersSortedByRevenue(admin->cinemaName);
            break;
        case 5:
            viewOrdersSortedByTime(admin->cinemaName);  // 新增
            break;
        case 6:
            printf("Exiting order management...\n");
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 6);
}
//SeatSub.c
void initialize_seat_settings()
{
    // 使用unsigned long long并明确转换为unsigned int，确保不会发生数据丢失。
    time_t t;
    time(&t);
    srand((unsigned int)(t));

    random_number_kind = rand() % 3 + 1;
    int possible_values[] = { 5, 7, 9 };
    random_number_range = possible_values[rand() % 3];
    random_number_high = rand() % 3 + 3;
}

int choose_seats() {
    int seat_count;
    printf("请输入要预订的座位数量（最多3个）: ");
    scanf_s("%d", &seat_count);

    // 清理输入缓冲区的换行符
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    if (seat_count < 1 || seat_count > 3) {
        printf("无效的座位数量，请重新输入！\n");
        return 0;
    }

    // 调用函数显示影厅座位布局
    Seats(seat_count);

    // 现在用户可以选择他们想要的座位
    for (int i = 0; i < seat_count; i++) {
        int row, col;
        printf("请输入要选择的座位的行和列 (例如: 9 10): ");
        scanf_s("%d %d", &row, &col);

        // 清理输入缓冲区的换行符
        while ((ch = getchar()) != '\n' && ch != EOF);

        // 检查用户选择的座位是否有效
        if (row < 1 || row > ROWS || col < 1 || col > COLS || seats[row - 1][col - 1] == 1) {
            printf("无效或已被预订的座位，请重新选择！\n");
            i--;  // 重新选择座位
            continue;
        }

        // 预订座位
        seats[row - 1][col - 1] = 1;
        printf("已预订第%d行，第%d列的座位。\n", row, col);

        // 检查是否会溢出
        if (num_of_selected_seats >= INT_MAX / sizeof(movie_seat) - 1) {
            printf("座位选择已达到上限，无法选择更多座位。\n");
            return seat_count;
        }

        // 检查乘积是否超过SIZE_MAX
        size_t new_size = (num_of_selected_seats + 1) * sizeof(movie_seat);
        if (new_size / sizeof(movie_seat) != (num_of_selected_seats + 1)) {
            printf("内存分配请求超出限制，无法分配更多内存！\n");
            free(seat_ID);
            exit(1);
        }

        // 动态分配内存并存储座位信息
        movie_seat* temp = realloc(seat_ID, new_size);
        if (temp == NULL) {
            printf("内存分配失败！\n");
            free(seat_ID);
            exit(1);
        }
        seat_ID = temp;
        seat_ID[num_of_selected_seats].row = row;
        seat_ID[num_of_selected_seats].col = col;

        // 获取当前时间并存储
        time_t now;
        struct tm timeinfo;
        char buffer[80];
        time(&now);
        localtime_s(&timeinfo, &now);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
        strcpy_s(seat_ID[num_of_selected_seats].purchase_time, sizeof(seat_ID[num_of_selected_seats].purchase_time), buffer);
        printf("当前时间: %s\n", buffer);

        num_of_selected_seats++;
    }

    return seat_count;
}




// 检查新选择的座位与之前已选座位之间的曼哈顿距离
int is_valid_distance_from_previous(int row, int col)
{
    for (int i = 0; i < num_of_selected_seats; i++)
    {
        int manhattan_distance = abs(seat_ID[i].row - row) + abs(seat_ID[i].col - col);
        if (manhattan_distance < 3)
        {
            return 0;
        }
    }
    return 1;
}

// 推荐座位：根据用户购买的座位数量选择算法
void recommend_seats(int rec_rows[], int rec_cols[], int* rec_count, int seat_count)
{
    *rec_count = 0;
    int total_seats = ROWS * COLS; // 总座位数
    int recommended_seats = (int)(total_seats * 0.1);  // 推荐座位数量为总座位的10%

    if (seat_count == 1)
    {
        // 曼哈顿距离推荐最优座位
        int center_row = ROWS / 2;
        int center_col = COLS / 2;
        int min_distance = ROWS + COLS;

        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                if (seats[i][j] == 0)
                {
                    int distance = abs(i - center_row) + abs(j - center_col);
                    if (distance < min_distance)
                    {
                        min_distance = distance;
                        *rec_count = 0; // 清空已有推荐
                        rec_rows[*rec_count] = i;
                        rec_cols[*rec_count] = j;
                        (*rec_count)++;
                    }
                    else if (distance == min_distance && *rec_count < recommended_seats)  // 相同距离时也推荐
                    {
                        rec_rows[*rec_count] = i;
                        rec_cols[*rec_count] = j;
                        (*rec_count)++;
                    }
                }
            }
        }
    }
    else
    {
        // 使用 BFS 寻找相邻座位
        int directions[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
        int queue[ROWS * COLS][2] = { 0 };
        int visited[ROWS][COLS] = { 0 };
        int front = 0, rear = 0;

        int center_row = ROWS / 2;
        int center_col = COLS / 2;

        queue[rear][0] = center_row;
        queue[rear][1] = center_col;
        rear++;
        visited[center_row][center_col] = 1;

        while (front < rear && *rec_count < recommended_seats)
        {
            int current_row = queue[front][0];
            int current_col = queue[front][1];
            front++;

            if (seats[current_row][current_col] == 0)
            {
                rec_rows[*rec_count] = current_row;
                rec_cols[*rec_count] = current_col;
                (*rec_count)++;
            }

            for (int i = 0; i < 4; i++)
            {
                int new_row = current_row + directions[i][0];
                int new_col = current_col + directions[i][1];

                if (new_row >= 0 && new_row < ROWS && new_col >= 0 && new_col < COLS && !visited[new_row][new_col] && seats[new_row][new_col] == 0)
                {
                    queue[rear][0] = new_row;
                    queue[rear][1] = new_col;
                    rear++;
                    visited[new_row][new_col] = 1;
                }
            }
        }
    }
}


void Seats(int seat_count)
{
    // 计算推荐座位的最大数量，取总座位数的10%
    int max_recommendations = (int)(ROWS * COLS * 0.1) + 1;

    // 使用动态内存分配来替代静态数组
    int* rec_rows = (int*)malloc(max_recommendations * sizeof(int));
    int* rec_cols = (int*)malloc(max_recommendations * sizeof(int));
    int rec_count = 0;

    if (rec_rows == NULL || rec_cols == NULL) {
        printf("内存分配失败！\n");
        if (rec_rows != NULL) free(rec_rows);
        if (rec_cols != NULL) free(rec_cols);
        exit(1);
    }

    recommend_seats(rec_rows, rec_cols, &rec_count, seat_count);

    // 确保 rec_count 不超过 max_recommendations
    if (rec_count > max_recommendations) {
        rec_count = max_recommendations;
    }

    // 调整错误数组访问处
    switch (random_number_kind)
    {
    case 1:
        printf("当前座位布局(正方形):\n");
        printf("     ");
        for (int i = 0; i < COLS; i++)
        {
            printf("%2d   ", i + 1);
        }
        printf("\n");
        for (int i = 0; i < ROWS; i++)
        {
            printf("%2d  ", i + 1);
            for (int j = 0; j < COLS; j++)
            {
                int is_recommended = 0;
                for (int k = 0; k < rec_count; k++)
                {
                    if (i == rec_rows[k] && j == rec_cols[k])
                    {
                        is_recommended = 1;
                        break;
                    }
                }
                if (is_recommended)
                {
                    printf("[荐] ");
                }
                else if (seats[i][j] == 0)
                {
                    printf("[空] ");
                }
                else
                {
                    printf("[XX] ");
                }
            }
            printf("\n");
        }
        printf("推荐您选择带有[荐]标记的位置以获得更好的观影体验!\n");
        break;

    case 2:
    {
        char* array[ROWS][COLS] = { 0 };
        int center = COLS / 2;
        int radius_square = (COLS / 2) * (COLS / 2);
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                int is_recommended = 0;
                for (int k = 0; k < rec_count; k++)
                {
                    if (i == rec_rows[k] && j == rec_cols[k])
                    {
                        is_recommended = 1;
                        break;
                    }
                }
                if (is_recommended)
                {
                    array[i][j] = "[荐]";
                }
                else if (seats[i][j] == 1)
                {
                    array[i][j] = "[XX]";
                }
                else if ((i - center) * (i - center) + (j - center) * (j - center) <= radius_square)
                {
                    array[i][j] = "[空]";
                }
                else
                {
                    array[i][j] = "    ";
                }
            }
        }
        printf("当前座位布局（圆形）：\n");
        printf("    ");
        for (int i = 0; i < COLS; i++)
        {
            printf("%2d  ", i + 1);
        }
        printf("\n");
        for (int i = 0; i < ROWS; i++)
        {
            printf("%2d  ", i + 1);
            for (int j = 0; j < COLS; j++)
            {
                printf("%s", array[i][j]);
            }
            printf("\n");
        }
        printf("推荐您选择带有[荐]标记的位置以获得更好的观影体验!\n");
        break;
    }

    case 3:
    {
        int base_length = random_number_range + 6;
        int top_length = base_length - 2 * (random_number_high - 1);
        printf("当前座位布局（梯形）：\n");
        printf("     ");
        for (int i = 0; i < base_length; i++)
        {
            printf("%2d   ", i + 1);
        }
        printf("\n");
        for (int i = 0; i < random_number_high; i++)
        {
            int current_length = top_length + 2 * i;
            printf("%2d  ", i + 1);
            for (int j = 0; j < (base_length - current_length) / 2; j++)
            {
                printf("     ");
            }
            for (int j = 0; j < current_length; j++)
            {
                int col_index = j + (base_length - current_length) / 2;
                int is_recommended = 0;
                for (int k = 0; k < rec_count; k++)
                {
                    if (i == rec_rows[k] && col_index == rec_cols[k])
                    {
                        is_recommended = 1;
                        break;
                    }
                }
                if (is_recommended)
                {
                    printf("[荐] ");
                }
                else if (seats[i][col_index] == 1)
                {
                    printf("[XX] ");
                }
                else
                {
                    printf("[空] ");
                }
            }
            printf("\n");
        }
        printf("推荐您选择带有[荐]标记的位置以获得更好的观影体验!\n");
        break;
    }

    default:
        printf("出错了！\n");
        break;
    }

    // 释放分配的内存
    free(rec_rows);
    free(rec_cols);
}





void check_seats()
{
    while (1)
    {
        int check_row, check_col;
        printf("请选择你要查询的位置（行 列): ");
        scanf_s("%d %d", &check_row, &check_col);

        // 清理输入缓冲区的换行符
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);

        int seat_found = 0;
        for (int i = 0; i < num_of_selected_seats; i++)
        {
            if (seat_ID[i].row == check_row && seat_ID[i].col == check_col)
            {
                printf("查询结果: %s\n", seat_ID[i].purchase_time);
                seat_found = 1;
                break;
            }
        }
        if (!seat_found)
        {
            printf("无效的查询位置或该位置未被预订！\n");
        }
        return; // 返回主菜单
    }
}

int isSeatAvailable(Show* show, int seatNumber) {
    if (seatNumber < 1 || seatNumber > ROWS * COLS) {
        return 0; // 座位编号无效
    }
    int row = (seatNumber - 1) / COLS;
    int col = (seatNumber - 1) % COLS;
    return seats[row][col] == 0; // 返回座位是否可用
}

// 操作日志记录
void logOperation(const char* operation) {
    FILE* logFile;
    fopen_s(&logFile, "operation_log.txt", "a");
    if (logFile == NULL) {
        perror("Unable to open log file!");
        return;
    }
    fprintf(logFile, "%s\n", operation);
    fclose(logFile);
}

// 数据保存和加载代码省略...
// 从文件加载用户数据
void saveUserDataToFile() {
    FILE* file;
    fopen_s(&file, "users.txt", "wb");
    if (file == NULL) {
        perror("Unable to open file for writing");
        return;
    }

    unsigned char key[] = "mysecretkey";  // 加密密钥

    HashTableEntry* entry;
    for (int i = 0; i < usersTable->size; i++) {
        entry = usersTable->table[i];
        while (entry != NULL) {
            User* user = (User*)entry->value;
            char encryptedData[256] = { 0 };  // 假设加密后的数据不超过256字节

            // 序列化用户数据
            char userData[256];
            sprintf_s(userData, sizeof(userData), "%s %s %c %s %s %s %.2f %s %s %.2f %s",
                user->userID, user->name, user->gender, user->phone, user->passwordHash,
                user->email, user->accountBalance, user->preferredGenre, user->preferredTime,
                user->maxPrice, user->preferredScreenType);

            // 加密用户数据
            simpleEncrypt((unsigned char*)userData, (unsigned char*)encryptedData, key);

            // 写入加密后的数据到文件
            fwrite(encryptedData, sizeof(char), strlen(encryptedData), file);
            fwrite("\n", sizeof(char), 1, file);  // 每条记录用新行分隔

            entry = entry->next;
        }
    }

    fclose(file);
}

// 从文件读取用户数据（解密）
void loadUserDataFromFile() {
    FILE* file;
    fopen_s(&file, "users.txt", "rb");
    if (file == NULL) {
        perror("Unable to open file for reading");
        return;
    }

    unsigned char key[] = "mysecretkey";  // 加密密钥
    char encryptedData[256] = { 0 };  // 假设每条记录不超过256字节
    char decryptedData[256] = { 0 };  // 初始化解密数组

    User user;
    memset(&user, 0, sizeof(User));  // 初始化 user 所有成员为 0

    while (fgets(encryptedData, sizeof(encryptedData), file) != NULL) {
        // 解密用户数据
        simpleEncrypt((unsigned char*)encryptedData, (unsigned char*)decryptedData, key);

        // 将解密后的数据转换为User结构
        sscanf_s(decryptedData, "%9s %49s %c %11s %49s %99s %f %49s %5s %f %49s",
            user.userID, (unsigned)_countof(user.userID),
            user.name, (unsigned)_countof(user.name),
            &user.gender, 1,
            user.phone, (unsigned)_countof(user.phone),
            user.passwordHash, (unsigned)_countof(user.passwordHash),
            user.email, (unsigned)_countof(user.email),
            &user.accountBalance,
            user.preferredGenre, (unsigned)_countof(user.preferredGenre),
            user.preferredTime, (unsigned)_countof(user.preferredTime),
            &user.maxPrice,
            user.preferredScreenType, (unsigned)_countof(user.preferredScreenType));

        addUser(user);  // 加载每个用户后，添加到哈希表

        // 清空用于下次读取的数据
        memset(encryptedData, 0, sizeof(encryptedData));
        memset(decryptedData, 0, sizeof(decryptedData));
    }

    fclose(file);
}

// 保存电影和场次信息到文件（加密）
void saveShowDataToFile() {
    FILE* file;
    fopen_s(&file, "shows.txt", "wb");
    if (file == NULL) {
        perror("Unable to open file for writing");
        return;
    }

    unsigned char key[] = "mysecretkey";  // 加密密钥

    for (int i = 0; i < showCount; i++) {
        Show* show = &shows[i];
        char encryptedData[512] = { 0 };  // 假设加密后的数据不超过512字节

        // 序列化电影数据
        char showData[512];
        sprintf_s(showData, sizeof(showData), "%s %s %s %s %s %s %s %d %d %.2f %s %s %s %.2f",
            show->showID, show->cinemaName, show->hallID, show->movie.title,
            show->startTime, show->endTime, show->movie.genre, show->movie.duration,
            show->availableTickets, show->price, show->showType, show->discountInfo,
            show->screenType, show->popularity);

        // 加密电影数据
        simpleEncrypt((unsigned char*)showData, (unsigned char*)encryptedData, key);

        // 写入加密后的数据到文件
        fwrite(encryptedData, sizeof(char), strlen(encryptedData), file);
        fwrite("\n", sizeof(char), 1, file);  // 每条记录用新行分隔
    }

    fclose(file);
}

// 从文件读取电影和场次信息（解密）
void loadShowDataFromFile() {
    FILE* file;
    fopen_s(&file, "shows.txt", "rb");
    if (file == NULL) {
        perror("Unable to open file for reading");
        return;
    }

    unsigned char key[] = "mysecretkey";  // 加密密钥
    char encryptedData[512] = { 0 };  // 假设每条记录不超过512字节
    char decryptedData[512] = { 0 };  // 初始化解密数组

    Show show;
    memset(&show, 0, sizeof(Show));  // 初始化 show 所有成员为 0

    while (fgets(encryptedData, sizeof(encryptedData), file) != NULL) {
        // 解密电影数据
        simpleEncrypt((unsigned char*)encryptedData, (unsigned char*)decryptedData, key);

        // 将解密后的数据转换为Show结构
        sscanf_s(decryptedData, "%9s %49s %9s %49s %5s %5s %49s %d %d %f %9s %99s %49s %f",
            show.showID, (unsigned)_countof(show.showID),
            show.cinemaName, (unsigned)_countof(show.cinemaName),
            show.hallID, (unsigned)_countof(show.hallID),
            show.movie.title, (unsigned)_countof(show.movie.title),
            show.startTime, (unsigned)_countof(show.startTime),
            show.endTime, (unsigned)_countof(show.endTime),
            show.movie.genre, (unsigned)_countof(show.movie.genre),
            &show.movie.duration,
            &show.availableTickets,
            (float*)&show.price,  // Cast to (float *) to match %f
            show.showType, (unsigned)_countof(show.showType),
            show.discountInfo, (unsigned)_countof(show.discountInfo),
            show.screenType, (unsigned)_countof(show.screenType),
            (float*)&show.popularity);  // Cast to (float *) to match %f

        shows[showCount++] = show;  // 加载每个电影后，添加到全局电影场次数组

        // 清空用于下次读取的数据
        memset(encryptedData, 0, sizeof(encryptedData));
        memset(decryptedData, 0, sizeof(decryptedData));
    }

    fclose(file);
}

// 初始化默认的电影信息
void initializeDefaultShows() {
    if (showCount >= MAX_ORDERS) {
        printf("Error: Maximum number of shows reached.\n");
        return;
    }

    // 默认电影信息1
    Show show1 = { 0 }; // 确保 show1 初始化
    show1.movie = (Movie){ "缘之空", "Science Fiction", 148, "空" };
    strcpy_s(show1.showID, sizeof(show1.showID), "S001");
    strcpy_s(show1.cinemaName, sizeof(show1.cinemaName), "Cinema A");
    strcpy_s(show1.hallID, sizeof(show1.hallID), "Hall 1");
    strcpy_s(show1.movieName, sizeof(show1.movieName), "缘之空");
    strcpy_s(show1.startTime, sizeof(show1.startTime), "18:00");
    strcpy_s(show1.endTime, sizeof(show1.endTime), "20:28");
    show1.duration = 148;
    show1.availableTickets = 100;
    show1.price = 12.5f;
    strcpy_s(show1.showType, sizeof(show1.showType), "3D");
    strcpy_s(show1.discountInfo, sizeof(show1.discountInfo), "10% off for students");
    strcpy_s(show1.genre, sizeof(show1.genre), "Science Fiction");
    show1.popularity = 9.0f;
    strcpy_s(show1.screenType, sizeof(show1.screenType), "IMAX");

    // 检查 showCount 是否超出数组界限
    if (showCount < MAX_ORDERS) {
        shows[showCount++] = show1; // 添加到全局电影场次数组
    }
    else {
        printf("Error: Maximum number of shows reached.\n");
        return;
    }

    // 默认电影信息2
    Show show2 = { 0 }; // 确保 show2 初始化
    show2.movie = (Movie){ "仲夏夜淫梦", "Crime", 175, "帅Otto" };
    strcpy_s(show2.showID, sizeof(show2.showID), "S002");
    strcpy_s(show2.cinemaName, sizeof(show2.cinemaName), "Cinema B");
    strcpy_s(show2.hallID, sizeof(show2.hallID), "Hall 2");
    strcpy_s(show2.movieName, sizeof(show2.movieName), "仲夏夜淫梦");
    strcpy_s(show2.startTime, sizeof(show2.startTime), "20:00");
    strcpy_s(show2.endTime, sizeof(show2.endTime), "22:55");
    show2.duration = 175;
    show2.availableTickets = 80;
    show2.price = 10.0f;
    strcpy_s(show2.showType, sizeof(show2.showType), "Regular");
    strcpy_s(show2.discountInfo, sizeof(show2.discountInfo), "None");
    strcpy_s(show2.genre, sizeof(show2.genre), "Crime");
    show2.popularity = 9.2f;
    strcpy_s(show2.screenType, sizeof(show2.screenType), "Standard");

    // 检查 showCount 是否超出数组界限
    if (showCount < MAX_ORDERS) {
        shows[showCount++] = show2; // 添加到全局电影场次数组
    }
    else {
        printf("Error: Maximum number of shows reached.\n");
        return;
    }

    // 默认电影信息3
    Show show3 = { 0 }; // 确保 show3 初始化
    show3.movie = (Movie){ "我为祖国献石油", "Adventure", 169, "我" };
    strcpy_s(show3.showID, sizeof(show3.showID), "S003");
    strcpy_s(show3.cinemaName, sizeof(show3.cinemaName), "Cinema C");
    strcpy_s(show3.hallID, sizeof(show3.hallID), "Hall 3");
    strcpy_s(show3.movieName, sizeof(show3.movieName), "我为祖国献石油");
    strcpy_s(show3.startTime, sizeof(show3.startTime), "21:00");
    strcpy_s(show3.endTime, sizeof(show3.endTime), "23:49");
    show3.duration = 169;
    show3.availableTickets = 150;
    show3.price = 15.0f;
    strcpy_s(show3.showType, sizeof(show3.showType), "IMAX");
    strcpy_s(show3.discountInfo, sizeof(show3.discountInfo), "Buy 1 Get 1 Free");
    strcpy_s(show3.genre, sizeof(show3.genre), "Adventure");
    show3.popularity = 8.6f;
    strcpy_s(show3.screenType, sizeof(show3.screenType), "IMAX");

    // 检查 showCount 是否超出数组界限
    if (showCount < MAX_ORDERS) {
        shows[showCount++] = show3; // 添加到全局电影场次数组
    }
    else {
        printf("Error: Maximum number of shows reached.\n");
        return;
    }

    printf("Default shows initialized.\n");
}



// 统一的登录函数，根据角色显示不同的菜单
void login() {
    char userID[MAX_ID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter User/Admin ID: ");
    scanf_s("%9s", userID, (unsigned)_countof(userID));

    printf("Enter Password: ");
    scanf_s("%19s", password, (unsigned)_countof(password));

    char hashedPassword[MAX_PASSWORD_LENGTH];
    simpleHash(password, hashedPassword);

    // 检查用户表
    unsigned int hashIndex = hashFunction(userID, usersTable->size);
    HashTableEntry* entry = usersTable->table[hashIndex];
    while (entry) {
        User* user = (User*)entry->value;
        if (strcmp(user->userID, userID) == 0) {
            if (strcmp(user->passwordHash, hashedPassword) == 0) {
                printf("User login successful! Welcome, %s.\n", user->name);
                userMenu(user);  // 用户菜单
                return;
            }
            else {
                printf("Error: Incorrect password.\n");
                return;
            }
        }
        entry = entry->next;
    }

    // 检查管理员表
    hashIndex = hashFunction(userID, adminsTable->size);
    entry = adminsTable->table[hashIndex];
    while (entry) {
        Admin* admin = (Admin*)entry->value;
        if (strcmp(admin->adminID, userID) == 0) {
            if (strcmp(admin->passwordHash, hashedPassword) == 0) {
                printf("Admin login successful! Welcome, %s.\n", admin->name);
                adminMenu(admin);  // 管理员菜单
                return;
            }
            else {
                printf("Error: Incorrect password.\n");
                return;
            }
        }
        entry = entry->next;
    }

    printf("Error: ID not found.\n");
}
// 主程序入口
int main() {
    // Initialize usersTable and adminsTable at the start
    usersTable = initHashTable(100);  // Example size of 100
    adminsTable = initHashTable(100);  // Example size of 100

    int choice;
    while (1) {
        printf("\n=== Main Menu ===\n");
        printf("1. Register User\n");
        printf("2. Login as User\n");
        printf("3. Login as Admin\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            registerUser();
            break;
        case 2:
            loginUser();
            break;
        case 3:
            adminLogin();
            break;
        case 4:
            printf("Exiting...\n");
            return 0;
        default:
            printf("Invalid choice!\n");
        }
    }
    return 0;
}
