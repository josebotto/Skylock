#include "def_global_var.h"
#include "read_pulse.h"


static CARDBUF_BANK volatile TIntCtrl * CARDBUF_BANK IntVecs[MAXINPUTS];

/********************************************************
	Definition of functions
********************************************************/
void EnableInts(volatile CARDBUF_BANK TIntCtrl *CtrlBuf, byte BitMask, byte enable)
// Llamar con interrupciones deshabilitadas !!!
{
	if(enable) 
		CtrlBuf->BitMask |= BitMask;
	else 
		CtrlBuf->BitMask &= ~BitMask;
}

int InitPulseInput(byte chnl, byte BitMask, byte TimeOut, TDataFunc DataFunc,
						 volatile CARDBUF_BANK TIntCtrl *CtrlBuf)
{

	if(chnl > MAXINPUTS)
		return PUL_INVALIDINPUT;

	IntVecs[chnl] = CtrlBuf;

	CtrlBuf->Input = chnl;
	CtrlBuf->ToutCtr = TOUT_HELD;
	CtrlBuf->PulseTout = TimeOut;

	CtrlBuf->DataFunc = DataFunc;

	CtrlBuf->BitMask = BitMask;

  	return PUL_OK;
}

void TrackBit(byte data, volatile CARDBUF_BANK TIntCtrl *icntrl)
{
	auto int	bit_rslt;
	
	if(bit_rslt = icntrl->DataFunc(icntrl, data)) 
		DisableInt(icntrl, bit_rslt);
		
	icntrl->ToutCtr = 0;
}

void * memset_b3(CARDBUF_BANK void *s, char c, int n)
{
	auto CARDBUF_BANK char *s3;
	
	for(s3 = s; n; n--)
		*s3++ = c;

	return s3;
}

/********************************************************
	Interruption Routine
********************************************************/
void ReadPulse_isr(byte data)
{
	auto int    i;

	for(i = 0; i < MAXINPUTS; i++) 
		if(IntVecs[i])
			TrackBit(data, IntVecs[i]);	
}

void UpdCounter(volatile CARDBUF_BANK TIntCtrl *icntrl)
{
	if( !(icntrl->ToutCtr & TOUT_HELD) ) {
		if(icntrl->ToutCtr != icntrl->PulseTout) 
			icntrl->ToutCtr++;
		else 
			icntrl->ToutCtr = TIME_OUT;
	}
}

/********************************************************
	Timing Routine
********************************************************/
int ReadPulse_tick_ii0(int data)
{
	if(IntVecs[0]) 
		UpdCounter(IntVecs[0]);
	if(IntVecs[1]) 
		UpdCounter(IntVecs[1]);
	return 0;
}
