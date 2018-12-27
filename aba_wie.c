#include "def_global_var.h"
#include "read_pulse.h"
#include "aba_wie.h"


//#define H10304    // Activa la lectura del Formato HID ITK I15101 y desactiva el formato HID H10304

/********************************************************
	Variables locales al módulo
********************************************************/
// Tabla que sirve para el cálculo rápido del bit de paridad
//							    0     1     2     3     4     5     6     7
const byte	ParityTbl[16] = {0x10, 0x00, 0x00, 0x10, 0x00, 0x10, 0x10, 0x00,
//						        8     9     A     B     C     D     E     F
							 0x00, 0x10, 0x10, 0x00, 0x10, 0x00, 0x00, 0x10 };

const	TParityCheck ptyCheck26[] = {
		{ 0 | WIE_PTY_EVEN,  1, 12, 0x11},
   	   	{25 | WIE_PTY_ODD,  13, 12, 0x11},
        LAST_PTY_DEF
};

const	TParityCheck ptyCheck35[] = {
		{ 0 | WIE_PTY_ODD,   1, 34, 0x11},
        { 1 | WIE_PTY_EVEN,  2, 33, 0x32},
        {34 | WIE_PTY_ODD,   1, 33, 0x32},
        LAST_PTY_DEF
};

#ifndef H10304
const	TParityCheck ptyCheck37[] = {               // HID Wiegand 37 ITK I15101
		{ 0 ,               0, BIT_MUST_BE, 1},
		{ 1 | WIE_PTY_EVEN, 2, 17, 0x11},
        {36 | WIE_PTY_ODD, 19, 17, 0x11},
        LAST_PTY_DEF
};
#else
const	TParityCheck ptyCheck37[] = {               // HID Wiegand 37 H10304
		{ 0 | WIE_PTY_EVEN,  1, 18, 0x11},
   	   	{36 | WIE_PTY_ODD,  18, 18, 0x11},
        LAST_PTY_DEF
};
#endif

const TWiegandFmt	WiegandFormats[] = {
/*
; La decodificación de Wiegand/26 es :
; 	Bit
; 	1			Paridad de los bits 2..13 (Even)
; 	2..9		Facility Code (8 bits)
; 	10..25	ID Code (16 bits)
; 	26			Paridad de los bits 14..25 (Odd)
*/
	{ 26, 1, 8, 3, 9, 16, 5, ptyCheck26 },		// Wiegand 26

/*
; La decodificación de Wiegand/35 (HID Corp 1000) es:
; 	Bit
; 	1			Paridad 'Odd' de los bits 2..35
;	2			Paridad 'Even' de los bits 3-4, 6-7, 9-10, ... 33-34
; 	3..14		Company Code (12 bits)
; 	15..34	ID Code (20 bits)
; 	35			Paridad 'Odd' de los bits 2-3, 5-6, 8-9, ... 32-33
*/
	{ 35, 2, 12, 4, 14, 20, 6, ptyCheck35 }, 	// Wiegand 35 Corp 1000

#ifndef H10304
/*
$DecodeWie37:		; Implementa decodificacion ITK I15101
;
;0         1         2         3
;0123456789012345678901234567890123456
;1PFFFFFFFFFFFFFFFFFIIIIIIIIIIIIIIIIIP
;.Exxxxxxxxxxxxxxxxx..................
;...................xxxxxxxxxxxxxxxxxO
;
;Donde:
;	1:	Siempre en 1 (uno)
;	P:	Paridad Par de los bits 2 - 18
;	F:	Facility Code 17 bits (0-131.071)
;	I:	ID Code 17 bits (0-131.071)
;	O:	Paridad Impar de los bits 19-35
;
*/
	{ 37, 2, 17, 5, 19, 17, 5, ptyCheck37 }		// Wiegand 37 ITK I15101
#else
/*
$DecodeWie37:		; Implementa decodificacion H10304
;
;0         1         2         3
;0123456789012345678901234567890123456
;PFFFFFFFFFFFFFFFFIIIIIIIIIIIIIIIIIIIP
;Exxxxxxxxxxxxxxxxxx.................
;..................xxxxxxxxxxxxxxxxxxO
;
;Donde:
;	PE:	Paridad Impar de los bits  1-18
;	F:	Facility Code 16 bits (0-65535)
;	I:	ID Code 19 bits (0-524287)
;	PO:	Paridad Par de los bits 18-35
;
*/
	{ 37, 1, 16, 5, 17, 19, 6, ptyCheck37 }		// Wiegand 37 H10304
#endif
};

