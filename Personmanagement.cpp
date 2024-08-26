#include "Individual.cpp"
#include <stdio.h>
#include <string.h>
#include "Event.cpp"


static void updateUserInfo(User* user, char* newPhone, char* newPassword, char* newEmail) {
    if (newPhone != NULL) strcpy(user->phone, newPhone);
    if (newPassword != NULL) strcpy(user->password, newPassword);
    if (newEmail != NULL) strcpy(user->email, newEmail);
}

void cancelOrder(Order* order) {
    if (!order->isPaid) {
        order->totalPrice = 0;
        // Reset other order fields as necessary
    }
}
