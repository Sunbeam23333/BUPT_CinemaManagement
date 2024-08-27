#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // 新增头文件用于字符检测
#include <time.h>

// 常量定义
#define MAX_NAME_LENGTH 50
#define MAX_EMAIL_LENGTH 100
#define MAX_PHONE_LENGTH 12
#define MAX_PASSWORD_LENGTH 20
#define MAX_ID_LENGTH 10
#define MAX_USERS 100
#define MAX_ORDERS 100

// 数据结构定义和全局变量
typedef struct {
    char userID[MAX_ID_LENGTH];
    char name[MAX_NAME_LENGTH];
    char gender;
    char phone[MAX_PHONE_LENGTH];
    char passwordHash[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    float accountBalance;
} User;

typedef struct {
    char adminID[MAX_ID_LENGTH];
    char cinemaName[MAX_NAME_LENGTH];
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char passwordHash[MAX_PASSWORD_LENGTH];
} Admin;

typedef struct {
    char showID[MAX_ID_LENGTH];
    char cinemaName[MAX_NAME_LENGTH];
    char hallID[MAX_ID_LENGTH];
    char movieName[MAX_NAME_LENGTH];
    char startTime[6];
    char endTime[6];
    int duration;
    int availableTickets;
    float price;
    char showType[10];
    char discountInfo[100];
} Show;

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

// 函数声明
void simpleHash(const char* input, char* outputHash);
void simpleEncrypt(const unsigned char* data, unsigned char* encryptedData, const unsigned char* key);
void addUser(User user);
void addAdmin(Admin admin);
void addShow(Show show);
void registerUser();
void loginUser();
void rechargeUser();
void adminLogin();
void manageShows();
void saveUserDataToFile();
void loadUserDataFromFile();
void logOperation(const char* operation);
AVLNode* insertAVLNode(AVLNode* node, Show show);
AVLNode* rightRotate(AVLNode* y);
AVLNode* leftRotate(AVLNode* x);
int height(AVLNode* node);
int max(int a, int b);
int getBalance(AVLNode* node);
void printAVLTree(AVLNode* node);
HashTable* initHashTable(int size);
unsigned int hashFunction(const char* key, int tableSize);

// 核心功能实现

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
    hashTable->size = size;
    hashTable->table = (HashTableEntry**)malloc(sizeof(HashTableEntry*) * size);
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
    strcpy_s(newEntry->key, sizeof(newEntry->key), user.userID);
    newEntry->value = malloc(sizeof(User));
    memcpy(newEntry->value, &user, sizeof(User));
    newEntry->next = usersTable->table[hashIndex];
    usersTable->table[hashIndex] = newEntry;
}

// 添加管理员
void addAdmin(Admin admin) {
    unsigned int hashIndex = hashFunction(admin.adminID, adminsTable->size);
    HashTableEntry* newEntry = (HashTableEntry*)malloc(sizeof(HashTableEntry));
    strcpy_s(newEntry->key, sizeof(newEntry->key), admin.adminID);
    newEntry->value = malloc(sizeof(Admin));
    memcpy(newEntry->value, &admin, sizeof(Admin));
    newEntry->next = adminsTable->table[hashIndex];
    adminsTable->table[hashIndex] = newEntry;
}

// 注册用户
void registerUser() {
    User user;
    printf("Enter User ID: ");
    scanf("%s", user.userID);
    // 检查 User ID 是否只包含字母和数字
    for (int i = 0; i < strlen(user.userID); i++) {
        if (!isalnum(user.userID[i])) {
            printf("Error: User ID must contain only letters and digits.\n");
            return;
        }
    }
    printf("Enter Name: ");
    scanf("%s", user.name);
    printf("Enter Gender (M/F): ");
    scanf(" %c", &user.gender);
    // 检查 Gender 是否为 'M' 或 'F'
    if (user.gender != 'M' && user.gender != 'F') {
        printf("Error: Gender must be 'M' or 'F'.\n");
        return;
    }
    printf("Enter Phone: ");
    scanf("%s", user.phone);
    // 检查电话号码是否只包含数字
    for (int i = 0; i < strlen(user.phone); i++) {
        if (!isdigit(user.phone[i])) {
            printf("Error: Phone number must contain only digits.\n");
            return;
        }
    }
    printf("Enter Email: ");
    scanf("%s", user.email);
    printf("Enter Password: ");
    char password[MAX_PASSWORD_LENGTH];
    scanf("%s", password);
    // 检查密码是否只包含字母和数字
    for (int i = 0; i < strlen(password); i++) {
        if (!isalnum(password[i])) {
            printf("Error: Password must contain only letters and digits.\n");
            return;
        }
    }
    simpleHash(password, user.passwordHash);
    user.accountBalance = 0.0f;
    addUser(user);
    printf("User registered successfully!\n");
}

// 登录用户
void loginUser() {
    char userID[MAX_ID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    printf("Enter User ID: ");
    scanf("%s", userID);
    printf("Enter Password: ");
    scanf("%s", password);
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
            } else {
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
    scanf("%s", userID);
    printf("Enter amount to recharge: ");
    scanf("%f", &amount);
    
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

// 管理员登录
void adminLogin() {
    char adminID[MAX_ID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    printf("Enter Admin ID: ");
    scanf("%s", adminID);
    printf("Enter Password: ");
    scanf("%s", password);
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
            } else {
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

// 主程序入口
int main() {
    usersTable = initHashTable(100);
    adminsTable = initHashTable(100);
    showsTree = NULL;

    loadUserDataFromFile(); // 从文件加载用户数据

    int choice;
    do {
        printf("1. Register User\n");
        printf("2. Login User\n");
        printf("3. Recharge User\n");
        printf("4. Admin Login\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            case 3:
                rechargeUser();
                break;
            case 4:
                adminLogin();
                break;
            case 5:
                saveUserDataToFile(); // 退出前保存用户数据到文件
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice!\n");
        }
    } while (choice != 5);

    return 0;
}
