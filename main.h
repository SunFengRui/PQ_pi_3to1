#ifndef MAIN_H
#define MAIN_H

#include <semaphore.h>
#include <stdio.h>
#include "dlist.h"
extern struct tm *Start_time;
extern time_t start_time;
extern char start_time_s[200];
extern int threadsum;

extern pthread_mutex_t fft_mutex;
extern sem_t FFT_A_semaphore,A_halfcalc_semaphore,A_flicker_semaphore;
extern sem_t FFT_B_semaphore,B_halfcalc_semaphore,B_flicker_semaphore;
extern sem_t FFT_C_semaphore,C_halfcalc_semaphore,C_flicker_semaphore;
extern sem_t data_send_sem;
extern long cpu_num;
extern FILE *fp;
extern int index_800;
extern DList *list_f;
#endif // MAIN_H
