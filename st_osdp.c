#include <xc.h>
#include "def_global_var.h"
#include "eeprom.h"
#include "state_machine.h"
#include "osdp.h"
#include "osdp_functions.h"
#include "st_rs485.h"
#include "st_osdp.h"


/*************************************************************************/
// States of the state machine
/*************************************************************************/
TStateList	stInit_Osdp[] = { 
	evINI_OSDP              ,Ini_OsdpStateMachine                ,stRxData_Osdp,
};

TStateList	stRxData_Osdp[] = {
	evRXHEADER_OSDP         ,RxHeader_OsdpStateMachine	         ,stRxData_Osdp,
	evRXID_OSDP             ,RxId_OsdpStateMachine	             ,stRxData_Osdp,
    evRXBYTES_OSDP          ,RxBytes_OsdpStateMachine            ,stRxData_Osdp,
    evRXERRORLENGTH         ,RxErrorlength_OsdpStateMachine      ,stTxData_Osdp,
	evRXREMAININGBYTES_OSDP ,RxRemainingBytes_OsdpStateMachine   ,stRxData_Osdp,
	evRXCSANALYZE_OSDP      ,RxChecksumAnalyze_OsdpStateMachine  ,stTxData_Osdp,
};

TStateList	stTxData_Osdp[] = {
	evTXPREPAREPACKAGE      ,TxPreparePackage_OsdpStateMachine	 ,stTxData_Osdp,
	evTXSENDPACKAGE         ,TxSendPackage_OsdpStateMachine   	 ,stTxData_Osdp,
	evTX2MODERX             ,Tx2ModeRx_OsdpStateMachine      	 ,stRxData_Osdp,
};

//******************************************************************************
// Declaring Functions
//******************************************************************************
void Ini_OsdpStateMachine(void)     // Inicializacion de la FSM OsdpStateMachine
{
    count_err_chks_osdp = 0x0000;       //Contador errores de checksum 
    count_min_byte_osdp = 0x0000;       //Contador errores de recepcion tramas menores al tamaño minimo establecido por OSDP
    count_max_byte_osdp = 0x0000;       //Contador errores de recepcion tramas mayores al tamaño maximo establecido por OSDP   
    count_max_buf_osdp = 0x0000;        //Contador errores de recepcion tramas con tamaño mayor al soportado por la placa y menor del establecido por OSDP

    pos_osdp = 0x0001;                  //Posicion en msg_osdp 

    PD_ID = read_eeprom(USERD_PD_ID);   // Actualiza PD-ID desde la Eeprom USER DEFAULT
    
    ctrl_osdp = 0x04;                   // Bit 0-1  SQN         The sequence number of the message is used for message delivery confirmation
                                        //                      and for error recovery.
                                        // Bit 2    CKSUM/ CRC  Set ? 16-bit CRC is contained in the last 2 bytes of the message
                                        //                      Clear ? 8-bit CHECKSUM is contained in the last byte of the messageSQN 
                                        // Bit 3    SCB         Set ? Security Control Block is present in the message
                                        //                      Clear ? No Security Control block in the message
                                        // Bit 4-6              Deprecated (formerly Reply Status Field) 
                                        // Bit 7    MULTI       Set ? more packets to come for this message
                                        //                      Clear ? this is the last/only packet of this message

    stm_msg2statemachine(evRXHEADER_OSDP, &osdpStateMachine);
    return;
}

void RxHeader_OsdpStateMachine(void)       // Espera Recepcion del SOM (Start of Mensaje))    
{
    if(flag_data_present){                                      // Chequea si existe un dato presente
        flag_data_present = 0;
        if(data_RS485==C_SOM){                                  // Chequea si es el encabezado de la trama
            msg_osdp[pos_osdp] = data_RS485;
            pos_osdp++;
            stm_msg2statemachine(evRXID_OSDP, &osdpStateMachine);   
            return;
        }     
    }     

    stm_msg2statemachine(evRXHEADER_OSDP, &osdpStateMachine);
    return;
}

