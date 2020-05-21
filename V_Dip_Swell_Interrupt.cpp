#include "V_Dip_Swell_Interrupt.h"
#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include "workthread.h"
static struct timezone tz;
double A_VoltagedipDepth, A_VoltagedipLastVoltageResult;
int A_VoltagedipDurationTime, A_VoltagedipDurationLastTime;
static struct timeval A_VoltagedipDurationStartTime,A_VoltagedipDurationeEndTime;  //AJSNFCJISNA
static double A_VoltagedipVoltageTemp;
static double A_VoltageswellVoltageTemp;
double A_VoltageswellVoltageResult,A_VoltageswellLastVoltageResult;
int A_VoltageswellDurationTime, A_VoltageswellDurationLastTime;
static struct timeval A_VoltageswellDurationStartTime,A_VoltageswellDurationEndTime;
static double A_VoltageinterruptVoltageTemp;
double A_VoltageinterruptVoltageResult, A_VoltageinterruptLastVoltageResult;
int A_VoltageinterruptionDurationTime, A_VoltageinterruptionDurationLastTime;
static struct timeval A_VoltageinterruptionDurationStartTime,A_VoltageinterruptionDurationEndTime;

/******************************************************************************************/
void A_voltagedipcalculation(void)  
{
	/*updata the Ures*/
	if (A_result_800half< A_VoltagedipVoltageTemp)
		A_VoltagedipVoltageTemp = A_result_800half;   
											
	A_VoltagedipDepth = A_VoltagedipVoltageTemp;
    gettimeofday (&A_VoltagedipDurationeEndTime , &tz);
    A_VoltagedipDurationTime = (A_VoltagedipDurationeEndTime.tv_sec-A_VoltagedipDurationStartTime.tv_sec)*1000+(A_VoltagedipDurationeEndTime.tv_usec-A_VoltagedipDurationStartTime.tv_usec)/1000;
	if ((A_result_800half >= (VoltagedipThreshold + 0.02*DeclaredInputVoltageUdin)) || (A_result_800half<VoltageinterruptThreshold))       
	{
		A_VoltagedipLastVoltageResult = A_VoltagedipDepth;
		A_VoltagedipDurationLastTime = A_VoltagedipDurationTime;
		A_VoltagedipDurationTime = 0;
		A_voltagedipstartflag = 0;
		A_VoltagedipDepth = 0;
	}
}

void A_voltageswellcalculation(void)
{
	/*updata the Umax*/
	if (A_result_800half>A_VoltageswellVoltageTemp)
		A_VoltageswellVoltageTemp = A_result_800half;     
															 /*supply voltage swells ended*/
     A_VoltageswellVoltageResult = A_VoltageswellVoltageTemp;
     gettimeofday (&A_VoltageswellDurationEndTime , &tz);
     A_VoltageswellDurationTime = (A_VoltageswellDurationEndTime.tv_sec-A_VoltageswellDurationStartTime.tv_sec)*1000+(A_VoltageswellDurationEndTime.tv_usec-A_VoltageswellDurationStartTime.tv_usec)/1000;
	if ((A_result_800half <= (VoltageswellThreshold - 0.02*DeclaredInputVoltageUdin)))  
	{
		A_VoltageswellDurationLastTime = A_VoltageswellDurationTime;
		A_VoltageswellLastVoltageResult = A_VoltageswellVoltageResult;
		A_voltageswellstartflag = 0;
		A_VoltageswellDurationTime = 0;      
		A_VoltageswellVoltageResult = 0;
	}
}

void A_voltageinterruptioncalculation(void)
{
	/*updata the Ures*/
	if (A_result_800half<A_VoltageinterruptVoltageTemp)
		A_VoltageinterruptVoltageTemp = A_result_800half;
	/*supply voltage interruptions ended*/
	A_VoltageinterruptVoltageResult = A_VoltageinterruptVoltageTemp;
    gettimeofday (&A_VoltageinterruptionDurationEndTime , &tz);
    A_VoltageinterruptionDurationTime = (A_VoltageinterruptionDurationEndTime.tv_sec-A_VoltageinterruptionDurationStartTime.tv_sec)*1000+(A_VoltageinterruptionDurationEndTime.tv_usec-A_VoltageinterruptionDurationStartTime.tv_usec)/1000;
	
	if ((A_result_800half >= (VoltageinterruptThreshold + 0.02*DeclaredInputVoltageUdin)))  
	{
		A_VoltageinterruptionDurationLastTime = A_VoltageinterruptionDurationTime;
		A_VoltageinterruptLastVoltageResult = A_VoltageinterruptVoltageResult;
		A_voltageinterruptstartflag=0;
		A_VoltageinterruptionDurationTime = 0;
		A_VoltageinterruptVoltageResult = 0;
	}
}

