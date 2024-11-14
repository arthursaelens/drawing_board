#include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <invken.h>

#define LENGTH_ARM1 10.9 //10.9 hoofdletters
#define LENGTH_ARM2 15.2 //15.2  servo2 bovenste arm


volatile int state = -1; 

//rndm testwaarden in format zoals we ze zullen verwachten
float interval_servo1;
float interval_servo2;
float interval_servo3 = 0.7;
float ms_to_ticks_fact = 2000;




ISR(TIMER3_COMPA_vect){
    if (state == 0) {
        TCNT3 = 0x0000; //timer vanaf nul
        OCR3A = interval_servo1 * ms_to_ticks_fact; //timelimit is geschaalde versie van timeinterval servo 1
        PORTC |= _BV(PC0);
    }
    if (state == 1) {
        OCR3A += interval_servo2 * ms_to_ticks_fact; //tel geschaalde versie v interval 2 op voor nieuwe timelimit(puls voor servo 2)
        PORTC = 0x00; //&= ~((_BV(PC0))); //zet pc0 op 0 
        PORTC |= _BV(PC1); //switcheroo de bit voor servo 2 (puls voor servo 2)
    }
    if (state == 2){
        OCR3A += interval_servo3 * ms_to_ticks_fact; //tel geschaalde versie v interval 2 op voor nieuwe timelimit(puls voor servo 2)
        PORTC = 0x00; //&= ~((_BV(PC1))); //zet pc1 op 0 
        PORTF |= (_BV(PF0)); // bitjes worden geswitcherood
    }
    if (state == 3) {
        OCR3A = 15 * ms_to_ticks_fact; //zet timelimit op 15ms totaal
        PORTF = 0x00; //&= ~(_BV(PF0)); 
        // alles af voor resterende tijd in de periode
    }
    if (state >= 0) {
        state = (state+1)%4;
    }

}
    

int main(void) {

    Point_XY array_XY[] = {{8,0},{8.5,0},{8.5,-1},{9,-1},{9,0},{10,0},{10,-2},{9,-3},{8,-2},{8,0},{10,0},{10,-2},{8,-2}};
    
    
    int aantal_punten = sizeof(array_XY)/sizeof(array_XY[0]);

    Point_angles array_angle[aantal_punten];

    Point_pws array_PW[aantal_punten];

    for (int j = 0; j < aantal_punten; j++ ) {
        Point_angles point_angle_j = XY_to_angles(array_XY[j],LENGTH_ARM1,LENGTH_ARM2);

        array_angle[j] = point_angle_j;
        array_PW[j] = angles_to_pws(point_angle_j);
    }
    
    


    initBoard();
    initLCD();
    clearLCD();
    backlightOn();

    DDRA = 0xFF;
    PORTA = 0x00;

    //configure PC0 & PC1 as output
    DDRC |= (_BV(PC0));
    DDRC |= (_BV(PC1));

    //add random button (west) as input
    DDRE &= ~(_BV(PE4));
    PORTE |= _BV(PE4);
    DDRE &= ~(_BV(PE6)); // knop EAST
    PORTE |= _BV(PE6);

    //servo 3
    DDRF |= (_BV(PF0));

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

    int puntindex = 0;

    while (1) {
        if (~PINE & _BV(PE4)) {

            PORTA |= 0b00000010;
            TCNT3 = 0x0000; //reset timer
            state = 0;
        }
        
        PORTA |= 0b00000100;
        interval_servo1 = array_PW[puntindex].pw1;
        interval_servo2 = array_PW[puntindex].pw2;

        int puls = round(100 * interval_servo1);
        int puls2 = round(100 * interval_servo2);
        //int hoek = round(100 * array_angle[puntindex].theta1);
        //int hoek2 = round(100 * array_angle[puntindex].theta2);
        clearLCD();
        printIntToLCD(puls,0,0);
        printIntToLCD(puls2,1,0);
        printIntToLCD(puntindex,0,7);
        
        puntindex = (puntindex+1)%aantal_punten;
        
        _delay_ms(1000);
    }


    return 0;
}
