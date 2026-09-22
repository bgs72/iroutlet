/**
 *
 *  @file   Interrupts.c
 *  @date   21-09-2026
 *  @author Bart Gysens
 *
 *  @brief  Interrupt routines.
 */

/* --------------------------- *
 * Project and system includes *
 * --------------------------- */
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "rc5.h"


/**
 * 
 *  Main interrupt service routine.
 * 
 */
void __interrupt() isr(void)
{
    if ( IOCAFbits.IOCAF2 )
    {
        /*-- RA2 interrupt on change --*/
        rc5IsrPin(); 
        IOCAFbits.IOCAF2 = 0;
    }        

    if ( PIR4bits.TMR2IF )
    {                
        /*-- timer 2 overflow interrupt flag --*/        
        rc5IsrTimer();        
        PIR4bits.TMR2IF = 0;        
    }    
}