void RxId_OsdpStateMachine(void)       // Verifica si la trama a recibir le corresponde 
{
    if(flag_data_present){                                         // Chequea si existe un dato presente
        flag_data_present = 0;
        if(data_RS485==PD_ID || data_RS485==OSDP_PD_Broadcast){    // Chequea si es correcto el PD_ID o si se trata de un Broadcast
            msg_osdp[pos_osdp] = data_RS485;
            pos_osdp++;

        	timers[0x02].status_timer |= TIMER_TRIGGER;             // Produce el redisparo del timer cada vez que recibe del PD (Panel de control)
                                                                    // un paquete OSDP con el PD_ID definido para esta placa. Esto hace destellar 
                                                                    // el out_led_5 (Led On-Line) y evita el reset de la placa si esta configurada por el PD

            stm_msg2statemachine(evRXBYTES_OSDP, &osdpStateMachine);   // Pasa a la siguiente estapa
            return;
        }     
        else{
            pos_osdp = 0x0001;
            stm_msg2statemachine(evRXHEADER_OSDP, &osdpStateMachine);   // Dato no corresponde
            return;
        }
    }     

    stm_msg2statemachine(evRXID_OSDP, &osdpStateMachine);
    return;
}

void RxBytes_OsdpStateMachine(void)       // Recibe informacion sobre la cantidad de bytes esperados
{
    unsigned int  aux_byte;

    if(flag_data_present){                // Chequea si existe un dato presente
        flag_data_present = 0;
        msg_osdp[pos_osdp] = data_RS485;
        pos_osdp++;

        if(pos_osdp==0x05){               // Recibio los dos char correspondiente a la longitud de la trama y lo chequea

            aux_byte = (int)msg_osdp[4];
            aux_byte = (aux_byte<<8)+(int)msg_osdp[3];

            if((aux_byte >= min_byte_osdp) && (aux_byte <= max_buf_osdp) ){
                stm_msg2statemachine(evRXREMAININGBYTES_OSDP, &osdpStateMachine);
                return;
            }
            else{
                stm_msg2statemachine(evRXERRORLENGTH, &osdpStateMachine);
                return;
            }
        }
    }

    stm_msg2statemachine(evRXBYTES_OSDP, &osdpStateMachine);
    return;
}

void RxRemainingBytes_OsdpStateMachine(void)       // Recepciona los bytes informados
{
    unsigned int  aux_byte;

    if(flag_data_present){                         // Chequea si existe un dato presente
        flag_data_present = 0;
        msg_osdp[pos_osdp] = data_RS485;

        aux_byte = (int)msg_osdp[4];
        aux_byte = (aux_byte<<8)+(int)msg_osdp[3];

        if(pos_osdp == aux_byte){                                         // Chequea si se recibieron todos los char definidos en la longitud de la trama 
            stm_msg2statemachine(evRXCSANALYZE_OSDP, &osdpStateMachine);  // Pasa al proximo estado
            return;      
        }
        pos_osdp++;
    }

    stm_msg2statemachine(evRXREMAININGBYTES_OSDP, &osdpStateMachine);
    return;
}

void RxChecksumAnalyze_OsdpStateMachine(void)       // Analiza el checksum
{
    char  aux_crc; 
    unsigned int  aux_byte;

    aux_byte = (int)msg_osdp[4];
    aux_byte = (aux_byte<<8)+(int)msg_osdp[3];

    pos_osdp = aux_byte - 0x02;
    stm_msg2statemachine(evTXPREPAREPACKAGE, &osdpStateMachine);

    aux_crc = (char)(fCrcBlk((msg_osdp+1), pos_osdp));   
    if(msg_osdp[aux_byte - 0x01] != aux_crc){                   // Chequea el L_CRC
        count_err_chks_osdp++;                                  // En caso de error adiciona una unidad al contador
        msg_osdp[0] = error_checksum_L_CRC;
        return;
    }

    aux_crc = (char)(fCrcBlk((msg_osdp+1), pos_osdp)>>8);   
    if(msg_osdp[aux_byte] != aux_crc){                          // Chequea el H_CRC
        count_err_chks_osdp++;                                  // En caso de error adiciona una unidad al contador
        msg_osdp[0] = error_checksum_H_CRC;
        return;
    }

    msg_osdp[0] = frame_without_errors;                         // Informa que la trama recibida no tiene errores
    return;
}

