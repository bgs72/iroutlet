/**
 *
 *  @file   main.c
 *  @date   21-09-2026
 *  @author Bart Gysens
 *
 *  @brief  Main entry point.
 *
 */

/* --------------------------- *
 * Project and system includes *
 * --------------------------- */
#include <xc.h>
#include <stdbool.h>
#include "rc5.h"

/* ------------------------------ *
 * PIC16F15325 Configuration bits *
 * ------------------------------ */

/*-- CONFIG1 --*/
#pragma config FEXTOSC = OFF        // External Oscillator mode selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT32     // Power-up default value for COSC bits (HFINTOSC with OSCFRQ= 32 MHz and CDIV = 1:1)
#pragma config CLKOUTEN = OFF       // Clock Out Enable bit (CLKOUT function is disabled; i/o or oscillator function on OSC2)
#pragma config CSWEN = OFF          // Clock Switch Enable bit (The NOSC and NDIV bits cannot be changed by user software)
#pragma config FCMEN = OFF          // Fail-Safe Clock Monitor Enable bit (FSCM timer disabled)

/*-- CONFIG2 --*/
#pragma config MCLRE = ON           // Master Clear Enable bit (MCLR pin is Master Clear function)
#pragma config PWRTE = OFF          // Power-up Timer Enable bit (PWRT disabled)
#pragma config LPBOREN = OFF        // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = OFF          // Brown-out reset enable bits (Brown-out reset disabled)
#pragma config BORV = LO            // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices)
#pragma config ZCD = OFF            // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY = OFF        // Peripheral Pin Select one-way control (The PPSLOCK bit can be set and cleared repeatedly by software)
#pragma config STVREN = ON          // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

/*-- CONFIG3 --*/
#pragma config WDTCPS = WDTCPS_31   // WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF           // WDT operating mode (WDT Disabled, SWDTEN is ignored)
#pragma config WDTCWS = WDTCWS_7    // WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC          // WDT input clock selector (Software Control)

/*-- CONFIG4 --*/
#pragma config BBSIZE = BB512       // Boot Block Size Selection bits (512 words boot block size)
#pragma config BBEN = OFF           // Boot Block Enable bit (Boot Block disabled)
#pragma config SAFEN = OFF          // SAF Enable bit (SAF disabled)
#pragma config WRTAPP = OFF         // Application Block Write Protection bit (Application Block not write protected)
#pragma config WRTB = OFF           // Boot Block Write Protection bit (Boot Block not write protected)
#pragma config WRTC = OFF           // Configuration Register Write Protection bit (Configuration Register not write protected)
#pragma config WRTSAF = OFF         // Storage Area Flash Write Protection bit (SAF not write protected)
#pragma config LVP = OFF            // Low Voltage Programming Enable bit (High Voltage on MCLR/Vpp must be used for programming)

/*-- CONFIG5 --*/
#pragma config CP = OFF             // UserNVM Program memory code protection bit (UserNVM code protection disabled)


/* -------------------- *
 * Forward declarations *
 * -------------------- */
void _initialize(void);
void _execute(void);



/**
 *
 *  Main entry point.
 *
 */
void main(void)
{
    /*-- initialize i/o and peripherals --*/
    _initialize();

    /*-- execute application --*/
    _execute();
}



/**
 *
 *  Initialize ports, peripherals & interrupts.
 *
 */
void _initialize(void)
{
    di();
    
    /*-- disable conflicting peripherals --*/
    PMD4bits.UART1MD = 1;   /* EUSART1 disabled */
    PMD4bits.UART2MD = 1;   /* EUSART2 disabled */
    PMD4bits.MSSP1MD = 1;   /* MSSP1 disabled */    
    
    /*-- initialize IO pins --*/
    PORTA &= 0b11001111;     /* reset pin value */
    LATA &= 0b11001111;      /* reset pin latch */
    ANSELA &= 0b11001111;    /* set as digital io */    
    ODCONA &= 0b11001111;    /* enable push-pull */
    SLRCONA &= 0b11001111;   /* disable rate limit */
    TRISA &= 0b11001111;     /* set as digital output pin */  
    
    PORTC &= 0b11000000;     /* reset pin value */
    LATC &= 0b11000000;      /* reset pin latch */
    ANSELC &= 0b11000000;    /* set as digital io */
    ODCONC &= 0b11000000;    /* enable push-pull */
    SLRCONC &= 0b11000000;   /* disable rate limit */
    TRISC &= 0b11000000;     /* set as digital output pin */
    
    rc5Setup();
    
    ei();
}



