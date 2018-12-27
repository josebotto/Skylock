/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   def_hardware.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 24 de agosto de 2017, 10:49
 * 
 * Log:
 *     Define Pin-out and Registers.  
 *  
 * =====================================================================
 */


#ifndef DEF_HARDWARE_H
#define	DEF_HARDWARE_H

    #include "compile_code.h"

    #define RA_I    PORTAbits.RA0  
    #define RB_I    PORTAbits.RA1
    #define IN_AB   PORTAbits.RA2
    #define IN_AA   PORTAbits.RA3
    #define IN_BA   PORTAbits.RA4
    #define IN_BB   PORTAbits.RA5

    #define LEC_A_D0   PORTBbits.RB0
    #define LEC_A_D1   PORTBbits.RB1
    #define LEC_B_D0   PORTBbits.RB2
    #define LEC_B_D1   PORTBbits.RB3
    #define IN_AUX1    PORTBbits.RB4
    #define ALARM      PORTBbits.RB5
    #define IN_AUX2    PORTBbits.RB6
    #define TAMPER     PORTBbits.RB7

    #define LED_B1    PORTCbits.RC0
    #define LED_B2    PORTCbits.RC1
    #define ACT_LEC   PORTCbits.RC2
    #define I2C_SCI   PORTCbits.RC3
    #define I2C_SDA   PORTCbits.RC4

    #ifdef COMPILAR_PLACA_PCC
        #define SW_CONF   PORTCbits.RC5
    #else
        #define RS485_DE_RE   PORTCbits.RC5
    #endif

    #define RS485_TX   PORTCbits.RC6
    #define RS485_RX   PORTCbits.RC7

    #define LED_0    PORTDbits.RD0
    #define LED_1    PORTDbits.RD1
    #define LED_2    PORTDbits.RD2
    #define LED_3    PORTDbits.RD3
    #define LED_4    PORTDbits.RD4
    #define LED_5    PORTDbits.RD5
    #define LED_A1   PORTDbits.RD6
    #define LED_A2   PORTDbits.RD7

    #define RELE_A   PORTEbits.RE0
    #define RELE_B   PORTEbits.RE1
    #define RELE_C   PORTEbits.RE2

    #ifndef COMPILAR_PLACA_PCC
        #define SW_CONF   PORTEbits.RE3
    #endif

    // Frecuencia del cristal 
    #define _XTAL_FREQ   39321600   // ((XTAL = 9830400 Hz)*4 = 39321600 Hz)
                                    // La frecuencia del cristal se multiplica * 4 debido a que se utilizo
                                    // en la definicion de Bits del micro el HSPLL 

    void init_hard(void);           // Inicializa el hardware
    void init_hard_delay(void);     // Genera un retardo al inicio en cada power-on o reset

#endif	/* DEF_HARDWARE_H */
