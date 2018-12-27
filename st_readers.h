/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   st_readers.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 27 de diciembre de 2017, 19:55
 * 
 * Log:
 *     Management control of the reading and transfer logic.
 *     Control interface readers ABA / Wiegand
 * 
 *     This state machine works with def_global-var.h, aba_wie.h and 
 *     read_pulse.h
 *      
 * =====================================================================
 */


#ifndef ST_READERS_H
#define	ST_READERS_H

    // Eventos de la FSM
    enum Reader_Event_Codes {
        evINI_READERS = 0,
        evCHECKCARD_READERS,
        evERRORDATA_READERS,
        evPREPAREDATA_READERS,
        evUNLOCK_READERS,
    };
    
    // Nombre de la FSM
    StateMachine		readersStateMachine;

    // Estados de la FSM
    TStateList	stInit_Readers[], stWorking_Readers[], stUnlock_Readers[];

    // Variables y constantes
    static const 	byte		RdrClkMask[2] = {L1_CK_MSK, L2_CK_MSK};
    static const 	byte		RdrDataMask[2] = {L1_DATA_MSK, L2_DATA_MSK};

    static byte 	nCardBuf;   // Indice de CardBuf. Identifica de que lectora viene el dato
    
    // Estructura y variables para lectores
    struct Readers  readers_pcl[2] = {       //Define extructura 
    //    reader_name    reader_value                         status_in   
        { reader_a    , 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  , 0x00      },
        { reader_b    , 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  , 0x00      },
    };
    
    // Declarando funciones
    void Ini_ReadersStateMachine(void);
    void Checkcard_ReadersStateMachine(void);
    void Errordata_ReadersStateMachine(void);
    void Preparedata_ReadersStateMachine(void);
    void Unlock_ReadersStateMachine(void);
        
#endif	/* ST_READERS_H */         
    