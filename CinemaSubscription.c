#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // 新增头文件用于字符检测
#include <time.h>
#include <math.h>

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
} Order;

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

// 全局变量
HashTable* usersTable;
HashTable* adminsTable;
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
// 核心功能实现的代码保持不变


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
    unsigned int hashIndex = hashFunction(user.userID, usersTable->size);
    HashTableEntry* newEntry = (HashTableEntry*)malloc(sizeof(HashTableEntry));
    if (!newEntry) {  // 检查内存分配是否成功
        printf("Error: Unable to allocate memory for hash table entry.\n");
        exit(1);
    }

    strcpy_s(newEntry->key, sizeof(newEntry->key), user.userID);
    newEntry->value = malloc(sizeof(User));
    if (!newEntry->value) {  // 检查内存分配是否成功
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
        printf("5. Logout\n");
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
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 5);
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
        printf("5. Query All Shows\n");
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
            queryAllShows();
            break;
        case 6:
            printf("Logging out...\n");
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 6);
}


// 登录用户
void loginUser() {
    char userID[MAX_ID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter User ID: ");
    scanf_s("%9s", userID, (unsigned)_countof(userID));  // 使用 scanf_s 并指定缓冲区大小

    printf("Enter Password: ");
    scanf_s("%19s", password, (unsigned)_countof(password));  // 使用 scanf_s 并指定缓冲区大小

    char hashedPassword[MAX_PASSWORD_LENGTH];
    simpleHash(password, hashedPassword);

    unsigned int hashIndex = hashFunction(userID, usersTable->size);
    HashTableEntry* entry = usersTable->table[hashIndex];
    while (entry) {
        User* user = (User*)entry->value;
        if (strcmp(user->userID, userID) == 0) {
            if (strcmp(user->passwordHash, hashedPassword) == 0) {
                printf("Login successful! Welcome, %s.\n", user->name);
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
    float boxOfficeFactor = show->price * (float)(show->availableTickets - 100); // 假设卖了100张票
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
    int hour = atoi(strtok(show->startTime, ":"));  // 提取放映时间的小时部分
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
    scanf_s("%f", &newShow.price);

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
    scanf_s("%f", &show->price);

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

// 查询所有电影场次信息
void queryAllShows() {
    printf("Querying all shows:\n");
    for (int i = 0; i < showCount; i++) {
        Show* show = &shows[i];
        printf("ShowID: %s, Movie: %s, Cinema: %s, Hall: %s, Start: %s, End: %s, Price: %.2f, Tickets: %d, Screen: %s\n",
            show->showID, show->movie.title, show->cinemaName, show->hallID, show->startTime, show->endTime, show->price, show->availableTickets, show->screenType);
    }
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
            &show.price,
            show.showType, (unsigned)_countof(show.showType),
            show.discountInfo, (unsigned)_countof(show.discountInfo),
            show.screenType, (unsigned)_countof(show.screenType),
            &show.popularity);

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


// 主程序入口
int main() {
    usersTable = initHashTable(100);
    adminsTable = initHashTable(100);
    showsTree = NULL;

    initializeDefaultShows(); // 初始化默认电影信息
    loadUserDataFromFile();   // 从文件加载用户数据
    loadShowDataFromFile();   // 从文件加载电影场次数据

    int choice;
    do {
        printf("\n=== Main Menu ===\n");
        printf("1. Register User\n");
        printf("2. Login\n");
        printf("3. Recharge User\n");
        printf("4. Exit\n");
        printf("Enter choice: ");

        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            registerUser();
            break;
        case 2:
            login();  // 调整为统一的登录函数
            break;
        case 3:
            rechargeUser();
            break;
        case 4:
            saveUserDataToFile();  // 退出前保存用户数据到文件
            saveShowDataToFile();  // 退出前保存电影场次数据到文件
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice!\n");
        }
    } while (choice != 4);

    return 0;
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
