#ifndef WORKTHREAD
#define WORKTHREAD

#include <stdlib.h>     /* qsort */

#define AD_SAMPLE_ACCURACY 18
#define mod_value 65536
#define factor_factor (6553.6)   //104857.6
#define FFT_8000 8000
#define HarmonicWave 40
#define HarmonicWaveParam 5
#define PeriodPoint 800
#define PeriodPointMax 880
#define AN_BUFFER_LEN_8000	(8000)
#define Plus_8000 8800
#define AN_FFT_LEN_8000		(AN_BUFFER_LEN_8000)
#define AN_BUFFER_880kLEN	(880*1000)

#define PI 3.1415926
#define phase_param1  10
#define phase_param2  (phase_param1+5)

//短时闪变计算系数
#define Kpointone                      0.0314
#define Kone                           0.0525
#define Kthree                         0.065
#define Kten                           0.28
#define Kfifty                         0.08
typedef struct
{
    #if (AD_SAMPLE_ACCURACY==16)
        short an_ch0;
        short an_ch1;
        short an_ch2;
        short an_ch3;
        short an_ch4;
        short an_ch5;
        unsigned short stand_flag;
    #else      
        int A_U;
        int A_I;
        int B_U;
        int B_I;
        int C_U;
        int C_I;
        int a1;
        int a2;
        int a3;
        int a4;
        int a5;
        int a6;

        u_short check;
        u_short stand_flag;

        int an_ch4;
        int an_ch5;
    #endif
}an_point;
#pragma pack()

typedef union _measuring_results_union
{
    double  indicators_array_double[30];
    char    indicators_array_char[240];
}measuring_results_union;

extern int packet_number ;
extern unsigned long an_buffer_idx;
extern u_int A_err_flag, B_err_flag, C_err_flag;
extern u_short  A_err_current, B_err_current, C_err_current;
extern unsigned long  A_err_sum, B_err_sum, C_err_sum;
extern u_short A_flag, B_flag, C_flag;
extern double A_rms, B_rms, C_rms;//
extern double A_cur_rms, B_cur_rms, C_cur_rms;//
extern double A_active_power;//
extern double A_reactive_power;//
extern double A_apparent_power;
extern double A_active_power_meter;//
extern double A_reactive_power_meter;//
extern double THDU;
extern double A_fre;
extern int pointfre;
extern int A_FFT_Number;
extern double fuzhi_a[HarmonicWave];
extern double fuzhi_a_cur[HarmonicWave];
extern double fftw_phase_a_vol[AN_BUFFER_LEN_8000];
extern double fftw_phase_a_cur[AN_BUFFER_LEN_8000];
extern double fftw_phase_differ[AN_BUFFER_LEN_8000];
extern u_long A_FFT, B_FFT, C_FFT;
extern double B_fre;
extern int B_FFT_Number;
extern double B_active_power;//A�й�����
extern double B_reactive_power;//A�޹�����
extern double B_apparent_power;
extern double B_active_power_meter;//�й����
extern double B_reactive_power_meter;//�޹����
extern double B_THDU, B_THDI;
extern double fuzhi_b[HarmonicWave];
extern double fuzhi_b_cur[HarmonicWave];
extern double fftw_phase_differ_b[AN_BUFFER_LEN_8000];//��ѹ������������
extern double fftw_phase_b[AN_BUFFER_LEN_8000];
extern double fftw_phase_b_cur[HarmonicWave];
extern double C_fre;
extern int C_FFT_Number;
extern double C_active_power;
extern double C_reactive_power;
extern double C_apparent_power ;
extern double C_active_power_meter;
extern double C_reactive_power_meter;
extern double C_THDU, C_THDI;
extern double fuzhi_c[HarmonicWave];
extern double fuzhi_c_cur[HarmonicWave];
extern double fftw_phase_differ_c[AN_BUFFER_LEN_8000];//��ѹ������������
extern double fftw_phase_c[AN_BUFFER_LEN_8000];
extern double fftw_phase_c_cur[HarmonicWave];
extern double A_InstantaneousFlickerValue;
extern double A_ShorttimeFlickerValue;
extern double A_LongtimeFlickerValue;
extern double A_tiaozhibo_f;
extern double A_V_fluctuation;
extern int A_shanbianCount;
extern unsigned int A_instantaneousflickervaluecnt;
extern double B_InstantaneousFlickerValue;
extern double B_ShorttimeFlickerValue;
extern double B_LongtimeFlickerValue;

extern double B_tiaozhibo_f;
extern double B_V_fluctuation;
extern int B_shanbianCount;
extern unsigned int B_instantaneousflickervaluecnt;
extern double C_InstantaneousFlickerValue;
extern double C_ShorttimeFlickerValue;
extern double C_LongtimeFlickerValue;

extern double C_tiaozhibo_f;
extern double C_V_fluctuation;
extern int C_shanbianCount;
extern unsigned int C_instantaneousflickervaluecnt;
extern char A_voltagedipstartflag;
extern char A_voltageswellstartflag;
extern char A_voltageinterruptstartflag;
extern double A_result_800half, A_result_400half;
extern char B_voltagedipstartflag;
extern char B_voltageswellstartflag;
extern char B_voltageinterruptstartflag;
extern double B_result_800half, B_result_400half;
extern char C_voltagedipstartflag;
extern char C_voltageswellstartflag ;
extern char C_voltageinterruptstartflag;
extern double C_result_800half, C_result_400half;
extern double uneg;
extern double uneg_param1, uneg_param2;
extern double BA_phase_average, CA_phase_average;
extern measuring_results_union measuring_results;
extern volatile short an_buffer[AN_BUFFER_880kLEN];
extern double a_channel_index;
void ethernet_protocol_packet_callback(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet);
void *FFT_ThreadFunc(void *arg);
void *A_FlickerThreadFunc(void *arg);
void *A_HalfThreadFunc(void *arg);

void *B_FlickerThreadFunc(void *arg);
void *B_HalfThreadFunc(void *arg);

void *C_FlickerThreadFunc(void *arg);
void *C_HalfThreadFunc(void *arg);


void *CheckThreadFunc(void *arg);
void OneMinuteTimerCallbackFunc(int s);
void indicators2union(void);



#endif // WORKTHREAD

