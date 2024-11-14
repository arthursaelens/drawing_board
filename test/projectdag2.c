#include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile int state = -1; 

//rndm testwaarden in format zoals we ze zullen verwachten
float interval_servo1 = 1.5;
float interval_servo2 = 1.5;
float ms_to_ticks_fact = 2000;



ISR(TIMER3_COMPA_vect){
    if (state == 0) {
        OCR3A += interval_servo2 * ms_to_ticks_fact; //tel geschaalde versie v interval 2 op voor nieuwe timelimit(puls voor servo 2)
        PORTC = 0x00; //zet alles 0, pc0 en pc1 zijn nu dus beide 0
        PORTC |= _BV(PC1); //switcheroo de bit voor servo 2 (puls voor servo 2)
        PORTA |= 0b10000000;
    }

    if (state == 1) {
        OCR3A = 15 * ms_to_ticks_fact; //zet timelimit op 15ms totaal
        PORTC = 0x00; //alles affff
        PORTA |= 0b01000000;
    }

    if (state == 2) {
        TCNT3 = 0x0000; //timer vanaf nul
        OCR3A = interval_servo1 * ms_to_ticks_fact; //timelimit is geschaalde versie van timeinterval servo 1
        PORTC = 0x00;
        PORTC |= _BV(PC0);
        PORTA |= 0b00100000;
    }
    if (state >= 0) {
        state = (state+1)%3;
    }
}
    

int main(void) {
    initBoard();
    initLCD();
    clearLCD();

    DDRA = 0xFF;
    PORTA = 0x00;

    //configure PC0 & PC1 as output
    DDRC |= (_BV(PC0));
    DDRC |= (_BV(PC1));

    //add random button (west) as input
    DDRE &= ~(_BV(PE4));
    PORTE |= _BV(PE4);

    SREG|=_BV(SREG_I);
    TCCR3A|=_BV(COM3A0)|_BV(COM3A1); //Set OC3A flag high when OCR3X matches timer TCNT3
    TIMSK3|=_BV(OCIE3A); //generate interupt when OC3A flag is high


    TCCR3B&=~(0x07); //reset prescaler register to 0
    TCCR3B|= (0x02);//(juiste prescaler voor 20ms moet nog uitgezocht worden), vermoeden: /8:: 
    /* 
    16 * 10^6 / 8 = 2 * 10^6 ticks per sec
    max val 16 bits = 65 535
    na hoeveel tijd bereikt timer max val ::
        0.03276 sec
        32.76 ms

    tijd per tick = 1/(2*10^16) sec = 5*10^-14 ms tussen ticks
    */
    
    PORTC = 0x00;
    
    PORTA |= 0b00000001;

    while (1) {
        if (~PINE & _BV(PE4)) {
            PORTA |= 0b00000010;
            TCNT3 = 0x0000; //reset timer
            state = 0;
        }

        _delay_ms(1);
    }
    return 0;
}
