#include <xc.h>
#include "compile_code.h"
#include "def_hardware.h"
#include "def_global_var.h"
#include "eeprom.h"
#include "state_machine.h"
#include "st_rs485.h"


/*************************************************************************/
// States of the state machine
/*************************************************************************/
TStateList	stInit_Rs485[] = {
    evINI_RS485         ,Ini_Rs485StateMachine         ,stRx_Rs485,
};

TStateList	stRx_Rs485[] = {
	evRX_RS485          ,Rx_Rs485StateMachine    	   ,stRx_Rs485,
	evRX2TX_RS485       ,Rx2tx_Rs485StateMachine       ,stTx_Rs485,
};

TStateList	stTx_Rs485[] = {
    evTX_RS485          ,Tx_Rs485StateMachine          ,stTx_Rs485,
    evTX2RX_RS485       ,Tx2rx_Rs485StateMachine       ,stRx_Rs485,
};

//******************************************************************************
// Declaring Functions
//******************************************************************************
void Ini_Rs485StateMachine(void)        // Inicializacion Registros y variables
{
    // Registros vinculados al EUSART:
    //                               TXSTA,RCSTA,TXREG,RCREG,SPBRGH,SPBRG,PIE1,IPR1

    // Define registro BAUDCON
    BAUDCON=0b01001000;

    // Define registros de Reception y Transmission
    TXSTA=0b00000100;  // 8 bits, transmisión No habilitada, asíncrono, alta velocidad (8N1)
    RCSTA=0b10010000;  // Habilitado el USART PIC, recepción 8 bits,  habilitada, asíncrono  

    // Define velocidad de comunicacion
    // Formula utilizada para el calculo Baudio=Fosc/[4*(n+1)] donde n es SPBRGH-SPBRG
    SPBRGH = read_eeprom(USERD_BR_SPBRGH);   // Carga dato de eeprom USER DEFAULT  
    SPBRG = read_eeprom(USERD_BR_SPBRG);     // Carga dato de eeprom USER DEFAULT
    
    // Control en el tráfico de comunicación Rx=0 Tx=1
    flag_RXoTX = 0x00;              // Utilizada para cambiar de estado en State Machine
    aux_flag_RXoTX = flag_RXoTX;    // Utilizada en st_monitoring para chequear estado de la comunicacion
    flag_data_present = 0x00;       // Utilizada para informar cuando hay un dato valido en Rx o Tx

    #ifndef COMPILAR_PLACA_PCC
        RS485_DE_RE = 0x00;         // Actua sobre el integrado utilizado para RS485                     
    #endif

    count_FERR=0x0000;              // Contador de Framing Error  
    count_OERR=0x0000;              // Contador de Overrun Error 

    stm_msg2statemachine(evRX_RS485, &rs485StateMachine);
    return;
}

void Rx_Rs485StateMachine(void)     // Estado Rx
{
    if(flag_RXoTX){             // Consulta si debe seguir en modo recepcion
        RCSTAbits.CREN = 0;     // Deshabilita Rx
        TXSTAbits.TXEN = 1;     // Habilita Tx
        flag_data_present = 0;  // Reset flag

        #ifndef COMPILAR_PLACA_PCC
            RS485_DE_RE = 1;        // Habilita el Circuito integrado de comunicaciones modo TX                   
        #endif

        stm_msg2statemachine(evRX2TX_RS485, &rs485StateMachine);
        return;
    }     

    if(PIR1bits.RCIF){          // Si el bit5 del registro PIR1 se ha puesto a 1 indica recepcion
        if(RCSTAbits.FERR)
            count_FERR++;       // Suma un error de Framing Error

        data_RS485 = RCREG;     // Devuelve el dato almacenado en el registro RCREG
        flag_data_present = 1;  // Set flag
    }
    else{
        if(RCSTAbits.OERR){
            RCSTAbits.CREN = 0; // Permite que se puedan seguir recibiendo char
            RCSTAbits.CREN = 1;

            count_OERR++;       // Suma un error de Overrun Error 
        }
    }

    stm_msg2statemachine(evRX_RS485, &rs485StateMachine);
    return;
}

void Rx2tx_Rs485StateMachine(void)  // Estado de transicion Rx a Tx                 
{
    stm_msg2statemachine(evTX_RS485, &rs485StateMachine);
    return;
}

void Tx_Rs485StateMachine(void)     // Estado Tx
{
    if(flag_RXoTX){
        if(flag_data_present){
            if(TXSTAbits.TRMT){         // Consulta si el registro TSR esta vacio
                TXREG = data_RS485;     // Cuando el el registro TSR está vacio se envia el caracter
                flag_data_present = 0;  // Reset flag
            }   
        }   
    } 
    else{
        if(TXSTAbits.TRMT){          // Mientras el registro TSR esté lleno espera que se trasmita el ultimo char
            TXSTAbits.TXEN = 0;      // Deshabilita Tx
            RCSTAbits.CREN = 1;      // Habilita Rx
            flag_data_present = 0;   // Reset flag

            #ifndef COMPILAR_PLACA_PCC
                RS485_DE_RE = 0;     // Habilita el Circuito integrado de comunicaciones modo RX                   
            #endif

            data_RS485 = RCREG;      // Utilizado para limpiar RCREG

            stm_msg2statemachine(evTX2RX_RS485, &rs485StateMachine);  // Pasa al siguiente estado
            return;
        }     
    }     

    stm_msg2statemachine(evTX_RS485, &rs485StateMachine);       // continua en el miso estado esperando que se complete a Trasmision
    return;
}

void Tx2rx_Rs485StateMachine(void)      // Estado de transicion Tx a Rx                         
{
    stm_msg2statemachine(evRX_RS485, &rs485StateMachine);
    return;
}