/*
ms count
#include <sys/time.h>
struct timeval tvafter,tvpre;
struct timezone tz;
gettimeofday (&tvpre , &tz);
gettimeofday (&tvafter , &tz);
time_interval=(tvafter.tv_sec-tvpre.tv_sec)*1000+(tvafter.tv_usec-tvpre.tv_usec)/1000);
*/
char A_dip[100], A_swell[100], A_interrupt[100];
void A_voltagedipswellinterruptiondetection(void)
{
    struct tm *Diptime,*Swelltime, *Interrupttime;;
    time_t now_time;

	/*supply voltage dips occurred*/
	if (A_voltagedipstartflag == 0) 
	{
		if ((A_result_800half < VoltagedipThreshold) && (A_result_800half>VoltageinterruptThreshold))  
		{
            time(&now_time);
            Diptime = localtime(&now_time);
            sprintf(A_dip, "%2d-%2d-%2d %2d:%2d:%2d\n", Diptime->tm_year + 1900, Diptime->tm_mon + 1, Diptime->tm_mday,
            Diptime->tm_hour, Diptime->tm_min, Diptime->tm_sec);

            gettimeofday (&A_VoltagedipDurationStartTime , &tz);
			A_voltagedipstartflag = 1;
			/*Initialize the Ures*/
			A_VoltagedipVoltageTemp = A_result_800half;
		}
	}
	if (A_voltageswellstartflag == 0)
	{

		if ((A_result_800half> VoltageswellThreshold))   
		{

            time(&now_time);
            Swelltime = localtime(&now_time);
            sprintf(A_swell, "%2d-%2d-%2d %2d:%2d:%2d\n", Swelltime->tm_year + 1900, Swelltime->tm_mon + 1, Swelltime->tm_mday,
            Swelltime->tm_hour, Swelltime->tm_min, Swelltime->tm_sec);
            gettimeofday (&A_VoltageswellDurationStartTime , &tz);

			A_voltageswellstartflag = 1;

			A_VoltageswellVoltageTemp = A_result_800half;
		}
	}
	if ( A_voltageinterruptstartflag== 0)
	{
		if ((A_result_800half < VoltageinterruptThreshold))   

		{
            time(&now_time);
            Interrupttime = localtime(&now_time);
            sprintf(A_interrupt, "%2d-%2d-%2d %2d:%2d:%2d\n", Interrupttime->tm_year + 1900, Interrupttime->tm_mon + 1, Interrupttime->tm_mday,
            Interrupttime->tm_hour, Interrupttime->tm_min, Interrupttime->tm_sec);
            gettimeofday (&A_VoltageinterruptionDurationStartTime , &tz);
			A_voltageinterruptstartflag = 1;

			A_VoltageinterruptVoltageTemp = A_result_800half;
		}
	}
}
double B_VoltagedipDepth, B_VoltagedipLastVoltageResult;
int B_VoltagedipDurationTime, B_VoltagedipDurationLastTime;
static struct timeval B_VoltagedipDurationStartTime,B_VoltagedipDurationEndTime;
static double B_VoltagedipVoltageTemp;
static double B_VoltageswellVoltageTemp;
double B_VoltageswellVoltageResult, B_VoltageswellLastVoltageResult;
int B_VoltageswellDurationTime, B_VoltageswellDurationLastTime;
static struct timeval B_VoltageswellDurationStartTime,B_VoltageswellDurationEndTime;
static double B_VoltageinterruptVoltageTemp;
double B_VoltageinterruptVoltageResult, B_VoltageinterruptLastVoltageResult;
int B_VoltageinterruptionDurationTime, B_VoltageinterruptionDurationLastTime;
static struct timeval B_VoltageinterruptionDurationStartTime,B_VoltageinterruptionDurationEndTime;

