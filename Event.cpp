#include "individual.cpp"
#include "Scene.cpp"
#include <stdio.h>
#include <string.h>

int purchaseTicket(User* user, Show* show, int seatNumbers[], int seatCount) {
    if (user->accountBalance < show->price * seatCount) {
        return -1;  // 余额不足
    }

    // 座位和订单检查逻辑
    // 更新账户余额和场次余票数

    return 0;  // 购买成功
}

int loginUser(User users[], int userCount, char userID[], char password[]) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].userID, userID) == 0 && strcmp(users[i].password, password) == 0) {
            return i;  // 返回用户的索引
        }
    }
    return -1;  // 登录失败
}

int loginAdmin(Admin admins[], int adminCount, char adminID[], char password[]) {
    for (int i = 0; i < adminCount; i++) {
        if (strcmp(admins[i].adminID, adminID) == 0 && strcmp(admins[i].password, password) == 0) {
            return i;  // 返回管理员的索引
        }
    }
    return -1;  // 登录失败
}

