/**
 *
 * @file    rc5.h
 * @date    21-09-2026
 * @author  Bart Gysens
 *
 *  @brief  Philips RC5 IR receiver.
 *
 *          RC5 protocol details:
 *          A handset contains a keypad and a transmitter IC drives a IR LED.
 *          The command data is a Manchester coded bitstream modulating a 36KHz
 *          carrier. The IR signal from the transmitter is detected by a IR
 *          diode, and is amplified, filtered and demodulated so that the
 *          receiving device can act upon the received command. RC5 provides
 *          a one-way link only, with information traveling from the
 *          handset to the receiving unit. The command comprises 14 bits:
 *          + one start bit    : always logic 1.
 *          + one field bit    : initially this was a second start bit (always 
 *                               logic 1), more recently it is used as a sixth
 *                               (inversed!) address bit.
 *          + one control bit  : the value of this bit toggles with each
 *                               button press.
 *          + five address bits: selects one of the 32 possible systems.
 *          + six command bits : represents one of the 64 (or 128 with the
 *                               field bit) possible RC5 commands.
 *          A bit is transmited as:
 *          + value 0: a 36KHz burst of 889 �s followed by a pause of 889 �s.
 *          + value 1: a pause of 889 �s followed by a 36KHz burst of 889 �s.
 *          One bit is thus transmitted in 1778 �s.
 *
 *          Algorithm:
 *          + The pin input is driven by a RC5 IR receiver with inverse output,
 *            such as the TSOP4836. Everytime the digital level of the pin
 *            changes an interrupt is generated.
 *          + The time between two subsequent interrupts is measured. This time
 *            can be interpreted as a short pulse or a long pulse.
 *          + This allows to find out what the value of the received bit was:
 *            - a long pulse is interpreted as a bit received that differs in
 *              value from the previous bit.
 *            - two subsequent short pulses are interpreted as a bit received
 *              that equals in value from the previous bit.
 *            - the value of the first bit is always 1.
 *          + All of the received bits are considered invalid whenever the
 *            measured time is out of range. In such a case, receiption will
 *            start again with bit 1 (with a value of 1).
 *
 *          Implementation details:
 *          + The timer requires a clock (after prescaler) of 62.5KHz. This
 *            results in a time unit of 16�s per timer increment (1/62.5KHz).
 *          + The decision whether it is a single pulse or a double pulse is
 *            based on next time constants:
 *               bit time  := 1778 �s / 16 �s = 111 timer counts
 *               too short := 0.25 * bit time = 28  timer counts
 *               boundary  := 0.75 * bit time = 83  timer counts
 *               too long  := 1.25 * bit time = 139 timer counts
 *          + The above constants need to be adapted if it is impossible to use
 *            a clock of 62.5KHz. Also, make sure that the timer overflow
 *            interrupt happens at >= 2 * bit time in this case.
 * 
 *          Supported hardware:
 *          + IR RECEIVER
 *            - TSPOP4836 from Vishay
 *          + PROCESSOR
 *            - PIC16F15325:
 *                  INPUT=RA2
 *                  TIMER=TMR2: CLOCK=32MHz, FOSC/4, PRESCALER1:128
 *                              => 32MHz / 4 / 128 = 62.5KHz 
 *
 */

#ifndef     _IROUTLET_RC5_
#define     _IROUTLET_RC5_

/* --------------------------- *
 * Project and system includes *
 * --------------------------- */
#include <stdint.h>
#include <stdbool.h>



/* ------------------- *
 * Macros and typedefs *
 * ------------------- */

/*-- RC5 command data --*/
typedef struct
{
    uint8_t toggle;
    uint8_t address;
    uint8_t command;
} RC5_COMMAND;

/*-- external global data --*/
extern volatile bool _rc5CommandAvailable;
extern volatile RC5_COMMAND _rc5Command;

/*-- macros to manage RC5 command data in application --*/
#define rc5Ready() _rc5CommandAvailable
#define rc5Clear() _rc5CommandAvailable = false
#define rc5Get() _rc5Command



/* --------------------- *
 * Function declarations *
 * --------------------- */

/**
 *
 *  Configure the module for use.
 *  Remark: this function should be called from application.
 *
 */
void rc5Setup( void );



/**
 *
 *  Interrupt service routine.
 *  Remark: this function should be called on pin interrupt on change.
 *
 */
void rc5IsrPin( void );



/**
 *
 *  Interrupt service routine.
 *  Remark: this function should be called on timer overflow interrupt.
 *
 */
void rc5IsrTimer( void );

#endif  /*  _IROUTLET_RC5_  */