void B_voltagedipcalculation(void)  
{

	if (B_result_800half < B_VoltagedipVoltageTemp)
		B_VoltagedipVoltageTemp = B_result_800half;   

	B_VoltagedipDepth = B_VoltagedipVoltageTemp;
    gettimeofday (&B_VoltagedipDurationEndTime , &tz);
    B_VoltagedipDurationTime = (B_VoltagedipDurationEndTime.tv_sec-B_VoltagedipDurationStartTime.tv_sec)*1000+(B_VoltagedipDurationEndTime.tv_usec-B_VoltagedipDurationStartTime.tv_usec)/1000;
	if ((B_result_800half >= (VoltagedipThreshold + 0.02*DeclaredInputVoltageUdin)) || (B_result_800half < VoltageinterruptThreshold))       
	{
		B_VoltagedipLastVoltageResult = B_VoltagedipDepth;
		B_VoltagedipDurationLastTime = B_VoltagedipDurationTime;
		B_VoltagedipDurationTime = 0;
		B_voltagedipstartflag = 0;
		B_VoltagedipDepth = 0;
	}
}
void B_voltageswellcalculation(void)
{

	if (B_result_800half > B_VoltageswellVoltageTemp)
		B_VoltageswellVoltageTemp = B_result_800half;     

	B_VoltageswellVoltageResult = B_VoltageswellVoltageTemp;
    gettimeofday (&B_VoltageswellDurationEndTime , &tz);
    B_VoltageswellDurationTime = (B_VoltageswellDurationEndTime.tv_sec-B_VoltageswellDurationStartTime.tv_sec)*1000+(B_VoltageswellDurationEndTime.tv_usec-B_VoltageswellDurationStartTime.tv_usec)/1000;

	if ((B_result_800half <= (VoltageswellThreshold - 0.02*DeclaredInputVoltageUdin)))  
	{
		B_VoltageswellDurationLastTime = B_VoltageswellDurationTime;
		B_VoltageswellLastVoltageResult = B_VoltageswellVoltageResult;
		B_voltageswellstartflag = 0;
		B_VoltageswellDurationTime = 0;      
		B_VoltageswellVoltageResult = 0;
	}
}
void B_voltageinterruptioncalculation(void)
{

	if (B_result_800half < B_VoltageinterruptVoltageTemp)
		B_VoltageinterruptVoltageTemp = B_result_800half;

	B_VoltageinterruptVoltageResult = B_VoltageinterruptVoltageTemp;
    gettimeofday (&B_VoltageinterruptionDurationEndTime , &tz);
        B_VoltageinterruptionDurationTime = (B_VoltageinterruptionDurationEndTime.tv_sec-B_VoltageinterruptionDurationStartTime.tv_sec)*1000+(B_VoltageinterruptionDurationEndTime.tv_usec-B_VoltageinterruptionDurationStartTime.tv_usec)/1000;

	if ((B_result_800half >= (VoltageinterruptThreshold + 0.02*DeclaredInputVoltageUdin)))  
	{
		B_VoltageinterruptionDurationLastTime = B_VoltageinterruptionDurationTime;
		B_VoltageinterruptLastVoltageResult = B_VoltageinterruptVoltageResult;
		B_voltageinterruptstartflag = 0;
		B_VoltageinterruptionDurationTime = 0;
		B_VoltageinterruptVoltageResult = 0;
	}
}