/********************************************************
	Local prototypes
********************************************************/
dword RDRGetBits(volatile TCardBuf *Buf, byte BitPos, byte BitsLen);
#define RDRGetBit(Buf, BitPos)		((byte) RDRGetBits(Buf, BitPos, 1))

int _RDRReadBit(void CARDBUF_BANK *ptr, byte data); 				// Rutina de captura de bits en assembler

byte	WieFmtsDisabled;

/********************************************************
	Definition of public functions
********************************************************/
int RDRInit(byte chnl, byte ClkMask, byte DataMask, byte Flags, volatile TCardBuf *Buf)
{
	auto int	r;

	WieFmtsDisabled = 0;
	
	Buf->DataMask = DataMask;
	Buf->ClckMask = ClkMask;
	Buf->Flags = Flags;

	r = InitPulseInput(chnl, 
						Flags & RDR_ENABLE_WIEGAND ? ClkMask | DataMask : ClkMask, 
						LECTOUT,
					  	(TDataFunc)_RDRReadBit,
	 					&Buf->PulseBuf);

    RDRUnlock(Buf);
	return r;
}

int RDRStatus(volatile TCardBuf *Buf)
{
	if(Buf->Rslt == RDR_NONE) {
		if(Buf->PulseBuf.ToutCtr == TIME_OUT) {
		// Timeout sin decodificación ni deteccion de error
		// Puede ser una lectura Wiegand o pulsos espurios
			if(Buf->State & RDR_ST_ENABLE_WIEGAND && Buf->BitCtr >= WIE_MINBITS) 
				Buf->Rslt = RDR_WIE_POSSIBLY;
			else
				// Si son pulsos espurios reseteo la logica
				RDRUnlock(Buf);
		}
	}
	return Buf->Rslt;		
}

int RDRUnlock(volatile TCardBuf *Buf)
{
	auto byte	eints;
	
	eints = Buf->ClckMask;
	
	if(Buf->Flags & RDR_ENABLE_WIEGAND)
		eints |= Buf->DataMask;

	Buf->TarBuf[0] = 0xFF;
	Buf->BitCtr = 0;
	Buf->AbaPreBuf = 0x00;
	Buf->CharCtr = 0;

	Buf->PulseBuf.ToutCtr = TOUT_HELD;		
	Buf->State = Buf->Flags & RDR_ENABLE_BITS;
	Buf->Rslt = RDR_NONE;

	EnableInt(&Buf->PulseBuf, eints);
}

// ALLWAYS_STORE_IN_BCD
#define StoreDigit(_Buf, _c)									\
{																\
	auto byte NibCtr;											\
																\
	NibCtr = (_Buf)->CharCtr / 2;								\
																\
	if((_Buf)->CharCtr & 1)										\
		(_Buf)->TarBuf[NibCtr] &= (0xF0 | (_c)); 				\
	else														\
		(_Buf)->TarBuf[NibCtr] = (((_c) & 0x0F) << 4) | 0x0F;	\
																\
	(_Buf)->CharCtr++;											\
}

// ALLWAYS_STORE_IN_BCD
#define StoreEOB(_Buf)											\
{																\
	if(!((_Buf)->CharCtr & 1))									\
		(_Buf)->TarBuf[(_Buf)->CharCtr / 2] = 0xFF;				\
																\
	(_Buf)->CharCtr++;											\
}

