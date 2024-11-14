#include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <invken.h>

volatile int state = -1; 


//rndm testwaarden in format zoals we ze zullen verwachten
float interval_servo1;
float interval_servo2;
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

    Point_XY array_XY[] = {{8,0},{8.5,0},{8.5,-1},{9,-1},{9,0},{10,0},{10,-2},{9,-3},{8,-2},{8,0},{10,0},{10,-2},{8,-2}};
    

    int aantal_punten = sizeof(array_XY)/sizeof(array_XY[0]);
    
    Point_pws array_PW[aantal_punten];
    for (int j = 0; j < aantal_punten; j++ ) {
        Point_angles point_angle_j = XY_to_angles(array_XY[j],length_arm1,length_arm2);

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
        //int hoek = round(100 * point_angle_j.theta1);
        //int hoek2 = round(100 * point_angle_j.theta2);
        clearLCD();
        printIntToLCD(puls,0,0);
        printIntToLCD(puls2,1,0);
        printIntToLCD(puntindex,0,7);
        puntindex = (puntindex+1)%aantal_punten;
        
        _delay_ms(1000);
    }


    return 0;
}


/*we zijn bezig aan de schaling functie in het test bestand  maar eig boeit dat niet, nu is het de bedoeling dat we onze lijst met punten automatisch kunnen schrijven beginnend van een tekening
we kunnen al een lijst met punten tekenen
we moeten ook nog meer tussenpunten in de lijst steken zodat we minder storing en rechtere lijnen hebben
doe ook een accuratere meting van de lengte van de armen*/