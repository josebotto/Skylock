/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:    st_inputs_digital.h
 * Author:  JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 4 de octubre de 2017, 10:28
 * 
 * Log:
 *     Control of digital inputs
 *     
 *     This module reads the digital inputs, stores its status 
 *     current and previous in inputs_pcl [].
 *     It also leaves the data ready if it should be reported to the
 *     a change occurs. Another module will be responsible for taking that data.
 *     In the variable status_in additional information of each one is saved
 *     entrance in particular.
 *  
 * =====================================================================
 */


#ifndef ST_INPUTS_DIGITAL_H
#define	ST_INPUTS_DIGITAL_H

    #include "compile_code.h"

    // Eventos de la FSM
    enum  Inputs_Event_Codes {                     
        evINI_INPUTS = 0,
        evRead_INPUTS,
        evUpdate_INPUTS,
    };

    // Nombre de la FSM
    StateMachine		inputsStateMachine;		        

    // Estados de la FSM
    TStateList	stInit_Inputs[], stRead_Inputs[], stUpdate_Inputs[];

    // Estructura y variables para entradas externas
    struct Inputs  inputs_pcl[] = {                     // Define la extructura de entradas externas y establece valor inicial
        { inaa      , &PORTA , 0x08 , 0x00 , 0xFF },    // Todas las entradas seran reportadas ante un cambio de estado inicialmente
        { inab      , &PORTA , 0x04 , 0x00 , 0xFF },        
        { inba      , &PORTA , 0x10 , 0x00 , 0xFF },
        { inbb      , &PORTA , 0x20 , 0x00 , 0xFF },
        { i_aux1    , &PORTB , 0x10 , 0x00 , 0xFF },
        { i_aux2    , &PORTB , 0x40 , 0x00 , 0xFF },
        { tamp      , &PORTB , 0x80 , 0x00 , 0xFF },
        #ifdef COMPILAR_PLACA_PCC
            { swconf    , &PORTC , 0x20 , 0x00 , 0xFF },
        #else
            { swconf    , &PORTE , 0x08 , 0x00 , 0xFF },
        #endif
    };

    // Declarando funciones
    void Ini_InputsStateMachine(void);         // Inicializa InputsStateMachine 
    void Read_InputsStateMachine(void);        // Lecturas de las entradas digitales
    void Update_InputsStateMachine(void);      // Update a la tabla previo analisis de sus entradas

#endif	/* ST_INPUTS_DIGITAL_H */
