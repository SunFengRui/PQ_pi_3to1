#pragma once

#define DeclaredInputVoltageUdin     1.41421356  
#define VoltagedipThreshold (0.9*DeclaredInputVoltageUdin)
#define VoltageswellThreshold (1.1*DeclaredInputVoltageUdin)
#define VoltageinterruptThreshold (0.1*DeclaredInputVoltageUdin)





extern double A_VoltagedipDepth, A_VoltagedipLastVoltageResult;
extern int A_VoltagedipDurationTime, A_VoltagedipDurationLastTime;
extern double A_VoltageswellVoltageResult,A_VoltageswellLastVoltageResult;
extern int A_VoltageswellDurationTime, A_VoltageswellDurationLastTime;
extern double A_VoltageinterruptVoltageResult, A_VoltageinterruptLastVoltageResult;
extern int A_VoltageinterruptionDurationTime, A_VoltageinterruptionDurationLastTime;
extern char A_dip[100], A_swell[100], A_interrupt[100];
extern double B_VoltagedipDepth, B_VoltagedipLastVoltageResult;
extern int B_VoltagedipDurationTime, B_VoltagedipDurationLastTime;
extern double B_VoltageswellVoltageResult, B_VoltageswellLastVoltageResult;
extern int B_VoltageswellDurationTime, B_VoltageswellDurationLastTime;
extern double B_VoltageinterruptVoltageResult, B_VoltageinterruptLastVoltageResult;
extern int B_VoltageinterruptionDurationTime, B_VoltageinterruptionDurationLastTime;
extern char B_dip[100], B_swell[100], B_interrupt[100];
extern double C_VoltagedipDepth, C_VoltagedipLastVoltageResult;
extern int C_VoltagedipDurationTime, C_VoltagedipDurationLastTime;
extern double C_VoltageswellVoltageResult, C_VoltageswellLastVoltageResult;
extern int C_VoltageswellDurationTime, C_VoltageswellDurationLastTime;
extern double C_VoltageinterruptVoltageResult, C_VoltageinterruptLastVoltageResult;
extern int C_VoltageinterruptionDurationTime, C_VoltageinterruptionDurationLastTime;
extern char C_dip[100], C_swell[100], C_interrupt[100];

void A_voltagedipcalculation(void);
void A_voltageswellcalculation(void);
void A_voltageinterruptioncalculation(void);
void A_voltagedipswellinterruptiondetection(void);

void B_voltagedipcalculation(void);
void B_voltageswellcalculation(void);
void B_voltageinterruptioncalculation(void);
void B_voltagedipswellinterruptiondetection(void);

void C_voltagedipcalculation(void);
void C_voltageswellcalculation(void);
void C_voltageinterruptioncalculation(void);
void C_voltagedipswellinterruptiondetection(void);
