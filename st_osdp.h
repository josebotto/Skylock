/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   st_osdp.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 9 de enero de 2018, 10:06
 * 
 * Log:
 *     OSDP Protocol Module. Decode & encode. State Machine(FSM).  
 *  
 * =====================================================================
 */


#ifndef ST_OSDP_H
#define	ST_OSDP_H

    // Eventos de la FSM
    enum  Osdp_Event_Codes {                     
                            evINI_OSDP = 0,
                            evRXHEADER_OSDP,
                            evRXID_OSDP,
                            evRXBYTES_OSDP,
                            evRXERRORLENGTH,
                            evRXREMAININGBYTES_OSDP,
                            evRXCSANALYZE_OSDP,
                            evTXPREPAREPACKAGE,
                            evTXSENDPACKAGE,
                            evTX2MODERX,
    };

    // Nombre de la FSM
    StateMachine		osdpStateMachine;		        

    // Estados de la FSM
    TStateList	stInit_Osdp[], stRxData_Osdp[], stTxData_Osdp[];

    // Variables y constantes
    #define min_byte_osdp    0x0008     // Minima cantidad de byte por trama definido por OSDP(8 bytes) 
    #define max_byte_osdp    0x0400     // Maxima cantidad de byte por trama definido por OSDP(1024 bytes)

    #define frame_without_errors    0x00
    #define error_checksum_L_CRC    0x01
    #define error_checksum_H_CRC    0x02
    #define error_min_byte_osdp     0x03
    #define error_max_byte_osdp     0x04
    #define error_max_buf_osdp      0x05

    char  ctrl_osdp;                     // Registro de control osdp
    unsigned int  pos_osdp;              // Indice utilizado para posicionarse en cada elemento de buf_osdp 

    // Declarando funciones
    void Ini_OsdpStateMachine(void);                 // Inicializacion de la FSM OsdpStateMachine
    void RxHeader_OsdpStateMachine(void);            // Espera Recepcion del SOM (STart of Mensaje))
    void RxId_OsdpStateMachine(void);                // Verifica si la trama a recibir le corresponde 
    void RxBytes_OsdpStateMachine(void);             // Recibi informacion sobre la cantidad de bytes a recibir
    void RxErrorlength_OsdpStateMachine(void);       // Informa error si la cantidad de bytes a recibir no esta dentro de los parametros definidos 
    void RxRemainingBytes_OsdpStateMachine(void);    // Recepciona los bytes informados 
    void RxChecksumAnalyze_OsdpStateMachine(void);   // Analiza el checksum 
    void TxPreparePackage_OsdpStateMachine(void);    // Prepara trama para TX
    void TxSendPackage_OsdpStateMachine(void);       // Envia trama
    void Tx2ModeRx_OsdpStateMachine(void);           // Pasa al modo RX
    char Cmd_OSDP(void);                             // Analiza cual fue el cmd recibido y lo ejecuta

#endif	/* ST_OSDP_H */
