/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   read_pulse.h
 * Author: JDB
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 27 de diciembre de 2017, 19:35
 * 
 * Log:
 *      - Code taken from plate ML02 version 5.0.
 *      - Reading of pulses to be used as input of reading devices
 *      - In all cases a call is generated to a routine that will deal with the change of
 *        bit status.
 *      - These routines set an interruption vector for the reception of pulses
 *        and filtering spurious interruptions. 
 * 
 * =====================================================================
 */


#ifndef READ_PULSE_H
#define	READ_PULSE_H

    /********************************************************
        Data types and constants
    ********************************************************/
    #define			MAXINTS				2			// Maxima cantidad de entradas de bit a manejar por interrupcion
    #define			MAXPOLL				0			// Maxima cantidad de entradas de bit a manejar por polling
    #define			MAXINPUTS			(MAXINTS+MAXPOLL)	// Maxima cantidad de entradas de bit a manejar.
                                                            // Se allocan 8 bytes por entrada
    #define			TOUT_HELD			0x80		// Flag de contador de timeout detenido
    #define			TIME_OUT			0xFF		// Flag de timeout alcanzado
    #define			PUL_OK				0
    #define			PUL_INVALIDINPUT	1
    #define			PUL_INPUTINUSE		2
    #define	        BITMASK(nbit)		(1 << (nbit))
    #define CARDBUF_BANK            // Buffer para las estructuras de control

    typedef  int (*TDataFunc)(void CARDBUF_BANK *ptr, byte data);

    // Estructura usada para el control de la rutina de interrupción
    typedef struct {
        byte			Input;				// Numero de entrada
        byte			BitMask;			// Mascara del bit de CLOCK (ABA) o D1 (Wiegand)
        byte			ToutCtr;			// Contador de time-out para detectar el fin de la lectura
        byte			PulseTout;			// Tiempo a esperar (en incrementos de 92 uS) de silencio para
                                            // detectar el fin de los pulsos
        TDataFunc 		DataFunc;			// Pointer a la rutina de lectura de bits
    } TIntCtrl;

    /********************************************************
        Prototypes published
    ********************************************************/
    int InitPulse(byte Flags);

    int InitPulseInput(byte chnl, byte BitMask, byte TimeOut, TDataFunc DataFunc,
                             volatile CARDBUF_BANK TIntCtrl *CtrlBuf);

    void *memset_b3(CARDBUF_BANK void *s, char c, int n);
    int ReadPulse_tick_ii0(int data);

    void EnableInts(volatile CARDBUF_BANK TIntCtrl *CtrlBuf, byte BitMask, byte enable);
    void ReadPulse_isr(byte data); 	// User callable ISR routine

    #define EnableInt(_CtrlBuf, _BitMask)		EnableInts(_CtrlBuf, _BitMask, 1)
    #define DisableInt(_CtrlBuf, _BitMask)		EnableInts(_CtrlBuf, _BitMask, 0)

#endif	/* READ_PULSE_H */
