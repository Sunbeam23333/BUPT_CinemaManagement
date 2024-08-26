#define MAX_NAME_LENGTH 50
#define MAX_EMAIL_LENGTH 100
#define MAX_PHONE_LENGTH 12
#define MAX_PASSWORD_LENGTH 20
#define MAX_ID_LENGTH 10

typedef struct {
    char userID[MAX_ID_LENGTH];
    char name[MAX_NAME_LENGTH];
    char gender;  // 'M' or 'F'
    char phone[MAX_PHONE_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    float accountBalance;
} User;

typedef struct {
    char adminID[MAX_ID_LENGTH];
    char cinemaName[MAX_NAME_LENGTH];
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
} Admin;
