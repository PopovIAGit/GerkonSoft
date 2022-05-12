#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_DOTS 37  
#define KTY83_MIN_RESIST	730	// Нижн. предел сопр. датчика. Если сопр. ниже этого зн-я, считаем что КЗ
#define KTY83_MAX_RESIST	936	// Верхн. предел сопр. датчика. Если сопр. выше этого зн-я, считаем что обрыв
//--------Структура "точка"--------------------------------------------
typedef struct {
	uint16_t				temper;		// Температура
	uint16_t				adc;		// Величина АЦП (сопротивление датчика температуры)
} TDot;

//---------------------------------------------------------------------
typedef struct {
	TDot					dots[NUM_DOTS];	// Массив точек для интерполяции
	uint16_t						inputR;		// Вход: АЦП (сопротивление датчика температуры)
	uint16_t						maxResist;	// Параметр: максимальное сопротивление датчика
	uint16_t						outputT;	// Выход: температура
	bool					fault;		// Флаг сбоя датчика
} TTempObserver;



typedef struct{
 uint8_t mode;
 bool activate_rstest;
 bool float_test;
 bool RISO_test;
 bool calibrateR;
 bool calibrateHS;
 bool led_blink;
 bool continuously_on;
 bool stable_on;
 bool findMinR;
 bool findR;
 uint32_t suzType;
 uint32_t saved_RSon; 
 uint32_t saved_HS;
 uint32_t saved_Dispertion;
 uint32_t max_Rs;
 uint32_t max_Dispersion;
 uint32_t max_Hs;
 uint32_t cnt1k;
 uint32_t saved_Riso; 
 TTempObserver TempObserver;
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
void SetSuzType(uint32_t suz);
uint32_t GetSuzType();

void TempObserverInit(TTempObserver *);
void TempObserverUpdate (TTempObserver *);
#endif
