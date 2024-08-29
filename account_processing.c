#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Define the Account structure
typedef struct {
    int balance;
    pthread_mutex_t lock; // Mutex to protect the balance
} Account;

// Define a structure to hold the arguments for thread functions
typedef struct {
    Account* acc;
    int amount;
} ThreadArgs;

// Function to deposit an amount to the account
void deposit(Account* acc, int amount) {
    pthread_mutex_lock(&acc->lock); // Enter critical section
    acc->balance += amount;
    printf("Deposited %d, New Balance: %d\n", amount, acc->balance);
    pthread_mutex_unlock(&acc->lock); // Exit critical section
}

// Function to withdraw an amount from the account
void withdraw(Account* acc, int amount) {
    pthread_mutex_lock(&acc->lock); // Enter critical section
    if (acc->balance >= amount) {
        acc->balance -= amount;
        printf("Withdrew %d, New Balance: %d\n", amount, acc->balance);
    } else {
        printf("Insufficient funds to withdraw %d, Current Balance: %d\n", amount, acc->balance);
    }
    pthread_mutex_unlock(&acc->lock); // Exit critical section
}

// Thread function for a customer performing a transaction
void* customer_routine(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;

    // Perform a deposit or withdrawal based on the amount sign
    if (args->amount > 0) {
        deposit(args->acc, args->amount);
    } else {
        withdraw(args->acc, -args->amount);
    }

    free(args); // Free the allocated memory for thread arguments
    return NULL;
}

int main() {
    // Initialize the account with a balance of 1000
    Account account = {1000, PTHREAD_MUTEX_INITIALIZER};

    // Create threads for different transactions
    pthread_t threads[4];

    // Transaction 1: Deposit 500
    ThreadArgs* args1 = malloc(sizeof(ThreadArgs));
    args1->acc = &account;
    args1->amount = 500;
    pthread_create(&threads[0], NULL, customer_routine, (void*)args1);

    // Transaction 2: Withdraw 300
    ThreadArgs* args2 = malloc(sizeof(ThreadArgs));
    args2->acc = &account;
    args2->amount = -300;
    pthread_create(&threads[1], NULL, customer_routine, (void*)args2);

    // Transaction 3: Withdraw 800
    ThreadArgs* args3 = malloc(sizeof(ThreadArgs));
    args3->acc = &account;
    args3->amount = -800;
    pthread_create(&threads[2], NULL, customer_routine, (void*)args3);

    // Transaction 4: Deposit 200
    ThreadArgs* args4 = malloc(sizeof(ThreadArgs));
    args4->acc = &account;
    args4->amount = 200;
    pthread_create(&threads[3], NULL, customer_routine, (void*)args4);

    // Wait for all threads to finish
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    // Final balance
    printf("Final Balance: %d\n", account.balance);

    // Destroy the mutex
    pthread_mutex_destroy(&account.lock);

    return 0;
}