/*******************************************************
 Bit capture routine
********************************************************/
int _RDRReadBit(void CARDBUF_BANK *ptr, byte data)
{
	auto word	nByte;
	auto byte	chng;
	auto byte	c, p;
	auto byte	rslt = 0;
	auto TCardBuf *Buf = ptr;
	auto CARDBUF_BANK byte *pState= &(Buf->State);

	if(!((*pState) & RDR_ST_LOCKED)) {
        
		chng = Buf->PulseBuf.BitMask;   //Jose    
    	if(!(data & chng))              // Deja pasar las entradas activas, evita que se  
            return rslt;                // produzca una lectura no deseada nula (1)

		if((*pState) & RDR_ST_ENABLE_WIEGAND) {
    //			chng = Buf->PulseBuf.BitMask;      // Jose  Reemplazada por (1)
			if((data & chng) == chng) {
				// Ambos bits en 1. Condición imposible en Wiegand
				// Deshabilitar lectura Wiegand de ahora en mas
				(*pState) &= ~RDR_ST_ENABLE_WIEGAND;
	
				// Deshabilita interrupciones por cambio en bit DATA/WD1
				rslt |= Buf->DataMask;	// No more Data ints
			}
			else {
				nByte = Buf->BitCtr >> 3;
				if(nByte < sizeof(Buf->WieBuf)) {
					c = data & Buf->ClckMask ? 1 : 0;		// Get DATA bit (WD1)
					Buf->WieBuf[nByte] = (Buf->WieBuf[nByte] << 1) | c;
					Buf->BitCtr++;
				}
			}
		}
	
		if(((*pState) & RDR_ST_ENABLE_ABATK2) && (data & Buf->ClckMask)) {
			c = Buf->DataMask & data ? 0x20 : 0;		// Aislo DATA bit
			if(Buf->Flags & RDR_FAKE_1ST_ABA_BIT && !((*pState) & RDR_ST_BIT_DETECTED))
				c |= 0x10;	// Invento bit en 1 previo a este

			Buf->AbaPreBuf = (Buf->AbaPreBuf >> 1) | c;
			c = (Buf->AbaPreBuf >> 1) & 0x1F;
			if((*pState) & RDR_ST_ABA_SS_FOUND) {
				// Si capture 5 bits
				if(Buf->AbaPreBuf & 0x01) {
					p = c & 0x10;		// Extraigo paridad
					c &= 0x0F;			// y el caracter
	
					if((*pState) & RDR_ST_ABA_ES_FOUND) {
						if(c == Buf->AbaLrc || !(Buf->Flags & ABA_CHECK_LRC)) 
							Buf->Rslt = RDR_ABA_READ_OK;
						else
							Buf->Rslt = RDR_ABA_LRC_ERR;
	
						StoreEOB(Buf);

						// Deshabilita interrupciones por cambio en bit CLK/WD0
						(*pState) &= ~RDR_ST_ENABLE_ABATK2;
						(*pState) |= RDR_ST_LOCKED;
						rslt |= Buf->ClckMask;					// No more CLK ints, wait 4 timeout
					}
					else {
						if(ParityTbl[c] != p) {
							Buf->Rslt = RDR_ABA_PARITY_ERR;
							(*pState) &= ~RDR_ST_ENABLE_ABATK2;
							(*pState) |= RDR_ST_LOCKED;
							rslt |= Buf->ClckMask;				// No more ints, wait 4 timeout
						}
						else {
							Buf->AbaLrc ^= c;						// Calculo LRC
							StoreDigit(Buf, c)//;
							if(c == ABA_ES)								
								(*pState) |= RDR_ST_ABA_ES_FOUND;
								
							Buf->AbaPreBuf = 0x20;
						}
					}
				}
			}
			else {
				if(c == ABA_SS) {
					(*pState) |= RDR_ST_ABA_SS_FOUND;
					Buf->AbaLrc = ABA_SS;
					StoreDigit(Buf, ABA_SS);
					Buf->AbaPreBuf = 0x20;
				}
			}
			
			(*pState) |= RDR_ST_BIT_DETECTED;
		}
	}
	return rslt;
}

