/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   state_machine.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 25 de agosto de 2017, 10:02
 * 
 * Log:
 *     State Machine(FSM -finite state machine). Engine.  
 *  
 * =====================================================================
 */


#ifndef STATE_MACHINE_H
#define	STATE_MACHINE_H

    #define MAX_FIFO_LENGTH 	   0x20		                 // Tamaño de la cola. Debe ser potencia de 2. (0x20 / 32d / 00100000b)
                                                             // Considerar para su definicion que cada State Machine necesita dos bytes
                                                             // y adicionar dos byte mas minimo. Este valor permite hasta 15. 
    #define MAX_FIFO_LENGTH_MASK   0x1F	                     // Mascara que aisla bits para contar hasta MAX_FIFO_LENGTH (0x1F / 31d / 00011111b)
    #define	NEXT(a)   (((a) + 1) & MAX_FIFO_LENGTH_MASK)     // Utilizado para la cola circular
    #define	QUEUE_FULL   0x80                                 

    typedef const struct StateList  TStateList, *PStateList; 

    typedef const struct StateList {                         // Extructura utilizada por la FSM
        char	 	Event;
        void 		(*Func) (void);
        PStateList	Next_State;
    } TStateList, *PStateList;

    typedef TStateList		*StateMachine;
    typedef StateMachine	*PStateMachine;

    // Declarando funciones
    void stm_init_fifo(void);                                       // Inicializa FSM
    unsigned char stm_read_fifo(void);                              // Lee datos de la FIFO
    void stm_write_fifo(char Data);                                 // Escribe datos de la FIFO 
    void stm_msg2statemachine(char Event, PStateMachine Machine);   // Graba en FiFO proximo estado de la FSM
    void state_machine(void);                                       // Engine

#endif	/* STATE_MACHINE_H */
