/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   st_rs485.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 11 de enero de 2018, 15:21
 * 
 * Log:
 *     RS485 communication module. State Machine(FSM).
 * 
 * Variables of Input/output to the module st_rs485.h:
 * RX mode:
 *         flag_RXoTX: Used to indicate the module in which state
 *                     (0 in RX / 1 mode in TX mode)
 *         flag_data_present: Indicates that there is a valid data in the
 *                            variable data_RS485 to be analyzed.
 *         data_RS485: This variable stores the value obtained from
 *                     the EUSART (CREGS Registry).
 *
 * TX Mode:
 *         flag_RXoTX: IDEM RX mode.
 *         flag_data_present: Indicates that there is a valid data in the
 *                            variable data_RS485 to be sent by the
 *                            EUSART.
 *         data_RS485: This variable stores the value to be sent.
 *         count_FERR: This variable accumulates framming errors, its use is
 *                     by way of analysis. The user initializes it on demand.
 *         count_OERR: This variable accumulates overrun errors, its use is
 *                     by way of analysis. The user initializes it on demand.
 *  
 * =====================================================================
 */


#ifndef ST_RS485_H
#define	ST_RS485_H

    // Eventos de la FSM
    enum  Rs485_Event_Codes {
        evINI_RS485 = 0,
        evRX_RS485,
        evRX2TX_RS485,
        evTX_RS485,
        evTX2RX_RS485,
    };

    // Nombre de la FSM
    StateMachine		rs485StateMachine;		        

    // Estados de la FSM
    TStateList	stInit_Rs485[], stRx_Rs485[], stTx_Rs485[];

    // Variables
    char  flag_RXoTX;           // Utilizado para controlar la Rx o TX
    char  flag_data_present;    // Utilizada para informar cuando hay un dato valido en Rx o Tx
    char  data_RS485;           // Utilizado para almacenar caracter recibido y tambien para ser enviados

    // Declarando funciones
    void Ini_Rs485StateMachine(void);        // Inicializa Rs485StateMachine 
    void Rx_Rs485StateMachine(void);         // Recepcion de datos
    void Rx2tx_Rs485StateMachine(void);      // Transicion de Rx a Tx
    void Tx_Rs485StateMachine(void);         // Trasmision de datos
    void Tx2rx_Rs485StateMachine(void);      // Transicion de Tx a Rx

#endif	/* ST_RS485_H */