void RxErrorlength_OsdpStateMachine(void)       // Informa error si la cantidad de bytes a recibir no esta dentro de los parametros definidos
{
    unsigned int  aux_byte;

    aux_byte = (int)msg_osdp[4];
    aux_byte = (aux_byte<<8)+(int)msg_osdp[3];

    stm_msg2statemachine(evTXPREPAREPACKAGE, &osdpStateMachine); // Define el proximo estado

    if(aux_byte < min_byte_osdp){                                // Identifica error Minima cantidad de byte por trama y adiciona contador
        msg_osdp[0] = error_min_byte_osdp;                   
        count_min_byte_osdp++;
        return;      
    }

    if(aux_byte > max_byte_osdp){                                // Identifica error Maxima cantidad de byte por trama definido por OSDP y adiciona contador
        msg_osdp[0] = error_max_byte_osdp;                  
        count_max_byte_osdp++;
        return;      
    }

    msg_osdp[0] = error_max_buf_osdp;                            // Identifica error Maxima cantidad de byte de la trama a soportar y adiciona contador
    count_max_buf_osdp++;
    return;      
}

void TxPreparePackage_OsdpStateMachine(void)        // Prepara trama para TX
{
    char  estado;
    char  aux_ctrl;
    unsigned int  aux_byte;

    estado = msg_osdp[0];

    switch(estado){                                                  // Analiza como tratar los errores o dato valido
        case frame_without_errors:                                   // Dato valido
            if(Cmd_OSDP() == 0x00){
                stm_msg2statemachine(evTX2MODERX, &osdpStateMachine);
                return;      
            } 
            break;

        case error_checksum_L_CRC:			                         // Error de checksum
        case error_checksum_H_CRC:			                         // Responde al CP a excepcion si se produjo en un broadcast
            if(msg_osdp[2] == OSDP_PD_Broadcast){
                stm_msg2statemachine(evTX2MODERX, &osdpStateMachine);
                return;      
            }
            msg_osdp[3] = 0x02;      // Es la cantidad de elementos (comando + atributos) 
            msg_osdp[4] = 0x00;       
            msg_osdp[6] = OSDP_NAK;  // Comando OSDP_NAK   0x41
            msg_osdp[7] = 0x01;      // Indica Message check character(s) error (bad cks/crc/mac[4]  
            break;

        case error_min_byte_osdp:                                    // Error de tamaño de trama			
        case error_max_byte_osdp:	                                 // No responde solo es adicionado en el contador correspondiente
            stm_msg2statemachine(evTX2MODERX, &osdpStateMachine);
            return;      
            break;

        case error_max_buf_osdp:                                     // Error de tamaño de trama. Cumple con los standard OSDP pero supera 
                                                                     // al valor establecido para esta placa.
            if(msg_osdp[2] == OSDP_PD_Broadcast){                    // Responde al CP a excepcion si se produjo en un broadcast
                stm_msg2statemachine(evTX2MODERX, &osdpStateMachine);
                return;      
            }
            msg_osdp[3] = 0x02;      // Es la cantidad de elementos (comando + atributos) 
            msg_osdp[4] = 0x00;       
            msg_osdp[6] = OSDP_NAK;  // Comando OSDP_NAK   0x41
            msg_osdp[7] = 0x01;      // Indica Message check character(s) error (bad cks/crc/mac[4]  
            break;
            
        default:     
            stm_msg2statemachine(evTX2MODERX, &osdpStateMachine);
            return;      
            break;
    }

    // Data frame OSDP. Arma la trama para ser enviada en el proximo estado de la FSM
    msg_osdp[0] = C_OSDP_MARK;                      // Marca utilizado para generar delay en TX    
    msg_osdp[1] = C_SOM;                            // Encabezado de trama
    msg_osdp[2] = OSDP_DEST_CP;                     // Direccion donde va diriguido el paquete. CP es control Panel

    aux_byte = (int)msg_osdp[4];                    // Actualiza cantidad de bytes de la trama
    aux_byte = (aux_byte<<8)+(int)msg_osdp[3];
    aux_byte = aux_byte + 0x0007;                   // Suma 7 que son los bytes fijos
    msg_osdp[3] = (char)aux_byte;                 
    msg_osdp[4] = (char)aux_byte>>8;                             

    aux_ctrl = msg_osdp[5];                         // Calcula el numero de scuencia del mensaje
    aux_ctrl = aux_ctrl + 0x01;
    aux_ctrl = aux_ctrl & 0x03;
    if(aux_ctrl == 0x00)
        aux_ctrl = 0x01;
    msg_osdp[5] = ctrl_osdp | aux_ctrl;  

    msg_osdp[aux_byte-1] = (char)(fCrcBlk((msg_osdp+1), aux_byte-0x02));      // Calcula Checksum L_CRC    
    msg_osdp[aux_byte]   = (char)(fCrcBlk((msg_osdp+1), aux_byte-0x02)>>8);   // Calcula Checksum H_CRC

    // Configuration Flags and Variables 
    flag_RXoTX = 1 ;                // Utilizado para controlar modo Rx(0) o TX(1)
    aux_flag_RXoTX = flag_RXoTX;    // Utilizada en st_monitoring para chequear estado de la comunicacion
    flag_data_present = 0;          // Reset flag
    pos_osdp = 0x0000;

    stm_msg2statemachine(evTXSENDPACKAGE, &osdpStateMachine);
    return;      
}