/**
 *
 *  Execute application.
 *
 */
void _execute(void)
{
    RC5_COMMAND rc5Command;    

    /*-- enter forever loop --*/
    for(;; NOP())
    {        
        if ( rc5Ready() == true )
        {
            /*-- cache data --*/
            di();                   /* disable interrupts */
            rc5Command = rc5Get();  /* store local copy of data */
            rc5Clear();             /* clear ready flag */            
            ei();                   /* enable interrupts */

            /*-- check if address is VCR1 (0b00101) (Medion MD83709: device VCR, brand marantz [code 0257]) --*/
            if (rc5Command.address == 0b00101 )
            {
                switch ( rc5Command.command )
                {
                    case 0b0000000: /* '0 */
                    {
                        /*-- switch all outlets on --*/
                        PORTC |= 0b00111111;
                        PORTA |= 0b00110000;                        
                        break;
                    }

                    case 0b0000001: /* '1' */
                    {
                        /*-- switch outlet 1 on --*/
                        PORTCbits.RC0 = 1;                        
                        break;
                    }

                    case 0b0000010: /* '2' */
                    {
                        /*-- swith outlet 2 on --*/
                        PORTCbits.RC1 = 1;
                        break;
                    }

                    case 0b0000011: /* '3' */
                    {
                        /*-- switch outlet 3 on --*/
                        PORTCbits.RC2 = 1;
                        break;
                    }

                    case 0b0000100: /* '4' */
                    {
                        /*-- switch outlet 4 on --*/
                        PORTCbits.RC3 = 1;
                        break;
                    }

                    case 0b0000101: /* '5' */
                    {
                        /*-- switch outlet 5 on --*/
                        PORTCbits.RC4 = 1;
                        break;
                    }
                    
                    case 0b0000110: /* '6' */
                    {
                        /*-- switch outlet 6 on --*/
                        PORTCbits.RC5 = 1;
                        break;
                    }

                    case 0b0000111: /* '7' */
                    {
                        /*-- switch outlet 7 on --*/
                        PORTAbits.RA4 = 1;
                        break;
                    }

                    case 0b0001000: /* '8' */
                    {
                        /*-- switch outlet 8 on --*/
                        PORTAbits.RA5 = 1;
                        break;
                    }

                    case 0b0001001: /* '9' */
                    {
                        /*-- switch all outlets off --*/
                        PORTC &= 0b11000000;
                        PORTA &= 0b11001111;
                        break;
                    }
                    
                    case 0b00100000: /* 'PGM+' */
                    {
                        /*-- switch outlet 1 off --*/
                        PORTCbits.RC0 = 0;
                        break;
                    }

                    case 0b00100001: /* 'PGM-' */
                    {
                        /*-- switch outlet 2 off --*/
                        PORTCbits.RC1 = 0;
                        break;
                    }
                    
                    case 0b00110010: /* '<<' */
                    {
                        /*-- switch outlet 3 off --*/
                        PORTCbits.RC2 = 0;
                        break;
                    }

                    case 0b00110101: /* '>' */
                    {
                        /*-- switch outlet 4 off --*/
                        PORTCbits.RC3 = 0;
                        break;
                    }

                    case 0b00110100: /* '>>' */
                    {
                        /*-- switch outlet 5 off --*/
                        PORTCbits.RC4 = 0;
                        break;
                    }

                    case 0b00110110: /* '[]' */
                    {
                        /*-- switch outlet 6 off --*/
                        PORTCbits.RC5 = 0;
                        break;
                    }

                    case 0b00101001: /* '||' */
                    {
                        /*-- switch outlet 7 off --*/
                        PORTAbits.RA4 = 0;
                        break;
                    }

                    case 0b00111110: /* 'AV' */
                    {
                        /*-- switch outlet 8 off --*/
                        PORTAbits.RA5 = 0;
                        break;
                    }

                    default:
                    {
                        /*-- do nothing --*/
                        break;
                    }
                }                                
            }                        
        }
    }
}
