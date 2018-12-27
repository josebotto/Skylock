#include <xc.h>
#include "def_global_var.h"
#include "read_pulse.h"
#include "interrup.h"


void interrupt ISR(void) 
{
    //INTCON: INTERRUPT CONTROL REGISTER
    if(TMR0IF){                      
                                    // TMR0 temporizado a 10mseg. 
                                    // Utlizado para setear Flags
        TMR0L = 0x73;
        TMR0H = 0xFE;
        TMR0IF = 0;
        
        flag_timer_interrup = 0x01;     // Utilizada para temporizar en def_hardware.c y st_timers.c 
        ReadPulse_tick_ii0(0);          // Utilizada para temporizar las lecturas de Readers
    }

    /*
    if(RBIF){
    }
    */

    //INTCON3: INTERRUPT CONTROL REGISTER 3
 
    if(INT0IF || INT1IF || INT2IF || CCP2IF){
                                        // INT0 interrupcion LEC_A  W0/CLK
                                        // INT1 interrupcion LEC_A  W1/DAT
                                        // INT2 interrupcion LEC_B  W0/CLK
                                        // CCP2 interrupcion LEC_B  W1/DAT
  
        ReadPulse_isr(((~PORTB) & RDRS_MSK));

        INT0IF = 0;
        INT1IF = 0;
        INT2IF = 0;
        CCP2IF = 0;
    }
    
    //PIR1: PERIPHERAL INTERRUPT REQUEST (FLAG) REGISTER 1
    /*
    if(PSPIF){
    }

    if(ADIF){
    }

    if(RCIF){
    }

    if(TXIF){
    }

    if(SSPIF){
    }

    if(CCP1IF){
    }

    if(TMR2IF){
    }

    if(TMR1IF){
    }
    */

    //PIR2: PERIPHERAL INTERRUPT REQUEST (FLAG) REGISTER 2
    /*
    if(OSCFIF){
    }

    if(CMIF){
    }
    */
    
    if(EEIF){        // Data EEPROM/Flash Write Operation Interrupt Flag bit
                       // 1 = The write operation is complete (must be cleared by software)
        EEIF = 0;      // 0 = The write operation is not complete or has not been started
    }
    
    /*
    if(BCLIF){
    }

    if(HLVDIF){
    }

    if(TMR3IF){
    }

    if(CCP2IF){
    }
    */
}

void interrupt_enable(unsigned char mode) 
{
    switch(mode){
        case 0:
            GIE= OFF;    // Deshabilita Global Interrupt
            PEIE= OFF;   // Deshabilita Peripheral Interrupt
            break;

        case 1:
            GIE= ON;
            PEIE= OFF;
            break;

        case 2:
            GIE= ON;     //Habilita Global Interrupt
            PEIE= ON;    //Habilita Peripheral Interrupt
            break;
    }
}