void TxSendPackage_OsdpStateMachine(void)   // Envia trama
{
    unsigned int  aux_byte;

    if(flag_data_present == 0){

        aux_byte = (int)msg_osdp[4];
        aux_byte = (aux_byte<<8)+(int)msg_osdp[3];


        if(aux_byte < pos_osdp){
            stm_msg2statemachine(evTX2MODERX, &osdpStateMachine);
            return;      
        }
        flag_data_present = 1;              // Activa flag
        data_RS485 = msg_osdp[pos_osdp];    // Envia dato
        pos_osdp++;
    }

    stm_msg2statemachine(evTXSENDPACKAGE, &osdpStateMachine);
    return;      
}

void Tx2ModeRx_OsdpStateMachine(void)   // Pasa al modo RX
{
    flag_RXoTX = 0;                 // Utilizado para controlar modo Rx(0) o TX(1)
    aux_flag_RXoTX = flag_RXoTX;    // Utilizada en st_monitoring para chequear estado de la comunicacion
    flag_data_present = 0;          // Reset flag
    pos_osdp = 0x0001;

    stm_msg2statemachine(evRXHEADER_OSDP, &osdpStateMachine);  //Pasa a modo RX
    return;      
}

char Cmd_OSDP(void)     // Analiza comando recibido y se ejecuta
{
 	unsigned short int num_bytes;   // Utilizado para calculo auxiliar

	// Limita al comando PD-ID=0x7E (OSDP_PD_Factory_Default)
    if(msg_osdp[2] == OSDP_PD_Factory_Default){     // Consulta si PD-ID = 0x7E (OSDP_PD_Factory_Default). 
                                                    // El PD-ID con OSDP_PD_Factory_Default solo se le permitira utiilizar el comandos OSDP_COMSET, es 
                                                    // forzar al usuario en cierta manera a cambiar el PD-ID.
            
		if(msg_osdp[6] != OSDP_COMSET){     // Consulta si el comando recibido es igual al comando OSDP_COMSET.
                                            // Si esta condicion se cumple informa con un comando OSDP_CHANGE_PDID al CP(Control Panel) indicando
                                            // que se debe cambiar el PD-ID para poder operar con la placa

            msg_osdp[3] = 0x01;         	// Es la cantidad de elementos (comando)
			msg_osdp[4] = 0x00;         						
			msg_osdp[6] = OSDP_CHANGE_PDID; // Comando OSDP_CHANGE_FD   0xD8
            return(1);                      // Hace que se envie mensaje al CP  
		}
    }
    else{
		if((flag_system_configuration == 0x00) && (msg_osdp[2] != OSDP_PD_Broadcast)){  // Consulta si el sistema fue configurado despues del ultimo
                                                                                        // Power-on o Reset y ademas no es un OSDP_PD_Broadcast para
                                                                                        // poder responder al Panel de Control(PD)

            if(msg_osdp[6] != OSDP_CONFIG){     // Consulta si el comando recibido es igual al comando OSDP_CONFIG.
                                                // Si esta condicion se cumple informa con un comando OSDP_CONF_SYSTEM al CP(Control Panel) indicando
                                                // que se debe enviar la configuracion de la placa

                msg_osdp[3] = 0x01;         	// Es la cantidad de elementos (comando)
                msg_osdp[4] = 0x00;         						
                msg_osdp[6] = OSDP_CONF_SYSTEM; // Comando OSDP_CONF_SYSTEM   0xD11
                return(1);                      // Hace que se envie mensaje al CP  
            }
		}
    }
    
	// Analiza comando OSDP
    switch(msg_osdp[6]) {
        case OSDP_POLL:     // OSDP_POLL   (0x60)   Poll
            if(msg_osdp[2] != OSDP_PD_Broadcast){       // Si es un broadcast no responde al CP
			
            	num_bytes = (int)msg_osdp[4];					// En calc_aux almacena la cantidad total de bytes del mensaje recibido
                num_bytes = (num_bytes<<8)+(int)msg_osdp[3];

                if(num_bytes == 0x0008){        // Si se cumple entonces el comando recibido es valido, coincide con la cantidad de bytes definidos en la trama OSDP.
                                                // El comando esta compuesto por 1 byte + 7 bytes propios de la trama OSDP hacen un total de 8 bytes

					if(Osdp_Functions_Poll())               // Chequea si hay eventos pendientes para enviar 
						return(1);                          // Envia elemento
											// Caso contrario enviara un ACK
					msg_osdp[3] = 0x01;    	// Es la cantidad de elementos (comando + atributos) 
					msg_osdp[4] = 0x00;         			
					msg_osdp[6] = OSDP_ACK;	// Responde Comando
				}
                else{
                    msg_osdp[3] = 0x02;     // Es la cantidad de elementos (comando + atributos), 
                    msg_osdp[4] = 0x00;         			
                    msg_osdp[6] = OSDP_NAK; // Comando OSDP_NAK   0x41
                    msg_osdp[7] = 0x01;     // Indica Message check character(s) error (bad cks/crc/mac[4]  
                }
                return(1);                  // Hace que se envie mensaje al CP  
            }
            break;

        case OSDP_ISTAT:    // OSDP_ISTAT   (0x65)   Input Status Report Request  
            if(msg_osdp[2] != OSDP_PD_Broadcast){       	 	// Si es un broadcast no se ejecuta la accion ni se responde al CP
			
            	num_bytes = (int)msg_osdp[4];					// En calc_aux almacena la cantidad total de bytes del mensaje recibido
                num_bytes = (num_bytes<<8)+(int)msg_osdp[3];

                if(num_bytes == 0x0008){        // Si se cumple entonces el comando recibido es valido, coincide con la cantidad de bytes definidos en la trama OSDP.
                                                // El comando esta compuesto por 1 byte + 7 bytes propios de la trama OSDP hacen un total de 8 bytes

					Osdp_Functions_Istat();			// Prepara informe de estado de las entradas para enviarlo 
				}
                else{
                    msg_osdp[3] = 0x02;             // Es la cantidad de elementos (comando + atributos), 
                    msg_osdp[4] = 0x00;         			
                    msg_osdp[6] = OSDP_NAK;         // Comando OSDP_NAK   0x41
                    msg_osdp[7] = 0x01;         	// Indica Message check character(s) error (bad cks/crc/mac[4]  
                }
                return(1);                   		// Hace que se envie mensaje al CP  
            }
            break;

        case OSDP_OSTAT:    // OSDP_OSTAT   (0x66)   Output Status Report Request
            if(msg_osdp[2] != OSDP_PD_Broadcast){       	 	// Si es un broadcast no se ejecuta la accion ni se responde al CP

            	num_bytes = (int)msg_osdp[4];					// En calc_aux almacena la cantidad total de bytes del mensaje recibido
                num_bytes = (num_bytes<<8)+(int)msg_osdp[3];

                if(num_bytes == 0x0008){        // Si se cumple entonces el comando recibido es valido, coincide con la cantidad de bytes definidos en la trama OSDP.
                                                // El comando esta compuesto por 1 byte + 7 bytes propios de la trama OSDP hacen un total de 8 bytes

					Osdp_Functions_Ostat();			// Prepara informe de estado de las salidas para enviarlo 
				}
                else{
                    msg_osdp[3] = 0x02;             // Es la cantidad de elementos (comando + atributos), 
                    msg_osdp[4] = 0x00;         			
                    msg_osdp[6] = OSDP_NAK;         // Comando OSDP_NAK   0x41
                    msg_osdp[7] = 0x01;         	// Indica Message check character(s) error (bad cks/crc/mac[4]  
                }
                return(1);                   		// Hace que se envie mensaje al CP  
            }
            break;

        case OSDP_OUT:      // OSDP_OUT   (0x68)   Output Control Command
			Osdp_Functions_Out();								// Ejecuta la accion indicada para cada salida. 
			if(msg_osdp[2] != OSDP_PD_Broadcast){       	 	// Si es un broadcast ejecuta la accion, no se responde al CP
				return(1);                               		// No es un broadcast, responde al CP
            }
            break;
            
        case OSDP_COMSET:   // OSDP_COMSET   (0x6E)   Communication Configuration Command 
            if(msg_osdp[2] != OSDP_PD_Broadcast){       	 	// Si es un broadcast no se ejecuta la accion y no responde al CP
				Osdp_Functions_Comset();						// Modifica PD-ID (Perisferial Device ID)y Baud rate del PD. Primero informa el cambio 
                                                                // luego hace un reset en el sistema 
                return(1);                               		// Responde al Control Panel(CP)
            }
            break;

        case OSDP_FACTORY:  // OSDP_FACTORY   (0xD0)   Set Factory Default. Comando propietario Skylock. Setea con parametros de fabrica y reinicia el sistema
                            //                         Su extructura solo es un byte que representa al comando, luego se le adiciona los bytes 
							//						   necesarios para armar la trama osdp.
							//                         La respuesta es un OSDP_FACTORYR, luego inicia la secuencia de Factory Default. 								
		
            if(msg_osdp[2] != OSDP_PD_Broadcast){       // Si es un broadcast no se ejecuta la accion y no responde al CP

            	num_bytes = (int)msg_osdp[4];					// En calc_aux almacena la cantidad total de bytes del mensaje recibido
                num_bytes = (num_bytes<<8)+(int)msg_osdp[3];    

                if(num_bytes == 0x0008){        // Si se cumple entonces el comando recibido es valido, coincide con la cantidad de bytes definidos en la trama OSDP.
                                                // El comando esta compuesto por 1 byte + 7 bytes propios de la trama OSDP hacen un total de 8 bytes

					flag_monitoring = 0x02;         // Este valor define que es un Factory Reset. Primero Respondera con un OSDP_FACTORYR
													// luego configurara con los parametros de Factory default y finalmente un reset en el 
													// sistema (ver st_monitoring.c)
					msg_osdp[3] = 0x01;         	// Es la cantidad de elementos. En este caso solo es un comando. 
					msg_osdp[4] = 0x00;         						
					msg_osdp[6] = OSDP_FACTORYR;    // Responde Comando
				}
                else{
                    msg_osdp[3] = 0x02;             // Es la cantidad de elementos (comando + atributos), 
                    msg_osdp[4] = 0x00;         			
                    msg_osdp[6] = OSDP_NAK;         // Comando OSDP_NAK   0x41
                    msg_osdp[7] = 0x01;             // Indica Message check character(s) error (bad cks/crc/mac[4]  
                }
                return(1);                   		// Hace que se envie mensaje al CP  
            }
            break;

		case OSDP_RESET:	// OSDP_RESET   (0xD2)   Reset System. Comando propietario Skylock. Generador de reset.
                            //                       Su extructura solo es un byte que representa al comando, luego se le adiciona los bytes 
							//						 necesarios para armar la trama osdp.
							//                       La respuesta es un OSDP_RESETR y luego inicia la secuencia de Reset. 								
		
            if(msg_osdp[2] != OSDP_PD_Broadcast){       	// Si es un broadcast no se ejecuta la accion y no responde al CP

            	num_bytes = (int)msg_osdp[4];					// En calc_aux almacena la cantidad total de bytes del mensaje recibido
                num_bytes = (num_bytes<<8)+(int)msg_osdp[3];	

                if(num_bytes == 0x0008){        // Si se cumple entonces el comando recibido es valido, coincide con la cantidad de bytes definidos en la trama OSDP.
                                                // El comando esta compuesto por 1 byte + 7 bytes propios de la trama OSDP hacen un total de 8 bytes

					flag_monitoring = 0x03;     // Este valor define que es un Reset. Primero Respondera con un OSDP_RESETR
												// luego un reset en el sistema(ver st_monitoring.c)
					msg_osdp[3] = 0x01;         // Es la cantidad de elementos. En este caso solo es un comando. 
					msg_osdp[4] = 0x00;         						
					msg_osdp[6] = OSDP_RESETR;  // Responde Comando
				}
                else{
                    msg_osdp[3] = 0x02;         // Es la cantidad de elementos (comando + atributos), 
                    msg_osdp[4] = 0x00;         			
                    msg_osdp[6] = OSDP_NAK;     // Comando OSDP_NAK   0x41
                    msg_osdp[7] = 0x01;         // Indica Message check character(s) error (bad cks/crc/mac[4]  
                }
                return(1);                   	// Hace que se envie mensaje al CP  
            }
            break;
            
		case OSDP_ESTABLISH:	// OSDP_ESTABLISH   (0xD4)   Set Values. Comando propietario Skylock. 
                                // Permite habilitar o deshabilitar la accion reportar eventos de los dispositivos.
                                // El comando esta compuesto por tres bytes: Comando + Atributo + dispositivo. 
                                // Por default todos los dispositivos tienen deshabilitado el envio de eventos al 
                                // iniciarse o reiniciarse el sistema.    
                                // El atributo que acompaña al comando puede tener los siguientes valores:
                                //                  0x00:   Indica habilitar reportar evento.
                                //                  0x01:   Indica deshabilitar reportar evento. 
                                // La respuesta es un OSDP_ESTABLISHR confirmando la operacion.
                                // Si el dispositivo no es conocido, tamaño del mensaje no coincide con lo especificado o atributo desconocido
                                // respondera con OSDP_NAK

            if(msg_osdp[2] != OSDP_PD_Broadcast){       // Si es un broadcast no se ejecuta la accion y no responde al CP
				Osdp_Functions_Establish();				// Analiza el atributo del comando 
                return(1);                              // Responde al Control Panel(CP)
            }
            break;

		case OSDP_WATCH:	// OSDP_WATCH   (0xD6)   Status Errors Command. Comando propietario Skylock.
                            // Permite visualizar e inicializar los distintos contadores de error y brinda informacion sobre
                            // la identificacion del sistema y el hardware asociado.
                            // El comando esta compuesto por dos byte: Comando + Atributo.    
                            // El atributo que acompaña al comando puede tener los siguientes valores:
                            //                  0x00:   Envia informacion sobre el sistema y el hardware asociado
                            //                  0x01:   Envia informacion de los contadores de error(cnt_power.h) en el siguiente orden: 
                            //                          POR, BOR, TO, PD, RI . Cada variable es representada por un char
                            //                  0x02:   Inicializa contadores de error en st_osdp.h 
                            //                  0x03:   Envia informacion de los contadores de error(st_osdp.h) en el siguiente orden: count_err_chks_osdp, 
                            //                          count_min_byte_osdp, count_max_byte_osdp, count_max_buf_osdp. Cada variable es representada por un char
                            //                  0x04:   Inicializa contadores de error en st_rs485 
                            //                  0x05:   Envia informacion de los contadores de error(st_rs485) en el siguiente orden: count_FERR, count_OERR
                            //                          Cada variable es representada por un char
                            // La respuesta es un OSDP_WATCHR. Esta compuesta por el comando OSDP_WATCHR + Atributo recibido + variables de 
                            // de error solicitadas por el comando OSDP_WATCH. 
            
            if(msg_osdp[2] != OSDP_PD_Broadcast){       	// Si es un broadcast no se ejecuta la accion y no responde al CP
				Osdp_Functions_Watch();						// Analiza el atributo del comando 
                return(1);                               	// Responde al Control Panel(CP)
            }
            break;
            
		case OSDP_CONFIG:   // OSDP_CONFIG   (0xD9)  Comando Skylock. Configuracion del Sistema. Informa que se configurara la placa debido al Power-on o Reset ocurrido
                            //                       Su extructura solo es un byte que representa al comando en la trama osdp.
							//                       La respuesta es un OSDP_CONFIGR la cual es contenida en un byte. 								
		
            if(msg_osdp[2] != OSDP_PD_Broadcast){       // Si es un broadcast no se ejecuta la accion y no responde al CP

            	num_bytes = (int)msg_osdp[4];					// En calc_aux almacena la cantidad total de bytes del mensaje recibido
                num_bytes = (num_bytes<<8)+(int)msg_osdp[3];

                if(num_bytes == 0x0008){        // Si se cumple entonces el comando recibido es valido, coincide con la cantidad de bytes definidos en la trama OSDP.
                                                // El comando esta compuesto por 1 byte + 7 bytes propios de la trama OSDP hacen un total de 8 bytes
 
                    outputs_signaling_pcl[5].status_out = 0x15;     // El led out_led_1 deja de destellar apagandose.
                    flag_system_configuration = 0x01;               // Con este valor se indica que el CP(Control Panel) fue informado que la placa debe ser configurada
                                                                    // y las proximas tramas OSDP seran de configuracion.

                    msg_osdp[3] = 0x01;             // Es la cantidad de elementos. En este caso solo es un comando de respuesta. 
                    msg_osdp[4] = 0x00;         						
                    msg_osdp[6] = OSDP_CONFIGR;     // Responde Comando
                }
                else{
                    msg_osdp[3] = 0x02;             // Es la cantidad de elementos (comando + atributos), 
                    msg_osdp[4] = 0x00;         			
                    msg_osdp[6] = OSDP_NAK;         // Comando OSDP_NAK   0x41
                    msg_osdp[7] = 0x01;             // Indica Message check character(s) error (bad cks/crc/mac[4]  
                }
                return(1);                      // Responde al Control Panel(CP)
            }
            break;
 
                            // Comandos reconocidos pero no implementados 
        case OSDP_ID:       // OSDP_ID       (0x61)               
        case OSDP_CAP:      // OSDP_CAP      (0x62)  
        case OSDP_DIAG:     // OSDP_DIAG     (0x63)  
        case OSDP_LSTAT:    // OSDP_LSTAT    (0x64)  
        case OSDP_RSTAT:    // OSDP_RSTAT    (0x67)  
        case OSDP_LED:      // OSDP_LED      (0x69)  
        case OSDP_BUZ:      // OSDP_BUZ      (0x6A)  
        case OSDP_TEXT:     // OSDP_TEXT     (0x6B)      
        case OSDP_TDSET:    // OSDP_TDSET    (0x6D)   
        case OSDP_DATA:     // OSDP_DATA     (0x6F)      
        case OSDP_PROMPT:   // OSDP_PROMPT   (0x71)  
        case OSDP_BIOREAD:  // OSDP_BIOREAD  (0x73) 
        case OSDP_KEYSET:   // OSDP_KEYSET   (0x75)  
        case OSDP_CHLNG:    // OSDP_CHLNG    (0x76)   
        case OSDP_MFG:      // OSDP_MFG      (0x80)  
            if(msg_osdp[2] != OSDP_PD_Broadcast){       // Si es un broadcast no responde al CP
                msg_osdp[3] = 0x02;      // Es la cantidad de elementos (comando + atributos) 
            	msg_osdp[4] = 0x00;         						
                msg_osdp[6] = OSDP_NAK;  // Comando OSDP_NAK   0x41
                msg_osdp[7] = 0x03;      // Indica Unknown Command Code ? Command not implemented by PD  
                return(1);               // Hace que se envie mensaje al CP
            }
            break;

        default:                                        // Comandos no reconocidos
            if(msg_osdp[2] != OSDP_PD_Broadcast){       // Si es un broadcast no responde al CP
                msg_osdp[3] = 0x02;      // Es la cantidad de elementos (comando + atributos) 
            	msg_osdp[4] = 0x00;         						
                msg_osdp[6] = OSDP_NAK;  // Comando OSDP_NAK   0x41
                msg_osdp[7] = 0x04;      // Unexpected sequence number detected in the header  
                return(1);               // Hace que se envie mensaje al CP
            }
            break;
    }

    return(0);                           // Hace que no se envie mensaje al CP
}
