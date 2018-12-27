#include <xc.h>
#include "def_hardware.h"
#include "def_global_var.h"


void init_hard(void)    // Inicializa el hardware 
{
    TRISA = 0x3F;
    /*
    ------
    ------
    TRISA5 = IN;
    TRISA4 = IN;
    TRISA3 = IN;
    TRISA2 = IN;
    TRISA1 = IN;
    TRISA0 = IN;
    */

    TRISB = 0xDF;
    /*
    TRISB7 = IN; 
    TRISB6 = IN; 
    TRISB5 = OUT;
    TRISB4 = IN;
    TRISB3 = IN;
    TRISB2 = IN;
    TRISB1 = IN;
    TRISB0 = IN;
    */
    
    #ifdef COMPILAR_PLACA_PCC
        TRISC = 0xA0;
        /*
        TRISC7 = IN;
        TRISC6 = OUT;
        TRISC5 = IN;
        TRISC4 = OUT;    
        TRISC3 = OUT;
        TRISC2 = OUT;
        TRISC1 = OUT;
        TRISC0 = OUT;
        */
    #else
        TRISC = 0x80;
        /*
        TRISC7 = IN;
        TRISC6 = OUT;
        TRISC5 = OUT;
        TRISC4 = OUT;    
        TRISC3 = OUT;
        TRISC2 = OUT;
        TRISC1 = OUT;
        TRISC0 = OUT;
        */
    #endif

    TRISD = 0x00;
    /*
    TRISD7 = OUT;
    TRISD6 = OUT;
    TRISD5 = OUT;
    TRISD4 = OUT;
    TRISD3 = OUT;
    TRISD2 = OUT;
    TRISD1 = OUT;
    TRISD0 = OUT;
    */

    TRISE = 0x08;
    /*
    ------
    ------
    ------
    ------
    ------
    TRISE3 = IN;
    TRISE2 = OUT;
    TRISE1 = OUT;
    TRISE0 = OUT;
    */

    //PORTA

    //PORTB
    ALARM = OFF;  //RB5

    //PORTC
    LED_B1 = OFF;       //RC0
    LED_B2 = OFF;       //RC1
    ACT_LEC = OFF;      //RC2
    I2C_SCI = ON;       //RC3
    I2C_SDA = ON;       //RC4

    #ifndef COMPILAR_PLACA_PCC
        RS485_DE_RE = OFF;  //RC5
    #endif

    RS485_TX = OFF;     //RC6

    //PORTD
    LED_0 = ON;    //RD0
    LED_1 = ON;    //RD1
    LED_2 = ON;    //RD2
    LED_3 = OFF;    //RD3   Es para que se inicialice encendido        
    LED_4 = ON;    //RD4
    LED_5 = ON;    //RD5
    LED_A1 = OFF;  //RD6
    LED_A2 = OFF;  //RD7
    
    //PORTE
    RELE_A = OFF;  //RE0
    RELE_B = OFF;  //RE1
    RELE_C = OFF;  //RE2
    
    ADCON0 = 0x00; //A-D Control Register 0   
    ADCON1 = 0x0F; //A-D Control Register 1
    ADCON2 = 0x80; //A-D Control Register 2

    EECON1 = 0x00; //EEPROM CONTROL REGISTER 1

    INTCON = 0x20;  //INTERRUPT CONTROL REGISTER
    /*
    GIE    = OFF;  //INTCON.7
    PEIE   = OFF;  //INTCON.6
    TMR0IE =  ON;  //INTCON.5 Habilita Timer0 desde el arranque
    INT0IE = OFF;  //INTCON.4
    RBIE   = OFF;  //INTCON.3
    TMR0IF = OFF;  //INTCON.2
    INT0IF = OFF;  //INTCON.1
    RBIF   = OFF;  //INTCON.0
    */
 
    INTCON2 = 0x80;  //INTERRUPT CONTROL REGISTER 2  
    /*
    RBPU      = ON;   //INTCON2.7  Deshabilita las R de Pull ups
    INTEDG0   = OFF;  //INTCON2.6
    INTEDG1   = OFF;  //INTCON2.5
    INTEDG2   = OFF;  //INTCON2.4
    ------            //INTCON2.3
    TMR0IP    = OFF;  //INTCON2.2
    ------            //INTCON2.1
    RBIP      = OFF;  //INTCON2.0
    */

    INTCON3 = 0x00;  //INTERRUPT CONTROL REGISTER 3
    /*
    INT2IP = OFF;  //INTCON3.7
    INT1IP = OFF;  //INTCON3.6
    ------         //INTCON3.5
    INT2IE = OFF;  //INTCON3.4
    INT1IE = OFF;  //INTCON3.3
    ------         //INTCON3.2
    INT2IF = OFF;  //INTCON3.1
    INT1IF = OFF;  //INTCON3.0
    */
 
    PIE1 = 0x00;  //PERIPHERAL INTERRUPT ENABLE REGISTER 1    
    /*
    PSPIE  = OFF;  //PIE1.7
    ADIE   = OFF;  //PIE1.6
    RCIE   = OFF;  //PIE1.5  Deshabilita interupcion de EUSART
    TXIE   = OFF;  //PIE1.4  Deshabilita interupcion de EUSART
    SSPIE  = OFF;  //PIE1.3
    CCP1IE = OFF;  //PIE1.2
    TMR2IE = OFF;  //PIE1.1
    TMR1IE = OFF;  //PIE1.0
    */

    PIE2 = 0x00;  //PERIPHERAL INTERRUPT ENABLE REGISTER 1
    /*
    OSCFIE = OFF;  //PIE2.7
    CMIE   = OFF;  //PIE2.6
    ------         //PIE2.5
    EEIE   = OFF;  //PIE2.4
    BCLIE  = OFF;  //PIE2.3
    HLVDIE = OFF;  //PIE2.2
    TMR3IE = OFF;  //PIE2.1
    CCP2IE = OFF;  //PIE2.0
    */
 
    IPR1 = 0x00;  //PERIPHERAL INTERRUPT PRIORITY REGISTER 1
    /*
    PSPIP  = OFF;  //IPR1.7
    ADIP   = OFF;  //IPR1.6
    RCIP   = OFF;  //IPR1.5
    TXIP   = OFF;  //IPR1.4
    SSPIP  = OFF;  //IPR1.3
    CCP1IP = OFF;  //IPR1.2
    TMR2IP = OFF;  //IPR1.1
    TMR1IP = OFF;  //IPR1.0
    */

    IPR2 = 0xFF;  //PERIPHERAL INTERRUPT PRIORITY REGISTER 2
    /*
    OSCFIP        = OFF;  //IPR2.7
    CMIP          = OFF;  //IPR2.6
    Unimplemented = OFF;  //IPR2.5
    EEIP          = OFF;  //IPR2.4
    BCLIP         = OFF;  //IPR2.3
    HLVDIP        = OFF;  //IPR2.2
    TMR3IP        = OFF;  //IPR2.1
    CCP2IP        = OFF;  //IPR2.0
    */

    PIR1 = 0x00;  //PERIPHERAL INTERRUPT REQUEST (FLAG) REGISTER 1
    /*
    PSPIF  = OFF;  //PIR1.7
    ADIF   = OFF;  //PIR1.6
    RCIF   = OFF;  //PIR1.5
    TXIF   = OFF;  //PIR1.4
    SSPIF  = OFF;  //PIR1.3
    CCP1IF = OFF;  //PIR1.2
    TMR2IF = OFF;  //PIR1.1
    TMR1IF = OFF;  //PIR1.0
    */
         
    PIR2 = 0x00;  //PERIPHERAL INTERRUPT REQUEST (FLAG) REGISTER 2
    /*
    OSCFIF        = OFF;  //PIR2.7
    CMIF          = OFF;  //PIR2.6
    Unimplemented = OFF;  //PIR2.5
    EEIF          = OFF;  //PIR2.4
    BCLIF         = OFF;  //PIR2.3
    HLVDIF        = OFF;  //PIR2.2
    TMR3IF        = OFF;  //PIR2.1
    CCP2IF        = OFF;  //PIR2.0
    */

    //TIMER 0
    TMR0L = 0x73;    //Carga resultado obtenido de la formula 
    TMR0H = 0xFE;    //Carga resultado obtenido de la formula

    T0CON = 0x87;    //TIMER0 CONTROL REGISTER
                     //Formula para calculo de temporizacion TMR0
                     //(TMRH,TMROL)=65535-Tretardo*(Fosc/(4*prescaler))-1
    /*T0CON          // TIMER0 CONTROL REGISTER
    TMR0ON =  ON;    //T0CON.7 Habilita TMR0
    T08BIT = OFF;    //T0CON.6 Utiliza registro TMR0 de 16 bits
    T0CS   = OFF;    //T0CON.5 Se confihura como Timer
    T0SE   = OFF;    //T0CON.4 Actua sobre flanco positivo
    PSA    = OFF;    //T0CON.3 Habilita Prescaler
    T0PS2  =  ON;    //T0CON.2 Timer0 Prescaler en 1:256
    T0PS1  =  ON;    //T0CON.1
    T0PS0  =  ON;    //T0CON.0
    */

    //TIMER1
    TMR1L = 0;
    TMR1H = 0;

    T1CON = 0x00;   //TIMER1 CONTROL REGISTER
    /*
    RD16     = OFF;  //T1CON.7
    T1RUN    = OFF;  //T1CON.6
    T1CKPS1  = OFF;  //T1CON.5
    T1CKPS0  = OFF;  //T1CON.4
    T1OSCEN  = OFF;  //T1CON.3
    T1SYNC   = OFF;  //T1CON.2
    TMR1CS   = OFF;  //T1CON.1
    TMR1ON   = OFF;  //T1CON.0
    */

    //TIMER2
    TMR2 = 0;

    T2CON = 0x00;    // TIMER2 CONTROL REGISTER
    /*
    ------             //T2CON.7
    T2OUTPS3   = OFF;  //T2CON.6
    T2OUTPS2   = OFF;  //T2CON.5
    T2OUTPS1   = OFF;  //T2CON.4
    T2OUTPS0   = OFF;  //T2CON.3
    TMR2ON     = OFF;  //T2CON.2
    T2CKPS1    = OFF;  //T2CON.1
    T2CKPS0    = OFF;  //T2CON.0
    */

    //TIMER3
    TMR3L = 0;
    TMR3H = 0;

    T3CON = 0x00;   // TIMER3 CONTROL REGISTER
    /*
    RD16      = OFF;  //T3CON.7
    T3CCP2    = OFF;  //T3CON.6
    T3CKPS1   = OFF;  //T3CON.5
    T3CKPS0   = OFF;  //T3CON.4
    T3CCP1    = OFF;  //T3CON.3
    T3SYNC    = OFF;  //T3CON.2
    TMR3CS    = OFF;  //T3CON.1
    TMR3ON    = OFF;  //T3CON.0
    */

    CCP1CON = 0x00; //  ECCP CONTROL REGISTER (40/44-PIN DEVICES)
    /*
    P1M1     = OFF;  //CCP1CON.7  Enhanced PWM Output Configuration bits
    P1M0     = OFF;  //CCP1CON.6  Enhanced PWM Output Configuration bits
    DC1B1    = OFF;  //CCP1CON.5  PWM Duty Cycle bit 1 and bit 0
    DC1B0    = OFF;  //CCP1CON.4  PWM Duty Cycle bit 1 and bit 0
    CCP1M3   = OFF;  //CCP1CON.3  Enhanced CCP Mode Select bits
    CCP1M2   = OFF;  //CCP1CON.2  Enhanced CCP Mode Select bits
    CCP1M1   = OFF;  //CCP1CON.1  Enhanced CCP Mode Select bits
    CCP1M0   = OFF;  //CCP1CON.0  Enhanced CCP Mode Select bits
     */

    CCP2CON = 0x00; //  ECCP CONTROL REGISTER (40/44-PIN DEVICES)
    /*
    ------           //CCP1CON.7
    ------           //CCP1CON.6
    DC2B1    = OFF;  //CCP2CON.5  PWM Duty Cycle bit 1 and bit 0
    DC2B0    = OFF;  //CCP2CON.4  PWM Duty Cycle bit 1 and bit 0
    CCP2M3   = OFF;  //CCP2CON.3  Enhanced CCP Mode Select bits
    CCP2M2   = OFF;  //CCP2CON.2  Enhanced CCP Mode Select bits
    CCP2M1   = OFF;  //CCP2CON.1  Enhanced CCP Mode Select bits
    CCP2M0   = OFF;  //CCP2CON.0  Enhanced CCP Mode Select bits
     */
    
    return;
}

void init_hard_delay(void)  // Genera retardo de tiempo despues de cada power-on o reset de la placa
    {
        char unsigned time_delay;  // Variable local utilizada para temporizar 
    
        time_delay = 0x64;  // Cada interrupcion se produce en 0.010 mseg, con este
                            // valor se consigue un retardo de 1 segundo

        do{
            if(flag_timer_interrup == 0x01){    // Chequea estado del flag si hubo interrupcion 
                flag_timer_interrup = 0x00;     // Inicializa flag y espera la proxima interrupcion
                time_delay--;                   // Decrementa el timer
            }
            CLRWDT();                           // Refresh WDT
        }
        while (time_delay > 0);                 // Chequea si se cumplio el tiempo de retardo

        return;
    }