char B_dip[100], B_swell[100], B_interrupt[100];
void B_voltagedipswellinterruptiondetection(void)
{
    struct tm *Diptime,*Swelltime, *Interrupttime;;
    time_t now_time;


	if (B_voltagedipstartflag == 0) 
	{
		if ((B_result_800half < VoltagedipThreshold) && (B_result_800half > VoltageinterruptThreshold))  
		{
            time(&now_time);
            Diptime = localtime(&now_time);
            sprintf(A_dip, "%2d-%2d-%2d %2d:%2d:%2d\n", Diptime->tm_year + 1900, Diptime->tm_mon + 1, Diptime->tm_mday,
            Diptime->tm_hour, Diptime->tm_min, Diptime->tm_sec);

            gettimeofday (&B_VoltagedipDurationStartTime , &tz);
			B_voltagedipstartflag = 1;

			B_VoltagedipVoltageTemp = B_result_800half;
		}
	}
	if (B_voltageswellstartflag == 0)
	{

		if ((B_result_800half > VoltageswellThreshold))   
		{
            time(&now_time);
            Diptime = localtime(&now_time);
            sprintf(A_dip, "%2d-%2d-%2d %2d:%2d:%2d\n", Swelltime->tm_year + 1900, Swelltime->tm_mon + 1, Swelltime->tm_mday,
            Swelltime->tm_hour, Swelltime->tm_min, Swelltime->tm_sec);
            gettimeofday (&B_VoltageswellDurationStartTime , &tz);
			B_voltageswellstartflag = 1;

			B_VoltageswellVoltageTemp = B_result_800half;
		}
	}
	if (B_voltageinterruptstartflag == 0)
	{

		if ((B_result_800half < VoltageinterruptThreshold))   

		{
            time(&now_time);
            Diptime = localtime(&now_time);
            sprintf(A_dip, "%2d-%2d-%2d %2d:%2d:%2d\n", Interrupttime->tm_year + 1900, Interrupttime->tm_mon + 1, Interrupttime->tm_mday,
            Interrupttime->tm_hour, Interrupttime->tm_min, Interrupttime->tm_sec);
            gettimeofday (&B_VoltageinterruptionDurationStartTime , &tz);
			B_voltageinterruptstartflag = 1;

			B_VoltageinterruptVoltageTemp = B_result_800half;
		}
	}
}
double C_VoltagedipDepth, C_VoltagedipLastVoltageResult;
int C_VoltagedipDurationTime, C_VoltagedipDurationLastTime;
static struct timeval C_VoltagedipDurationStartTime,C_VoltagedipDurationEndTime;
static double C_VoltagedipVoltageTemp;
static double C_VoltageswellVoltageTemp;
double C_VoltageswellVoltageResult, C_VoltageswellLastVoltageResult;
int C_VoltageswellDurationTime, C_VoltageswellDurationLastTime;
static struct timeval C_VoltageswellDurationStartTime,C_VoltageswellDurationEndTime;
static double C_VoltageinterruptVoltageTemp;
double C_VoltageinterruptVoltageResult, C_VoltageinterruptLastVoltageResult;
int C_VoltageinterruptionDurationTime, C_VoltageinterruptionDurationLastTime;
static struct timeval C_VoltageinterruptionDurationStartTime,C_VoltageinterruptionDurationEndTime;

void C_voltagedipcalculation(void)  
{

	if (C_result_800half < C_VoltagedipVoltageTemp)
		C_VoltagedipVoltageTemp = C_result_800half;   

	C_VoltagedipDepth = C_VoltagedipVoltageTemp;
    gettimeofday (&C_VoltagedipDurationEndTime , &tz);
        A_VoltagedipDurationTime = (C_VoltagedipDurationEndTime.tv_sec-C_VoltagedipDurationStartTime.tv_sec)*1000+(C_VoltagedipDurationEndTime.tv_usec-C_VoltagedipDurationStartTime.tv_usec)/1000;
	if ((C_result_800half >= (VoltagedipThreshold + 0.02*DeclaredInputVoltageUdin)) || (C_result_800half < VoltageinterruptThreshold))      
	{
		C_VoltagedipLastVoltageResult = C_VoltagedipDepth;
		C_VoltagedipDurationLastTime = C_VoltagedipDurationTime;
		C_VoltagedipDurationTime = 0;
		C_voltagedipstartflag = 0;
		C_VoltagedipDepth = 0;
	}
}