dword RDRGetBits(volatile TCardBuf *Buf, byte BitPos, byte BitsLen)
{
	auto dword		rslt;
	auto word		nByte;
	auto byte		nBit, aBit;

	for(rslt = 0L; BitsLen > 0; BitPos++, BitsLen--) {
	   	rslt <<= 1;

    /* RDRGetBit Start */
		if(BitPos < Buf->BitCtr) {
			nByte = BitPos >> 3;
			nBit  = 7 - (BitPos & 7);
	
			aBit = ((Buf->WieBuf[nByte] >> nBit) & 1);
		}
		else
			aBit = 0xFF;
    /* RDRGetBit End */
	   	rslt |= aBit;
   }

	return rslt;
}

/**********************************************************************
				WIEGAND ROUTINES
**********************************************************************/
static const dword Divisors[] = {	1L,			
								 	10L,		
									100L,		
									1000L,		
									10000L,		
									100000L,
									1000000L,
									10000000L,
									100000000L,
									1000000000L};

int itobanked_a(volatile TCardBuf *Buf, dword i, byte places)
{
	auto byte	c, p;

	// Calculo el numero minimo de lugares necesarios para representar i
	for(p = places; i >= Divisors[p] ;p++) ;	

	// Elimino los digitos superiores de i no representables en 'places'
	for(; p > places; p--) 
		for(; i >= Divisors[p-1] ;i -= Divisors[p-1]) ;

	// Aqui, p = places
	for(; p > 0; p--) {
		if(p > 1) {
			for(c = 0; i >= Divisors[p-1] ;c++)
				i -= Divisors[p-1];
		}
		else
			c = i;

		StoreDigit(Buf, c);
	}

	return places;
}

int WiegandDecode(volatile TCardBuf *Buf)
{
	auto byte	pwf;
	auto int	i;
	auto dword	id;

	if (Buf->BitCtr >= WIE_MINBITS) {
		// Primero ajusto a izq los bits en el ultimo byte (imcompleto)
		if(i = Buf->BitCtr & 7) {
			i = 8 - i;
			Buf->WieBuf[Buf->BitCtr >> 3] <<= i;
		}

		for(i = 0, pwf = 1; i < (int)(sizeof(WiegandFormats) / sizeof(TWiegandFmt)); i++, pwf <<= 1) {
	      	if(Buf->BitCtr == WiegandFormats[i].bitCnt && (pwf & WieFmtsDisabled) == 0) {
    /** Start WieCheckParity */
				int rslt = 1;
				const TParityCheck *ptychk = WiegandFormats[i].ptyCheck; 
				{
					auto byte	pos, top, inc, cnt, pb;
					auto byte	pty;
				
					for(; ptychk->ptyBit != 0xFF; ptychk++) {
						if(ptychk->ptyLen == BIT_MUST_BE)
							pty = ptychk->ptyInc;
						else {
							pos = ptychk->ptyPos;
							top = pos + ptychk->ptyLen;
							inc = ptychk->ptyInc >> 4;
							cnt = ptychk->ptyInc & 0x0F;
							for(pty = 0; pos < top; pos += inc)
								for(pb = 0; pb < cnt; pb++)
									pty += RDRGetBit(Buf, pos + pb);
				
							if(ptychk->ptyBit & WIE_PTY_ODD)
								pty ^= 1;
						}
				
						pty &= 1;
				
						if(pty != RDRGetBit(Buf, ptychk->ptyBit & 0x7F))
							rslt = 0;
					}
				}

	         	if(rslt) {
    /** End WieCheckParity */
					StoreDigit(Buf, ABA_SS);

	            	id = RDRGetBits(Buf, WiegandFormats[i].fcPos, WiegandFormats[i].fcLen);
					itobanked_a(Buf, id, WiegandFormats[i].fcDigits);

	            	id = RDRGetBits(Buf, WiegandFormats[i].idPos, WiegandFormats[i].idLen);
					itobanked_a(Buf, id, WiegandFormats[i].idDigits);

					StoreDigit(Buf, ABA_ES);

					StoreEOB(Buf);
		 	 	   	return (Buf->Rslt = RDR_WIEGAND_READ_OK);
				}
				else
		 	 	   	return (Buf->Rslt = RDR_WIE_PARITY_ERR);
         	}
      	}
		return (Buf->Rslt = RDR_WIE_UNK_FMT);
	}
	else
		return (Buf->Rslt = RDR_NOTENOUGHBITS);
}
