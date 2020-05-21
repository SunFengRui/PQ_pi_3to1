#include "pq_linux.h"
#include <QApplication>

#include <QTextCodec>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include "net_init.h"
#include "workthread.h"
#include "socket_send.h"
#include "main.h"
#include <sys/time.h>
#include <signal.h>




struct tm *Start_time;
time_t start_time;
char start_time_s[200];
int threadsum = 0;
pthread_mutex_t fft_mutex;    //
static pthread_t handlePcap1,handlePcap2,handlePcap3;
static pthread_t handleFFT_AThread, handleA_FlickerThread, handleA_HalfPeriodThread;
static pthread_t handleFFT_BThread, handleB_FlickerThread, handleB_HalfPeriodThread;
static pthread_t handleFFT_CThread, handleC_FlickerThread, handleC_HalfPeriodThread;
static pthread_t handle_CheckThread,handleSocketThread;
sem_t FFT_A_semaphore,A_halfcalc_semaphore,A_flicker_semaphore;
sem_t FFT_B_semaphore,B_halfcalc_semaphore,B_flicker_semaphore;
sem_t FFT_C_semaphore,C_halfcalc_semaphore,C_flicker_semaphore;
sem_t data_send_sem;
long cpu_num;
FILE *fp;
void init()
{
    fp=fopen("/home/sun/PQ.txt","w+");

    cpu_num=sysconf(_SC_NPROCESSORS_ONLN);

    time(&start_time);
    Start_time = localtime(&start_time);
    sprintf(start_time_s, "%2d年%2d月%2d日 %2d时:%2d分:%2d秒\n", Start_time->tm_year + 1900, Start_time->tm_mon + 1, Start_time->tm_mday,
    Start_time->tm_hour, Start_time->tm_min, Start_time->tm_sec);  //年月日时分秒

    //10s执行第一次，60s循环执行
    struct itimerval timer;
    memset(&timer,0,sizeof(timer));
    signal(SIGALRM,OneMinuteTimerCallbackFunc);
    timer.it_value.tv_sec=10;
    timer.it_value.tv_usec=0;
    timer.it_interval.tv_sec=60;
    timer.it_interval.tv_usec=0;
    setitimer(ITIMER_REAL,&timer,nullptr);   //ITIMER_REAL SIGALRM

}
void os_create(void)
{
    //cpu_set_t mask;
    //CPU_SET(0x01,&mask);
    //sched_setaffinity(0,sizeof(mask),&mask);

    pthread_mutex_init(&fft_mutex, nullptr);
    sem_init (&FFT_A_semaphore , 0, 0);
    sem_init (&FFT_B_semaphore , 0, 0);
    sem_init (&FFT_C_semaphore , 0, 0);
    sem_init (&A_halfcalc_semaphore , 0, 0);
    sem_init (&B_halfcalc_semaphore , 0, 0);
    sem_init (&C_halfcalc_semaphore , 0, 0);
    sem_init (&data_send_sem , 0, 0);

    pthread_create(&handlePcap1,nullptr,Pcap1ThreadFunc,nullptr);
    threadsum++;
    pthread_create(&handlePcap2,nullptr,Pcap2ThreadFunc,nullptr);
    threadsum++;
    pthread_create(&handlePcap3,nullptr,Pcap3ThreadFunc,nullptr);
    threadsum++;
    pthread_create(&handleFFT_AThread,nullptr,FFT_AThreadFunc, nullptr);
    threadsum++;
    pthread_create(&handleFFT_BThread,nullptr,FFT_BThreadFunc, nullptr);
    threadsum++;
    pthread_create(&handleFFT_CThread,nullptr,FFT_CThreadFunc, nullptr);
    threadsum++;
    pthread_create(&handleA_HalfPeriodThread,nullptr,A_HalfThreadFunc, nullptr);
    threadsum++;
    pthread_create(&handleB_HalfPeriodThread,nullptr,B_HalfThreadFunc, nullptr);
    threadsum++;
    pthread_create(&handleC_HalfPeriodThread,nullptr,C_HalfThreadFunc, nullptr);
    threadsum++;
    pthread_create(&handleA_FlickerThread,nullptr,A_FlickerThreadFunc, nullptr);
    threadsum++;
    pthread_create(&handleB_FlickerThread,nullptr,B_FlickerThreadFunc, nullptr);
    threadsum++;
    pthread_create(&handleC_FlickerThread,nullptr,C_FlickerThreadFunc, nullptr);
    threadsum++;
    pthread_create(&handle_CheckThread,nullptr,CheckThreadFunc, nullptr);
    threadsum++;
    pthread_create(&handleSocketThread,nullptr,SocketThreadFunc, nullptr);
    threadsum++;
}
DList *list_f;
void create_list(int len, double init_val)
{
    list_f = CreateList();
        for(int i=0;i<len;i++)
            InsertList(list_f, init_val);
}
void distroy_list()
{
    DelList(list_f);
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));
    init();

        os_create();

        create_list(10,50.0);
    pq_linux w;
    w.show();

    return a.exec();
}
