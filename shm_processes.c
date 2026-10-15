#define _DEFAULT_SOURCE   /* silences the _SVID_SOURCE deprecation warning from the Makefile flags */
#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <unistd.h>
#include  <sys/wait.h>
#include  <time.h>

#define LOOPS 25

void  DadProcess(int []);
void  StudentProcess(int []);

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;

     (void) argc;
     (void) argv;

     /* ShmPTR[0] = BankAccount, ShmPTR[1] = Turn */
     ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Process has received a shared memory of two integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (ShmPTR == (int *) -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Process has attached the shared memory...\n");

     ShmPTR[0] = 0;   /* BankAccount */
     ShmPTR[1] = 0;   /* Turn */
     printf("Orig Bank Account = %d\n", ShmPTR[0]);
     fflush(stdout);

     pid = fork();
     if (pid < 0) {
          printf("*** fork error (server) ***\n");
          exit(1);
     }
     else if (pid == 0) {
          StudentProcess(ShmPTR);
          exit(0);
     }

     DadProcess(ShmPTR);

     wait(&status);
     shmdt((void *) ShmPTR);
     shmctl(ShmID, IPC_RMID, NULL);
     exit(0);
}

void  DadProcess(int  SharedMem[])
{
     int i, account, balance;
     int *BankAccount = &SharedMem[0];
     volatile int *Turn = &SharedMem[1];

     srand((unsigned) time(NULL) ^ (unsigned) getpid());

     for (i = 0; i < LOOPS; i++) {
          sleep(rand() % 6);
          account = *BankAccount;
          while (*Turn != 0)
               ;   /* no-op */
          account = *BankAccount;   /* re-read now that it is our turn */

          if (account <= 100) {
               balance = rand() % 101;
               if (balance % 2 == 0) {
                    account += balance;
                    printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
               }
               else {
                    printf("Dear old Dad: Doesn't have any money to give\n");
               }
          }
          else {
               printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
          }
          fflush(stdout);

          *BankAccount = account;
          *Turn = 1;
     }
}

void  StudentProcess(int  SharedMem[])
{
     int i, account, balance;
     int *BankAccount = &SharedMem[0];
     volatile int *Turn = &SharedMem[1];

     srand((unsigned) time(NULL) ^ ((unsigned) getpid() << 8));

     for (i = 0; i < LOOPS; i++) {
          sleep(rand() % 6);
          account = *BankAccount;
          while (*Turn != 1)
               ;   /* no-op */
          account = *BankAccount;   /* re-read now that it is our turn */

          balance = rand() % 51;
          printf("Poor Student needs $%d\n", balance);
          if (balance <= account) {
               account -= balance;
               printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
          }
          else {
               printf("Poor Student: Not Enough Cash ($%d)\n", account);
          }
          fflush(stdout);

          *BankAccount = account;
          *Turn = 0;
     }
}
