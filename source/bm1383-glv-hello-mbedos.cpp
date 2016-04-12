/*   Copyright 2016 Rohm Semiconductor

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "rohm-sensor-hal/rohm_hal.h"       //mbed.h, types, DEBUG_print*
#include "rohm-sensor-hal/I2CCommon.h"

#include "rohm-bm1383-glv/bm1383_driver.h"
#include "rohm-bm1383-glv/bm1383glv.h"


void printTreshodRegStatus(){
    if(bm1383_is_treshold_high_crossed()){
        printf("TresHighReg INT");
        }
    else{
        printf("TresHighReg NO INT");
        }
    printf("\r\n");
    if(bm1383_is_treshold_low_crossed()){
        printf("TresLowReg  INT");
        }
    else{
        printf("TresLowReg  NO INT");
        }
    printf("\r\n");
  }


void bm1383_init_and_setup(){
    printf("\n\rBM1383 library test program.\n\r");
    I2CCommonBegin();

    bm1383_wait_until_found();
    printf("\n\rSensor found.\n\r");
    bm1383_mode_poweroff2reset();
    wait(1);
    bm1383_mode_reset2standby();

    //setup bm1383 if needed
    bm1383_set_low_treshold(100);
    bm1383_enable_interrupt_latching();
    //bm1383_disable_interrupt_pullup();  //Pullup doesn't work on Japan-shield? Int line stays low(triggered).
    bm1383_enable_treshold_interrupts();
    bm1383_clear_interrupt();
}

void use_treshold_interrupts_to_find_treshold_matching_current_pressure(){
        float pressure;
        static uint16_t treshold = 0;

        //waitActiveLowInterrupt(INTPIN);

        pressure = bm1383_read_pressure();
        printf("Pressure: %f\n\r", pressure);

        printTreshodRegStatus();
        bm1383_clear_interrupt();
        printf("Clear int\n\r");
        printf("\n\r");
        //waitActiveLowInterruptClear(INTPIN);

        treshold = treshold + 1000;
        printf("Treshold: %d \n\r", treshold);
        printf("Start meas. - ");
        bm1383_set_high_treshold(treshold);
        bm1383_start_measurement_oneshot();
}


void app_start(int, char**){
    // set 115200 baud rate for stdout
    get_stdio_serial().baud(115200);
    printf("\n\rSerial initialized.\n\r");
    bm1383_init_and_setup();

    minar::Scheduler::postCallback(
        use_treshold_interrupts_to_find_treshold_matching_current_pressure
        ).period(minar::milliseconds(500));
}

