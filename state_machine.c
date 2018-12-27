#include <xc.h>
#include "def_hardware.h"
#include "def_global_var.h"
#include "state_machine.h"
#include "st_monitoring.h"
#include "st_inputs_digital.h"
#include "st_inputs_analogical.h"
#include "st_outputs.h"
#include "st_readers.h"
#include "st_rs485.h"
#include "st_timers.h"
#include "st_osdp.h"


static char 	Buffer[MAX_FIFO_LENGTH];   // Array usado de buffer para almacenar datos de las State Machine
static char 	Head, Tail;          	   // Punteros para lectura-escritura
                                           // Head -> Puntero a proxima posicion a extraer
                                           // Tail -> Puntero a proxima posicion a insertar

void stm_init_fifo(void)        // Inicializar punteros de cola y cargar máquinas de estado
{
    Head = 0x00;
    Tail = 0x00;

    // Define el estado de inicio de cada FSM. 
    timersStateMachine = stInit_Timers;
    monitoringStateMachine = stInit_Monitoring;
    inputs_analog_StateMachine = stInit_Inputs_analog;
    inputsStateMachine = stInit_Inputs;
    outputsStateMachine = stInit_Outputs;
    readersStateMachine = stInit_Readers;
    osdpStateMachine = stInit_Osdp;
    rs485StateMachine = stInit_Rs485;
    
    // Carga en la cola circular la dirección de cada FSM y su evento inicial (cero)
    stm_msg2statemachine( evINI_TIMERS, &timersStateMachine);
    stm_msg2statemachine( evINI_MONITORING, &monitoringStateMachine);
    stm_msg2statemachine( evINI_INPUTS_ANALOG, &inputs_analog_StateMachine);
    stm_msg2statemachine( evINI_INPUTS, &inputsStateMachine);
    stm_msg2statemachine( evINI_OUTPUTS, &outputsStateMachine);
    stm_msg2statemachine( evINI_READERS, &readersStateMachine);
    stm_msg2statemachine( evINI_OSDP, &osdpStateMachine);
    stm_msg2statemachine( evINI_RS485, &rs485StateMachine);
}

unsigned char stm_read_fifo(void)       // Leer datos de la cola           
{
	auto char	nh;

	nh = Buffer[Head];          // Lee Buffer
	Head = NEXT(Head);			// Apunta a la siguiente posicion
 	return nh;					// Retorna el valor obtenido en el Buffer
}

void stm_write_fifo(char Data)  // Escribe un dato en la cola			
{
	Buffer[Tail] = Data;        // Escribe Buffer
	Tail = NEXT(Tail);          // Apunta a la siguiente posicion
}

void stm_msg2statemachine(char Event, PStateMachine Machine)    // Función para enviar un mensaje (código de evento) a una máquina de estado
{
	stm_write_fifo((char)Machine);
	stm_write_fifo(Event);
}

void state_machine(void)        // Motor para máquinas de estado finito(FSM)
{
	char				Event;
	PStateMachine 		AState;     // Apunta a la variable de cada State Machine 
	PStateList			LState;     // Apunta al array de estados 

	AState = (PStateMachine) stm_read_fifo();
	Event  = stm_read_fifo();
    
	LState = *AState;
    
	for(; LState->Event != Event; 	// Mientras el evento del estado actual no sea el evento de entrada...
		LState++);					// paso al estado siguiente...	
    
	*AState = LState->Next_State;	// Setea el nuevo estado de la maquina
	
	if(LState->Func)
		LState->Func();				// Se ejecuta la funcion de cambio de estado si el ptr no es NULL
	return;
}
