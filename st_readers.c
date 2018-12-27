#include <xc.h>
#include "def_global_var.h"
#include "state_machine.h"
#include "read_pulse.h"
#include "aba_wie.h"
#include "st_readers.h"


/*************************************************************************/
// States of the state machine
/*************************************************************************/
TStateList	stInit_Readers[] = {
    evINI_READERS   ,Ini_ReadersStateMachine    ,stWorking_Readers,
};

TStateList	stWorking_Readers[] = {
    evCHECKCARD_READERS     ,Checkcard_ReadersStateMachine      ,stWorking_Readers,
    evERRORDATA_READERS     ,Errordata_ReadersStateMachine      ,stWorking_Readers,
    evPREPAREDATA_READERS   ,Preparedata_ReadersStateMachine    ,stUnlock_Readers,
};

TStateList	stUnlock_Readers[] = {
    evUNLOCK_READERS    ,Unlock_ReadersStateMachine     ,stWorking_Readers,
};

//******************************************************************************
// Declaring Functions
//******************************************************************************
void Ini_ReadersStateMachine(void)       // Inicializa esta maquina de estado
{
    auto int	i;
	auto volatile TCardBuf *Buf;
	nCardBuf = 0;
	
	// Habilita las interrupciones de captura de pulsos de lectora
    INT0IF = OFF;  // Clear flag INTO
    INT1IF = OFF;  // Clear flag INT1
    INT2IF = OFF;  // Clear flag INT2
    CCP2IF = OFF;  // Clear flag CCP2

    CCP2M3 = OFF;	// Capture mode
	CCP2M2 = ON;	// Every ...
	CCP2M1 = OFF;	// Falling...
	CCP2M0 = OFF;	// edge

    INT0IE = ON;   // Enable INTO
    INT1IE = ON;   // Enable INT1
    INT2IE = ON;   // Enable INT2
    CCP2IE = ON;   // Enable CCP2

	memset_b3(CardBuf, 0, sizeof(CardBuf));
	memset_b3(TarBuf, 0, sizeof(TarBuf));
	for(i = 0; i < NUM_READERS; i++) {
        Buf = &CardBuf[i];
		Buf->TarBuf = (cTarBuf *) TarBuf[i];
		Buf->TarBufSz = TARBUF_SZ;
		RDRInit(i, RdrClkMask[i], RdrDataMask[i], RDR_FLAGS, Buf); 
	}

    stm_msg2statemachine(evCHECKCARD_READERS, &readersStateMachine);
    return;
}

void Checkcard_ReadersStateMachine(void)        // Chequea si hay registraciones
{
 	auto int    r;
	auto volatile TCardBuf *Buf = &CardBuf[nCardBuf];
	
	if((r = RDRStatus(Buf)) != RDR_NONE) {
		if(r == RDR_WIE_POSSIBLY)
			r = WiegandDecode(Buf);

		if(r < 0)                              // Si se cumple esta condicin estamos en presencia de un error
			stm_msg2statemachine(evERRORDATA_READERS, &readersStateMachine);
		else 
			stm_msg2statemachine(evPREPAREDATA_READERS, &readersStateMachine);
    
        return;
	}		

    nCardBuf ^= 1;		// Si está en 1 pasa a 0 y viceversa.

    stm_msg2statemachine(evCHECKCARD_READERS, &readersStateMachine);
    return;
}

void Errordata_ReadersStateMachine(void)        // Se detecto un error y se reportara lo que se tiene
{
 	auto volatile TCardBuf *Buf = &CardBuf[nCardBuf];
	auto byte		i, b;
		
	Buf->TarBuf[0] = ABA_SS_RAW;
	Buf->TarBuf[1] = Buf->BitCtr;
	Buf->TarBuf[2] = Buf->Rslt;           
	b = 1 + ((Buf->BitCtr - 1) >> 3); 
		
	for(i = 0; i < b; i++)
		Buf->TarBuf[3 + i] = Buf->WieBuf[i];   

	Buf->TarBuf[3 + b] = 0xFF;		        

    stm_msg2statemachine(evPREPAREDATA_READERS, &readersStateMachine);
    return;
}
	
void Preparedata_ReadersStateMachine(void)       // Se almacena la registracion en un buffer intermedio 
{                                                // para que sea transmitido en la condicion de Poll
    char aux_data;
	char aux_for;
 	auto volatile TCardBuf *Buf = &CardBuf[nCardBuf];
    
	for(aux_for = 0; aux_for < READER_BUF ; aux_for++){       // Almacena registracion en buffer intermedio
        readers_pcl[nCardBuf].value[aux_for] = Buf->TarBuf[aux_for]; 

        if(Buf->TarBuf[aux_for] == 0xFF)                      // Detecta el ultimo byte de la registracion recibida
            aux_for = READER_BUF ;
	}
    
    aux_data = readers_pcl[nCardBuf].status_in & REPORT_READER_CHANGE;       // Consulta si se debe reportar la registracion
    if(aux_data)
        readers_pcl[nCardBuf].status_in |= REPORT_READER_PD;                 // Reporta registracion
    else
        readers_pcl[nCardBuf].status_in &= ~REPORT_READER_PD;                // No reporta registracion
    
    stm_msg2statemachine(evUNLOCK_READERS, &readersStateMachine);
    return;
}
    
void Unlock_ReadersStateMachine(void)       // Se libera la entrada de lectora y se pasa a la otra entrada de lectora
{
                        // Deslockeo las rutinas de lectura de tarjeta
                        // Poner FF en la 1ra posic de TarBuf
	RDRUnlock(&CardBuf[nCardBuf]);		
	nCardBuf ^= 1;

    stm_msg2statemachine(evCHECKCARD_READERS, &readersStateMachine);
    return;
}
