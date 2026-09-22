/**
 *
 *  @file   rc5.c
 *  @date   21-09-2026
 *  @author Bart Gysens
 *
 *  @brief  RC-5 decoder implementation.
 *
 */

/* --------------------------- *
 * Project and system includes *
 * --------------------------- */
#include <xc.h>
#include "rc5.h"



/* ------------------- *
 * Macros and typedefs *
 * ------------------- */

/*-- rc5 command bit size --*/
#define RC5_CMD_BITSIZE 14

/*-- rc5 time constants (adjust if timer clock is not 62.5KHz!) --*/
typedef enum
{
    RC5_TIME_TOOSHORT = 28
,   RC5_TIME_BOUNDARY = 83
,   RC5_TIME_TOOLONG = 139
} RC5_TIME;

/*-- rc5 state enumeration --*/
typedef enum
{
    RC5_STATE_IDLE      /* nothing to do */
,   RC5_STATE_RECV      /* receiving bits */
} RC5_STATE;



/* ----------- *
 * Global data *
 * ----------- */
static RC5_STATE _state;
static uint16_t _bitValues;
static uint8_t _bitCount;
static uint8_t _shortPulseCount;
static uint8_t _previousBitValue;
volatile bool _rc5CommandAvailable = false;
volatile RC5_COMMAND _rc5Command;



/* ------------------------ *
 * Function Implementations *
 * ------------------------ */

void rc5Setup( void )
{

    /*-- setup timer2 : FOSC/4 and 1:128 prescaler = 32MHz / 4 / 128 = 62500 Hz --*/
    T2CONbits.ON = 0;               /* disable timer */
    T2CONbits.OUTPS = 0b0000;       /* set postscaler to 1:1 */
    T2CONbits.CKPS = 0b111;         /* set prescaler to 1:128 */    
    T2CLKCONbits.CS = 0b0001;       /* set clock as FOSC/4 */    
    T2HLTbits.MODE = 0b0000;        /* set free running software mode */
    T2HLTbits.CKSYNC = 1;           /* set sync clock */
    T2HLTbits.CKPOL = 0;            /* set rising edge of clock */
    T2HLTbits.PSYNC = 1;            /* sync prescaler to FOSC/4 */
    T2RSTbits.RSEL = 0b0000;        /* T2INPPS external reset (= default) */
    T2PR = 0b11111111;              /* set timer reload value */
    PIE4bits.TMR2IE = 0;            /* disable timer2 interrupt */
    T2CONbits.ON = 1;               /* enable timer */
    INTCONbits.PEIE = 1;            /* enable peripheral interrupts */

    /*-- initialize RA2 pin as IR input pin --*/
    PORTAbits.RA2 = 0;              /* reset pin value */
    LATAbits.LATA2 = 0;             /* reset pin latch */
    ANSELAbits.ANSA2 = 0;           /* set as digital io */    
    WPUAbits.WPUA2 = 1;             /* enable weak pull-up */
    INLVLAbits.INLVLA2 = 1;         /* enable schmitt-trigger input */
    TRISAbits.TRISA2 = 1;           /* set as digital input */   
    IOCAFbits.IOCAF2 = 0;           /* reset pin interrupt on change flag */
    IOCAPbits.IOCAP2 = 0;           /* disable interrupt on change rising edge */
    IOCANbits.IOCAN2 = 1;           /* enable interrupt on change falling edge */
    PIE0bits.IOCIE = 1;             /* enable interrupt on change */

    /*-- initialize state machine --*/
    _state = RC5_STATE_IDLE;
}



void rc5IsrPin()
{
    /*-- clear pin interrupt flag --*/
    IOCAFbits.IOCAF2 = 0;
    
    /*-- flip edge trigger --*/
    IOCAPbits.IOCAP2 = 1 - IOCAPbits.IOCAP2;
    IOCANbits.IOCAN2 = 1 - IOCANbits.IOCAN2;
    
    /*-- get time since last pin interrupt --*/
    uint8_t time = TMR2;    /* read timer */ 
    TMR2 = 0b00000000;      /* clear timer 2 value register */

    /*-- handle state-machine --*/
    switch ( _state )
    {
        case RC5_STATE_IDLE:
        {
            /*-- first bit received --*/
            _state = RC5_STATE_RECV;
            _bitValues = 0x0001;
            _bitCount = 1;
            _shortPulseCount = 0;
            _previousBitValue = 1;

            /*-- enable timer overflow interrupt --*/
            PIR4bits.TMR2IF = 0;    /* reset timer interrupt flag */
            PIE4bits.TMR2IE = 1;    /* enable timer2 interrupt */
            break;
        }

        case RC5_STATE_RECV:
        {
            /*-- check if it is a valid pulse duration --*/
            if (   ( time > RC5_TIME_TOOSHORT )
                && ( time < RC5_TIME_TOOLONG  ) )
            {
                /*-- check if it is a short or a long pulse --*/
                if ( time < RC5_TIME_BOUNDARY )
                {
                    /*-- short pulse detected --*/
                    if ( ++_shortPulseCount == 2 )
                    {
                        _bitValues <<= 1;
                        _bitValues |= _previousBitValue;
                        _bitCount++;
                        _shortPulseCount = 0;
                    }
                }
                else
                {
                    /*-- long pulse detected --*/
                    _previousBitValue = 1 - _previousBitValue;
                    _bitValues <<= 1;
                    _bitValues |= _previousBitValue;
                    _bitCount++;
                    _shortPulseCount = 0;
                }

                if ( _bitCount == RC5_CMD_BITSIZE )
                {                    
                    /*-- next edge interrupt is on falling edge --*/
                    IOCAPbits.IOCAP2 = 0;   /* disable interrupt on change rising edge */
                    IOCANbits.IOCAN2 = 1;   /* enable interrupt on change falling edge */
                    PIR4bits.TMR2IF = 0;    /* disable timer2 interrupt */
                    
                    /*-- fill in command data --*/
                    _rc5Command.toggle = ( ( _bitValues >> 11 ) & 0x01 ) ? 1 : 0;
                    _rc5Command.address = ( ( _bitValues >> 6 ) & 0x1F );
                    _rc5Command.command = ( _bitValues & 0x3F );
                    _rc5Command.command |= ( ( _bitValues & 0x1000 ) ? 0x00 : 0x40 );

                    /*-- all done --*/
                    _state = RC5_STATE_IDLE;
                    _rc5CommandAvailable = true;
                }
            }
            else /* invalid pulse timing */
            {
                /*-- next edge interrupt is on falling edge --*/
                IOCAPbits.IOCAP2 = 0;   /* disable interrupt on change rising edge */
                IOCANbits.IOCAN2 = 1;   /* enable interrupt on change falling edge */
                PIR4bits.TMR2IF = 0;    /* disable timer2 interrupt */

                /*-- not a valid pulse, goto idle state --*/
                _state = RC5_STATE_IDLE;
            }
            break;
        }

        default:
        {
            /*-- do nothing --*/
            break;
        }
    }
}



void rc5IsrTimer( void )
{      
//    if (_state == RC5_STATE_RECV)
    {
        /*-- something is wrong, start all over --*/
        IOCAPbits.IOCAP2 = 0;   /* disable interrupt on change rising edge */
        IOCANbits.IOCAN2 = 1;   /* enable interrupt on change falling edge */
        PIR4bits.TMR2IF = 0;    /* disable timer2 interrupt */

        /*-- reset state machine --*/
        _state = RC5_STATE_IDLE;
    }
}
