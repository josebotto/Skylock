/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   aba_wie.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 27 de diciembre de 2017, 18:38
 * 
 * Log:
 *      - Code taken from plate ML02 version 5.0.
 *      - Logic decoding of reading pulses format ABA Track II and
 *        Wiegand 26, 35 and 37 (ITK format I15101 or H10304).
 *      - When commented on the line of #define H10304 in aba_wie.c
 *        can read card 37-bit HID format ITK I15101.
 *        Otherwise, 37-bit card readings can be made
 *        HID Wiegand 37 H10304.
 *      - Readings with error are reported. 
 * 
 * =====================================================================
 */


#ifndef ABA_WIE_H
#define	ABA_WIE_H

    /********************************************************
        Data types and constants
    ********************************************************/
    // Tiempo a esperar (en TimerB Clocks) despues de un bit para detectar el fin de los pulsos
    #define			LECTOUT			2			// Equivale a 2 * 50 mS = 100 mS (5 bits)

    #define			MINBITS			10			// Minima cant de bits para decodificar
                                                // Modificada para version 5.1 Visualizacion de lecturas erroneas

    #define			WIE_MINBITS		26			// Minima cant de bits para Wiegand

    #define			ABA_SS			0x0B		// ABA Start Sentinnel
    #define			ABA_ES			0x0F		// ABA End Sentinnel

    #define			ABA_SS_RAW		0xC0		// SS usado para reportar errores

    #define			NUM_READERS			2
    #define			MAX_TARJ_SZ			16
    #define			TARBUF_SZ			(MAX_TARJ_SZ + 1)

    #define TARBUF_BANK	            // Buffer para la traduccion ASCII

    typedef TARBUF_BANK char cTarBuf;
    typedef cTarBuf *pTarBuf;
    volatile cTarBuf			TarBuf[NUM_READERS][TARBUF_SZ];   

    // Estructura usada para el almacenamiento de los datos necesarios
    // para la decodificación de los pulsos de lectora
    typedef CARDBUF_BANK struct {
        TIntCtrl	PulseBuf;				// Estructura de control de pulsos embebida
                                            // Sirve para filtrar interrupciones espurias
        byte		DataMask;				// Mascara del bit de DATA (ABA) o D0 (Wiegand)
        byte		ClckMask;				// Mascara del bit de CLOCK (ABA) o D1 (Wiegand)

        byte		BitCtr;					// Contador de bits leídos (Wiegand)

        cTarBuf	 	*TarBuf;				// Ptr al buffer de decodificación. Como se decodifica
        byte		TarBufSz;				// On-the-fly, los buffers no se pueden compartir entre lectores
        byte		CharCtr;				// Contador de caracteres de decodificación

        byte		AbaPreBuf;				// Buffer ABA

        byte		AbaLrc;

        byte		WieBuf[8];				// Buffer Wiegand (64 bits)

        byte		Flags;					// Flags de configuración 
            #define	RDR_ENABLE_ABATK2		0x01
            #define	RDR_ENABLE_WIEGAND		0x02
            #define	RDR_STORE_IN_BCD		0x04
            #define	RDR_FAKE_1ST_ABA_BIT	0x08

            #define	ABA_CHECK_LRC			0x10	// Check LRC
            #define	ABA_SHOW_SS				0x20	// Show ES/SS in result

            #define	RDR_DONT_CLRBUF_AFTER	0x40	// Don´t clear BitBuf after decode
            #define	RDR_POLLED_READ    		0x80	// La entrada es polleada (no ints)

        byte		State;					// Flags de operacion
            #define	RDR_ST_ENABLE_ABATK2	RDR_ENABLE_ABATK2
            #define	RDR_ST_ENABLE_WIEGAND	RDR_ENABLE_WIEGAND
            #define	RDR_ENABLE_BITS			(RDR_ENABLE_ABATK2 | RDR_ENABLE_WIEGAND)

            #define	RDR_ST_LOCKED			0x10
            #define	RDR_ST_ABA_SS_FOUND		0x20
            #define	RDR_ST_ABA_ES_FOUND		0x40
            #define	RDR_ST_BIT_DETECTED		0x80

        signed char Rslt;					// resultado de la decodificación
            #define		RDR_NONE				0
            #define		RDR_ABA_READ_OK			1
            #define		RDR_WIEGAND_READ_OK  	2

            #define		RDR_ABA_NO_SS_FOUND		-1
            #define		RDR_ABA_NO_ES_FOUND		-2
            #define		RDR_ABA_PARITY_ERR		-3
            #define		RDR_ABA_LRC_ERR			-4

            #define		RDR_NOTENOUGHBITS		-5
            #define		RDR_WIE_UNK_FMT			-6
            #define		RDR_WIE_PARITY_ERR		-7

            #define		RDR_WIE_POSSIBLY		-8

    } TCardBuf;

    volatile TCardBuf			CardBuf[NUM_READERS];        

    typedef struct {
        byte	ptyBit,		// Pos del parity bit. el bit 7 indica: 0: Even, 1: Odd
                    #define	WIE_PTY_EVEN	0
                    #define	WIE_PTY_ODD		0x80
                ptyPos,     // Posicion
                ptyLen,		// Longitud
                   #define	BIT_MUST_BE		0
                ptyInc;		// Hi Nibble: Incremento entre muestras
                            // Lo Nibble: Bits válidos de la muestra
    } TParityCheck;

    #define	LAST_PTY_DEF	{0xFF, 0xFF, 0xFF, 0xFF}

    typedef struct {
        byte		bitCnt;
        byte		fcPos,		// En bits
                    fcLen,
                    fcDigits;
        byte		idPos,
                    idLen,
                    idDigits;
       const TParityCheck	*ptyCheck;
    } TWiegandFmt;

    /********************************************************
        Prototypes published
    ********************************************************/
    extern byte	WieFmtsDisabled;

    #define RDR_FLAGS   RDR_ENABLE_ABATK2 | RDR_ENABLE_WIEGAND | ABA_CHECK_LRC | ABA_SHOW_SS | RDR_STORE_IN_BCD

    int RDRInit(byte chnl, byte ClkMask, byte DataMask, byte Flags, volatile TCardBuf *Buf);
    int RDRStatus(volatile TCardBuf *Buf);
    int RDRUnlock(volatile TCardBuf *Buf);
    int WiegandDecode(volatile TCardBuf *Buf);

#endif	/* ABA_WIE_H */
