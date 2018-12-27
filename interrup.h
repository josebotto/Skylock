/*
 * =====================================================================
 * Skylock Project
 * Software version 1.0
 * 
 * File:   interrup.h
 * Author: JBotto
 * 
 * Digicard. 
 * Buenos Aires. Argentina
 * 
 * Created on 24 de agosto de 2017, 15:01
 * 
 * Log:
 *     Interrupt Vector.  
 *  
 * =====================================================================
 */


#ifndef INTERRUP_H
#define	INTERRUP_H

    // Declarando funciones
    void interrupt ISR(void);
    void interrupt_enable(unsigned char mode);

#endif	/* INTERRUP_H */
