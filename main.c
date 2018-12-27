/*
 * =====================================================================
 * Proyecto Skylock
 * Software version 1.0
 * 
 * Compatible con:
 *                -Placa PCL version 2.0 y 3.0
 *                -Placa PCC version 2.0 y 3.0
 *
 * File:   main.c
 * Author: JBotto
 * 
 * Digicard Sistemas
 * Created on 24 de agosto de 2017, 09:43
 * 
 * Log:
 *     Main body.        
 *     Microcontrolador utilizado PIC18F4520
 * 
 * IMPORTANTE !!! 
 *    - El proyecto MX900 desde el 02/08/17 se paso a llamar SkyLock
 *    - Esta version de firmware es compatible con Placa PCL version 2.0 y
 *      version 3.0
 *    - En compile_code.h se puede seleccionar para cual de las dos placas 
 *      se desea compilar
 *    - Realizado con MPLAB X IDE v3.30    XC8 v1.37
 * 
 * =====================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include "compile_code.h"

// #pragma Las declaraciones de configuración deben preceder a los archivos de proyecto

// CONFIG1H
#pragma config OSC = HSPLL  // Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF  // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF   // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
//#pragma config WDT = OFF    // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDT = ON     // Watchdog Timer Enable bit (WDT enabled)
//#pragma config WDTPS = 16   // Watchdog Timer Postscale Select bits (1:16). Ocurre cada 0.064 seg
//#pragma config WDTPS = 32   // Watchdog Timer Postscale Select bits (1:32). Ocurre cada 0.128 seg
#pragma config WDTPS = 256  // Watchdog Timer Postscale Select bits (1:256). Ocurre cada 1.024 seg
//#pragma config WDTPS = 512  // Watchdog Timer Postscale Select bits (1:512). Ocurre cada 2.048 seg

// CONFIG3H
//#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config CCP2MX = PORTBE  // CCP2 MUX bit (CCP2 input/output is multiplexed with RB3)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)

#ifdef COMPILAR_PLACA_PCC
    #pragma config MCLRE = ON   // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
#else
    #pragma config MCLRE = OFF  // MCLR Pin Enable bit (MCLR pin disabled; RE3 input pin enable)
#endif

// CONFIG4L
#pragma config STVREN = ON  // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF    // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF  // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = ON     // Code Protection bit (Block 0 (000800-001FFFh) code-protected)
#pragma config CP1 = ON     // Code Protection bit (Block 1 (002000-003FFFh) code-protected)
#pragma config CP2 = ON     // Code Protection bit (Block 2 (004000-005FFFh) code-protected)
#pragma config CP3 = ON     // Code Protection bit (Block 3 (006000-007FFFh) code-protected)

// CONFIG5H
#pragma config CPB = OFF    // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = ON     // Data EEPROM Code Protection bit (Data EEPROM code-protected)

// CONFIG6L
#pragma config WRT0 = OFF   // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF   // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF   // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF   // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF   // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF   // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF   // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF  // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF  // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF  // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF  // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF  // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

//__IDLOC
#pragma config IDLOC7 = 0, IDLOC6 = 0, IDLOC5 = 0, IDLOC4 = 0, IDLOC3 = 0, IDLOC2 = 0, IDLOC1 = 4, IDLOC0 = 3       // IDLOC = 00000043

#include "def_hardware.h"
#include "def_global_var.h"
#include "interrup.h"
#include "cnt_power.h"
#include "state_machine.h"

void main(void) 
{
    init_hard();            // Inicializa. Define Pin-out y registros. PCL version 2.0        
    interrupt_enable(0);    // Deshabilita interrupciones
    cnt_power_reset();      // Actualiza contadores de Power-on y Reset 
    stm_init_fifo();        // Inicializar buffer de State_Machine
    interrupt_enable(2);    // Habilita interrupciones
    init_hard_delay();      // Genera retardo de tiempo al iniciar despues de un power-on o reset de la placa    

    
    while (1) {               // Loop principal
        CLRWDT();             // Refresh WDT
        state_machine();      // State Machine (Engine)      
    }
}
