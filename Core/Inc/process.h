#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct{
 uint8_t mode;
 bool activate_rstest;
 bool float_test;
 bool RISO_test;
 bool calibrateR;
 bool led_blink;
 bool continuously_on;
 bool stable_on;
 uint32_t saved_RSon; 
 uint32_t saved_HS;
 uint32_t saved_Dispertion;
 uint32_t max_Rs;
 uint32_t max_Dispersion;
 uint32_t max_Hs;
 uint32_t cnt1k;
}Tdev_cntrl;


void Algorithm();
void SetSignleTestMode();
void SetCycleTestMode();
void StartRSTest();
void StopRSTest();
void StartMagnitTest();
void StartRISOTest();
void StopRISOTest();
void SetOnContinuously();
void StartCalibration();
bool IsContinuouslyMode();
void SetMaxRs(uint32_t R);
void SetMaxDispersion(uint32_t R);
void SetMaxHs(uint32_t R);
#endif
