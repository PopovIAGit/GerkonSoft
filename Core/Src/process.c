#include "stdint.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "string.h"
#include "stdbool.h"
#include "stm32f7xx_hal_adc.h"
#include <math.h>
#include "analog.h"
#include "main.h"
#include <task.h>
#include "process.h"
#include "eeprom.h"


extern TIM_HandleTypeDef htim1;

enum{SIGNEMODE, CYCLEMODE};

 Tdev_cntrl dev_cntrl;

static devparam_t confparam;




void Algorithm(){
  static float button_rs = 3.3, button_hs = 3.3;
  static bool buttrs_state = false, butths_state = false;
  uint32_t timerLed1 = osKernelSysTick();
  uint32_t timerLed2 = osKernelSysTick();
  enum{WAIT, CHOOSING_GAIN, TEST, CONTINUOUSLY_ON, POPLOVOK_TEST} fsm = WAIT;
  const uint32_t UREF = 2500;
 
 static uint8_t Led1 = 0;
 static uint8_t Led2 = 2;
 static uint8_t Led3 = 0;
    
  SetSignalGain(GAIN_1);
  osDelay(100);
  dev_cntrl.activate_rstest = false;
  dev_cntrl.float_test = false;
  dev_cntrl.RISO_test = false;
  dev_cntrl.led_blink = false;
  dev_cntrl.calibrateR = false;
  dev_cntrl.continuously_on = false;
  dev_cntrl.stable_on = false;
  dev_cntrl.max_Rs = 300;
  dev_cntrl.max_Dispersion = 100;
  dev_cntrl.max_Hs = 1;

  DisableCoilPower();
  ModuleGetParam(&confparam);   

  SetCoilCurrent(confparam.testparam.Icoil);
  SetRSCurrent(confparam.testparam.Irs);
  SetCoilPWMFreq(confparam.testparam.Fcoil); 
  
  //dev_cntrl.activate_rstest = true;
  //dev_cntrl.mode = CYCLEMODE;
  //dev_cntrl.continuously_on = true;
  //HAL_TIM_Base_Start_IT(&htim1);
  
  for(;;){

     /*indication*/  
    
  /*  if(abs(timer - osKernelSysTick()) > 500){
       timer = osKernelSysTick();
       HAL_GPIO_TogglePin(WRKLED_R_GPIO_Port, WRKLED_R_Pin);
      // HAL_GPIO_TogglePin(WRKLED_G_GPIO_Port, WRKLED_G_Pin);                  
      }*/

   // левый(Работа)
    if (Led2 == 1)
    { 
      HAL_GPIO_WritePin(WRKLED_R_GPIO_Port, WRKLED_R_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(WRKLED_G_GPIO_Port, WRKLED_G_Pin, GPIO_PIN_RESET);
      
 /*     if (abs(timerLed2 - osKernelSysTick()) > 500)
      {
        timerLed2 = osKernelSysTick();
        Led2 = 2;
      }*/
      
    }
    else if (Led2 == 2)
    {
   //   HAL_GPIO_WritePin(WRKLED_R_GPIO_Port, WRKLED_R_Pin, GPIO_PIN_RESET);
   //   HAL_GPIO_WritePin(WRKLED_G_GPIO_Port, WRKLED_G_Pin, GPIO_PIN_SET);
      
      if (abs(timerLed2 - osKernelSysTick()) > 500)
      {
        timerLed2 = osKernelSysTick();
        HAL_GPIO_TogglePin(WRKLED_G_GPIO_Port, WRKLED_G_Pin);
       // Led2 = 1;
      }
    }
    else 
    {
      HAL_GPIO_WritePin(WRKLED_R_GPIO_Port, WRKLED_R_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(WRKLED_G_GPIO_Port, WRKLED_G_Pin, GPIO_PIN_RESET);
    }
    
    //средний (ТЕСТ)
    if (Led1 == 1)
    {
      HAL_GPIO_WritePin(TESTLED_R_GPIO_Port, TESTLED_R_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(TESTLED_G_GPIO_Port, TESTLED_G_Pin, GPIO_PIN_RESET);
      if (abs(timerLed1 - osKernelSysTick()) > 5000)
      {
        timerLed1 = osKernelSysTick();
        Led1 = 0;
      }
    }
    else if (Led1 == 2)
    {
       HAL_GPIO_WritePin(TESTLED_R_GPIO_Port, TESTLED_R_Pin, GPIO_PIN_RESET);
       HAL_GPIO_WritePin(TESTLED_G_GPIO_Port, TESTLED_G_Pin, GPIO_PIN_SET);
     if (abs(timerLed1 - osKernelSysTick()) > 5000)
      {
        timerLed1 = osKernelSysTick();
        Led1 = 0;
      }
    }
    else 
    {
      HAL_GPIO_WritePin(TESTLED_R_GPIO_Port, TESTLED_R_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(TESTLED_G_GPIO_Port, TESTLED_G_Pin, GPIO_PIN_RESET);
    }

    // правый (ГЕРКОН)
    
    if (dev_cntrl.activate_rstest || dev_cntrl.continuously_on) Led3 = 1;
    else Led3 = 0;  
    
    if(Led3)
    {
        HAL_GPIO_WritePin(RSLED_GPIO_Port, RSLED_Pin, GPIO_PIN_SET);
    }
    else 
    {
        HAL_GPIO_WritePin(RSLED_GPIO_Port, RSLED_Pin, GPIO_PIN_RESET);
    }
    
    if(HAL_GPIO_ReadPin(FLOAT_TEST_GPIO_Port, FLOAT_TEST_Pin) == GPIO_PIN_SET) button_rs = button_rs+ (3.3 - button_rs) / 10.0; 
    else button_rs = button_rs + (0 - button_rs) / 10.0;  
    if(button_rs > 2.8) buttrs_state = false;
    else if(button_rs < 0.4) buttrs_state = true;    
    
    if(HAL_GPIO_ReadPin(RS_TEST_GPIO_Port , RS_TEST_Pin ) == GPIO_PIN_SET) button_hs = button_hs+ (3.3 - button_hs) / 10.0; 
    else button_hs = button_hs + (0 - button_hs) / 10.0;  
    if(button_hs > 2.8)
    {
      butths_state = false;
    }
    else if(button_hs < 0.4) 
    {
      butths_state = true; 
    }
    
    if(buttrs_state) dev_cntrl.activate_rstest = true;
    if(butths_state) dev_cntrl.float_test = true;
    
    switch(fsm){
       case WAIT : {
            dev_cntrl.cnt1k = 0;
            DisableCoilPower();     
           // dev_cntrl.mode = SIGNEMODE;
         if(dev_cntrl.activate_rstest || dev_cntrl.continuously_on){
           EnableCoilPower();
           SetRSCurrent(confparam.testparam.Irs);
           osDelay(100);
           fsm = CHOOSING_GAIN;
           ClearDSPResult();
          }
         if(dev_cntrl.calibrateR){
           SetCoilPWMFreq(100); 
           dev_cntrl.stable_on = true;
           SetResistance_offset(0);
           osDelay(50);
           dev_cntrl.calibrateR = false;
           ModuleGetParam(&confparam);
           SetRSCurrent(50);
           SetSignalGain(GAIN_1);
           osDelay(50);
           confparam.calibrate.R_offset_GAIN_1 = GetRS_ON();
           SetSignalGain(GAIN_2);
           osDelay(50);
           confparam.calibrate.R_offset_GAIN_2 = GetRS_ON();
           SetSignalGain(GAIN_5);
           osDelay(50);
           confparam.calibrate.R_offset_GAIN_5 = GetRS_ON();
           SetSignalGain(GAIN_10);
           osDelay(50);
           confparam.calibrate.R_offset_GAIN_10 = GetRS_ON();
           SetSignalGain(GAIN_20);
           osDelay(50);
           confparam.calibrate.R_offset_GAIN_20 = GetRS_ON();
           SetSignalGain(GAIN_50);
           osDelay(50);
           confparam.calibrate.R_offset_GAIN_50 = GetRS_ON();
           SetSignalGain(GAIN_100);
           osDelay(50);
           confparam.calibrate.R_offset_GAIN_100 = GetRS_ON();
           ModuleSetParam(&confparam); 
           SetRSCurrent(confparam.testparam.Irs);
           dev_cntrl.stable_on = false;
          } 
         if(dev_cntrl.float_test){
           SetCoilPWMFreq(100); 
           osDelay(100);   
           dev_cntrl.saved_HS = GetB_HS();
           if (dev_cntrl.saved_HS>dev_cntrl.max_Hs) Led1 = 1;
           else Led1 = 2;
           dev_cntrl.float_test = false;
           button_hs = 3300; /*clear avg filter for button*/
          }         
         break;
        }
       case CHOOSING_GAIN :{
         SetCoilPWMFreq(100); 
         dev_cntrl.stable_on = true;
         osDelay(50);  
         SetSignalGain(GAIN_1);
         osDelay(50);
         if(GetURSon() > (UREF * 90 / 100)){ /*if more than 90%*/
           dev_cntrl.stable_on = false;
           if(dev_cntrl.continuously_on) fsm = CONTINUOUSLY_ON;
           if(dev_cntrl.activate_rstest) fsm = TEST;
           ClearDSPResult();   
           SetSignalGain(GAIN_1);
           SetResistance_offset(confparam.calibrate.R_offset_GAIN_1);
           osDelay(50);
           break;
          }
         SetSignalGain(GAIN_2);
         osDelay(50);
         if(GetURSon() > (UREF * 90 / 100)){ /*if more than 90%*/
           dev_cntrl.stable_on = false;
           if(dev_cntrl.continuously_on) fsm = CONTINUOUSLY_ON;
           if(dev_cntrl.activate_rstest) fsm = TEST; 
           ClearDSPResult();                      
           SetSignalGain(GAIN_1);
           SetResistance_offset(confparam.calibrate.R_offset_GAIN_1);           
           osDelay(50);
           break;
          }
         SetSignalGain(GAIN_5);
         osDelay(50);
         if(GetURSon() > (UREF * 90 / 100)){ /*if more than 90%*/
           dev_cntrl.stable_on = false;           
           if(dev_cntrl.continuously_on) fsm = CONTINUOUSLY_ON;
           if(dev_cntrl.activate_rstest) fsm = TEST;  
           ClearDSPResult();           
           SetSignalGain(GAIN_2);
           SetResistance_offset(confparam.calibrate.R_offset_GAIN_2);           
           osDelay(50);
           break;
          }
         SetSignalGain(GAIN_10);
         osDelay(50);
         if(GetURSon() > (UREF * 90 / 100)){ /*if more than 90%*/
           dev_cntrl.stable_on = false;           
           if(dev_cntrl.continuously_on) fsm = CONTINUOUSLY_ON;
           if(dev_cntrl.activate_rstest) fsm = TEST;    
           ClearDSPResult();                      
           SetSignalGain(GAIN_5);
           SetResistance_offset(confparam.calibrate.R_offset_GAIN_5);                     
           osDelay(50);
           break;
          }
         SetSignalGain(GAIN_20);
         osDelay(50);
         if(GetURSon() > (UREF * 90 / 100)){ /*if more than 90%*/
           dev_cntrl.stable_on = false;
           if(dev_cntrl.continuously_on) fsm = CONTINUOUSLY_ON;
           if(dev_cntrl.activate_rstest) fsm = TEST; 
           ClearDSPResult();                      
           SetSignalGain(GAIN_10);
           SetResistance_offset(confparam.calibrate.R_offset_GAIN_10);                     
           osDelay(50);
           break;
          }
         SetSignalGain(GAIN_50);
         osDelay(50);
         if(GetURSon() > (UREF * 90 / 100)){ /*if more than 90%*/
           dev_cntrl.stable_on = false;
           if(dev_cntrl.continuously_on) fsm = CONTINUOUSLY_ON;
           if(dev_cntrl.activate_rstest) fsm = TEST;
           ClearDSPResult();           
           SetSignalGain(GAIN_20);
           SetResistance_offset(confparam.calibrate.R_offset_GAIN_20);                     
           osDelay(50);
           break;
          }
         SetSignalGain(GAIN_100);
         osDelay(50);
         if(GetURSon() > (UREF * 90 / 100)){ /*if more than 90%*/
           dev_cntrl.stable_on = false;
           if(dev_cntrl.continuously_on) fsm = CONTINUOUSLY_ON;
           if(dev_cntrl.activate_rstest) fsm = TEST;   
           ClearDSPResult();                      
           SetSignalGain(GAIN_50);
           SetResistance_offset(confparam.calibrate.R_offset_GAIN_50);                     
           osDelay(50);
           break;
          }
         dev_cntrl.stable_on = false;
         SetResistance_offset(confparam.calibrate.R_offset_GAIN_100);                   
         SetSignalGain(GAIN_100);         
         if(dev_cntrl.continuously_on) fsm = CONTINUOUSLY_ON;
         if(dev_cntrl.activate_rstest) fsm = TEST;  
         ClearDSPResult();
         osDelay(50); 
         break;
        }
       case CONTINUOUSLY_ON : {
         ModuleGetParam(&confparam);
         SetCoilPWMFreq(confparam.testparam.Fcoil);
         dev_cntrl.saved_RSon = GetRS_ON();
         if(!dev_cntrl.continuously_on) fsm = WAIT;
         break;
        }        
       case TEST : {
         ModuleGetParam(&confparam);         
         SetCoilPWMFreq(confparam.testparam.Fcoil);         
         if(dev_cntrl.mode == SIGNEMODE){   
           osDelay(1000);  
           if (dev_cntrl.cnt1k >= 100){
           dev_cntrl.saved_Dispertion = GetDispertion();
            if(dev_cntrl.saved_RSon <= dev_cntrl.max_Rs && dev_cntrl.saved_Dispertion < dev_cntrl.max_Dispersion)  
            {
              
              timerLed1 = osKernelSysTick();
              Led1 = 2;
            }              
            else   
            {
            
               timerLed1 = osKernelSysTick();
               Led1 = 1;
            }
              
           fsm = WAIT;
           dev_cntrl.activate_rstest = false;
           button_rs = 3300; /*clear avg filter for button*/
           }
          }
         else if(!dev_cntrl.activate_rstest) fsm = WAIT;         
         dev_cntrl.saved_RSon = GetRS_ON();   

         break;
        }        
      case POPLOVOK_TEST:
        {
         ModuleGetParam(&confparam);
         SetCoilPWMFreq(confparam.testparam.Fcoil);
         dev_cntrl.saved_RSon = GetRS_ON();
         if(!dev_cntrl.continuously_on) fsm = WAIT;
         break;
        }
      }
    osThreadYield();  
   }
}


void SetMaxRs(uint32_t R){
 dev_cntrl.max_Rs = R;
}

void SetMaxDispersion(uint32_t R){
 dev_cntrl.max_Dispersion = R;
}

void SetMaxHs(uint32_t R){
 dev_cntrl.max_Hs = R;
}

void StartMagnitTest(){
 dev_cntrl.float_test = true;
}

void SetSignleTestMode(){
 dev_cntrl.mode = SIGNEMODE;  
}

void SetCycleTestMode(){
 dev_cntrl.mode = CYCLEMODE;   
}

void StartRSTest(){
 dev_cntrl.activate_rstest = true; 
}

void StopRSTest(){
 dev_cntrl.activate_rstest = false; 
 dev_cntrl.continuously_on = false; 
}

void StartRISOTest(){
 dev_cntrl.RISO_test = true;
}

void StopRISOTest(){
 dev_cntrl.RISO_test = false;
}

void SetOnContinuously(){
 dev_cntrl.continuously_on = true; 
}

void StartCalibration(){
 dev_cntrl.calibrateR = true;
}

bool IsContinuouslyMode(){
  return (dev_cntrl.continuously_on || dev_cntrl.stable_on); 
}

uint32_t GetLastRSon(){
 return dev_cntrl.saved_RSon; 
}