void C_voltageswellcalculation(void)
{

	if (C_result_800half > C_VoltageswellVoltageTemp)
		C_VoltageswellVoltageTemp = C_result_800half;    

	C_VoltageswellVoltageResult = C_VoltageswellVoltageTemp;
    gettimeofday (&C_VoltageswellDurationEndTime , &tz);
         C_VoltageswellDurationTime = (C_VoltageswellDurationEndTime.tv_sec-C_VoltageswellDurationStartTime.tv_sec)*1000+(C_VoltageswellDurationEndTime.tv_usec-C_VoltageswellDurationStartTime.tv_usec)/1000;

	if ((C_result_800half <= (VoltageswellThreshold - 0.02*DeclaredInputVoltageUdin)))  
	{
		C_VoltageswellDurationLastTime = C_VoltageswellDurationTime;
		C_VoltageswellLastVoltageResult = C_VoltageswellVoltageResult;
		C_voltageswellstartflag = 0;
		C_VoltageswellDurationTime = 0;      
		C_VoltageswellVoltageResult = 0;
	}
}

void C_voltageinterruptioncalculation(void)
{

	if (C_result_800half < C_VoltageinterruptVoltageTemp)
		C_VoltageinterruptVoltageTemp = C_result_800half;

	C_VoltageinterruptVoltageResult = C_VoltageinterruptVoltageTemp;
    gettimeofday (&C_VoltageinterruptionDurationEndTime , &tz);
    C_VoltageinterruptionDurationTime = (C_VoltageinterruptionDurationEndTime.tv_sec-C_VoltageinterruptionDurationStartTime.tv_sec)*1000+(C_VoltageinterruptionDurationEndTime.tv_usec-C_VoltageinterruptionDurationStartTime.tv_usec)/1000;
	if ((C_result_800half >= (VoltageinterruptThreshold + 0.02*DeclaredInputVoltageUdin)))  
	{
		C_VoltageinterruptionDurationLastTime = C_VoltageinterruptionDurationTime;
		C_VoltageinterruptLastVoltageResult = C_VoltageinterruptVoltageResult;
		C_voltageinterruptstartflag = 0;
		C_VoltageinterruptionDurationTime = 0;
		C_VoltageinterruptVoltageResult = 0;
	}
}


char C_dip[100], C_swell[100], C_interrupt[100];
void C_voltagedipswellinterruptiondetection(void)
{
    struct tm *Diptime,*Swelltime, *Interrupttime;;
    time_t now_time;

	if (C_voltagedipstartflag == 0) 
	{
		if ((C_result_800half < VoltagedipThreshold) && (C_result_800half > VoltageinterruptThreshold))
		{
            time(&now_time);
            Diptime = localtime(&now_time);
            sprintf(A_dip, "%2d-%2d-%2d %2d:%2d:%2d\n", Diptime->tm_year + 1900, Diptime->tm_mon + 1, Diptime->tm_mday,
            Diptime->tm_hour, Diptime->tm_min, Diptime->tm_sec);

            gettimeofday (&C_VoltagedipDurationStartTime , &tz);
			C_voltagedipstartflag = 1;

			C_VoltagedipVoltageTemp = C_result_800half;
		}
	}
	if (C_voltageswellstartflag == 0)
	{

		if ((C_result_800half > VoltageswellThreshold))   
		{
            time(&now_time);
            Diptime = localtime(&now_time);
            sprintf(A_dip, "%2d-%2d-%2d %2d:%2d:%2d\n", Swelltime->tm_year + 1900, Swelltime->tm_mon + 1, Swelltime->tm_mday,
            Swelltime->tm_hour, Swelltime->tm_min, Swelltime->tm_sec);
            gettimeofday (&C_VoltageswellDurationStartTime , &tz);
			C_voltageswellstartflag = 1;

			C_VoltageswellVoltageTemp = C_result_800half;
		}
	}
	if (C_voltageinterruptstartflag == 0)
	{

		if ((C_result_800half < VoltageinterruptThreshold))   

		{
            time(&now_time);
            Diptime = localtime(&now_time);
            sprintf(A_dip, "%2d-%2d-%2d %2d:%2d:%2d\n", Interrupttime->tm_year + 1900, Interrupttime->tm_mon + 1, Interrupttime->tm_mday,
            Interrupttime->tm_hour, Interrupttime->tm_min, Interrupttime->tm_sec); 
             gettimeofday (&C_VoltageinterruptionDurationStartTime , &tz);
			C_voltageinterruptstartflag = 1;

			C_VoltageinterruptVoltageTemp = C_result_800half;
		}
	}
}

