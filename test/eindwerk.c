#include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <invken.h>
#include <lininterpol.h>
#include <cirkel.h>

#define LENGTH_ARM1 10.9 //10.9 hoofdletters
#define LENGTH_ARM2 15.2 //15.2  servo2 bovenste arm


volatile int state = -1; 
volatile int lijst = 0;
volatile int servo3 = 0; //als servo3 niet gebruikt moet worden servo3 = 0 anders 1
volatile int aantal_punten;
Point_XY *currentArrayXY;
volatile int totalNumberOfPoints;
Point_pws *array_PW;
//stopp
Point_XY array1[] = {{12,-8},{-4,-8}}; 
Point_XY array2[] = {{11,-16},{-4,-16}};
Point_XY array3[] = {{8,-4},{8,-20}};
Point_XY array4[] = {{0,-4},{0,-20}};
Point_XY array5[] = {{1,-17},{1,-21},{5,-21},{8,-19},{5,-17},{5,-21},{5,-17},{1,-17},{1,-17.5},{3,-17.5},{3,-18.5},{1,-18.5}}; 
Point_XY array6[] = {{12, -12}, {9, -14}, {10.5, -9}, {10.5, -15}, {9, -10}, {12, -12}}; 
Point_XY *array7; 
//Point_XY array8[] = {{1,-17},{1,-23},{5,-23},{8,-20},{5,-17},{1,-17},{1,-23},{5,-23},{5,-17},{8,-20},{5,-23}}; 


//rndm testwaarden in format zoals we ze zullen verwachten
float interval_servo1;
float interval_servo2;
float interval_pen_neer = 0.4; //uiterst links is 0.5(pen neer) en uiterst rechts is 2.3(pen op), midden wellicht 1.5
float interval_pen_op = 0.9;
unsigned int ms_to_ticks_fact = 2000;




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
        if (servo3 == 1) {
            //we heffen de pen op tot servo3 op nul gezet wordt
            OCR3A += interval_pen_op * ms_to_ticks_fact; //tel geschaalde versie v interval 2 op voor nieuwe timelimit(puls voor servo 2)
        } else {
            OCR3A += interval_pen_neer * ms_to_ticks_fact; //tel geschaalde versie v interval 2 op voor nieuwe timelimit(puls voor servo 2)
        }
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
    
void getNextArray(){
    if (lijst == 0){
        free(currentArrayXY);
        aantal_punten = sizeof(array1)/sizeof(array1[0]);
        currentArrayXY = malloc( aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array1[i];
        }
    }else if (lijst == 1){
        free(currentArrayXY);
        aantal_punten = sizeof(array2)/sizeof(array2[0]);
        currentArrayXY = malloc(aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array2[i];
        }
    } else if (lijst == 2){
        free(currentArrayXY);
        aantal_punten = sizeof(array3)/sizeof(array3[0]);
        currentArrayXY = malloc( aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array3[i];
        }
    }else if (lijst == 3){
        free(currentArrayXY);
        aantal_punten = sizeof(array4)/sizeof(array4[0]);
        currentArrayXY = malloc(aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array4[i];
        }
    }else if (lijst == 4){
        free(currentArrayXY);
        aantal_punten = sizeof(array5)/sizeof(array5[0]);
        currentArrayXY = malloc( aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array5[i];
        }
    }else if (lijst == 5){
        free(currentArrayXY);
        aantal_punten = sizeof(array6)/sizeof(array6[0]);
        currentArrayXY = malloc( aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array6[i];
        }
    }else if (lijst == 6){
        free(currentArrayXY);
        aantal_punten = sizeof(array7)/sizeof(array7[0]);
        currentArrayXY = malloc( aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array7[i];
        }
    }/*else if (lijst == 7){
        free(currentArrayXY);
        aantal_punten = sizeof(array8)/sizeof(array8[0]);
        currentArrayXY = malloc( aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array8[i];
        }
    }*/
    free(array_PW);
    int aantal_punten_extended;
    Point_XY* extendedArrayXY = extendArrayWithInterpolation(currentArrayXY, aantal_punten, 0.8, &aantal_punten_extended, 1);
    totalNumberOfPoints = aantal_punten_extended;
    array_PW = malloc(totalNumberOfPoints * sizeof(Point_pws));
    for (int j = 0; j < totalNumberOfPoints; j++ ) {
        Point_angles point_angle_j = XY_to_angles(extendedArrayXY[j],LENGTH_ARM1,LENGTH_ARM2);

        array_PW[j] = angles_to_pws(point_angle_j);
        /*initLCD();
        clearLCD();
        printIntToLCD((int)(array_PW[j].pw1*ms_to_ticks_fact),0,0);
        printIntToLCD((int)(array_PW[j].pw2*ms_to_ticks_fact),1,0);
        _delay_ms(200);*/
    }
    free(extendedArrayXY);
    
}


int main(void) {

    array7 = generatePointsOnCircle(2,4,-12,25);

    array_PW = malloc(sizeof(Point_pws));

    //Point_XY array_XY[] = {{12,-8},{-4,-8}};
    //int aantal_punten = sizeof(array_XY)/sizeof(array_XY[0]);


    getNextArray();


    //Point_XY* circle = generatePointsOnCircle(2, 10, -5, 100);
    
    /*int aantal_punten_extended;
    Point_XY* extendedArrayXY = extendArrayWithInterpolation(currentArrayXY, aantal_punten, 0.2, &aantal_punten_extended, 1);
    totalNumberOfPoints = aantal_punten_extended;
    array_PW = malloc( totalNumberOfPoints * sizeof(Point_pws));
    for (int j = 0; j < totalNumberOfPoints; j++ ) {
        Point_angles point_angle_j = XY_to_angles(extendedArrayXY[j],LENGTH_ARM1,LENGTH_ARM2);

        array_PW[j] = angles_to_pws(point_angle_j);
    }*/
    
    

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
    DDRE &= ~(_BV(PE4)); // knop WEST
    PORTE |= _BV(PE4);
    DDRE &= ~(_BV(PE6)); // knop EAST
    PORTE |= _BV(PE6);
    DDRE &= ~(_BV(PE7)); // knop NORTH
    PORTE |= _BV(PE7);
    DDRE &= ~(_BV(PE5)); // knop SOUTH
    PORTE |= _BV(PE5);
    DDRC &= ~(_BV(PC7)); // knop Central
    PORTC |= _BV(PC7);

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
    int iteratie = 0;
    int westknop = 0;
    int eastknop = 0;
    int northknop = 0;
    int southknop = 0;

    while (1) {
        /*clearLCD();
          
        for (int i = 0; i < aantal_punten; i++) {
            printIntToLCD(array_XY[i].x,0,0);

            printIntToLCD(array_XY[i].y,1,0);
            _delay_ms(300);
        }*/

        //We willen niet dat onze tekentafel begint te tekenen vanaf we de verbinding maken met de microchip
        //dus hierdoor begint het pas als we op W knop duwen
        if (~PINC & _BV(PC7)) {
            TCNT3 = 0x0000; //reset timer
            state = 0;
        }
        if (~PINE & _BV(PE6)) {
            eastknop = 1;
            westknop = 0;
            northknop = 0;
            southknop = 0;
            lijst = 4;
            getNextArray();
            puntindex = 0;
            iteratie = 0;
            servo3 = 1;
        }
        if (~PINE & _BV(PE4)) {
            eastknop = 0;
            westknop = 1;
            northknop = 0;
            southknop = 0;
            lijst = 5;
            getNextArray();
            puntindex = 0;
            iteratie = 0;
            servo3 = 1;
        }
        if (~PINE & _BV(PE5)) {
            eastknop = 0;
            westknop = 0;
            northknop = 0;
            southknop = 1;
            lijst = 6;
            getNextArray();
            puntindex = 0;
            iteratie = 0;
            servo3 = 1;
        }
        if ( lijst >= 0 && lijst < 4) {
            if (puntindex == 1 && iteratie == 0) {
                servo3 = 0;
            }
            interval_servo1 = array_PW[puntindex].pw1;
            interval_servo2 = array_PW[puntindex].pw2;
            puntindex = (puntindex+1)%totalNumberOfPoints;
            if ((puntindex+1) == totalNumberOfPoints && iteratie == 0) {
               lijst = (lijst+1);
                getNextArray();
               servo3 = 1;
               puntindex = 0;
               iteratie = 0;
            } else if ((puntindex+1) == totalNumberOfPoints ) {
                iteratie += 1;
            }
        }
        if (westknop || eastknop || southknop || northknop){
            if (puntindex == 1 && iteratie == 0) {
                servo3 = 0;
            }
            interval_servo1 = array_PW[puntindex].pw1;
            interval_servo2 = array_PW[puntindex].pw2;
            puntindex = (puntindex+1)%totalNumberOfPoints;
        }


        clearLCD();
        printIntToLCD(puntindex,0,0);
        printIntToLCD(totalNumberOfPoints,1,0);
        printIntToLCD(iteratie,0,7);
        printIntToLCD(lijst,1,7);
        printIntToLCD((int)(array_PW[puntindex].pw1*ms_to_ticks_fact),0,11);
        printIntToLCD((int)(array_PW[puntindex].pw2*ms_to_ticks_fact),1,11);


        /*
        Dit stukje code hebben we geschreven om te checken welke hoeken en welke pulsen er werden berekend en projecteerden ze zo op het LCDscherm
        */
        /*
        int puntx;
        int punty;
        if (lijst == 1) {
            puntx = round(100 * array_XY[puntindex].x);
            punty = round(100 * array_XY[puntindex].y);
        }
        if (lijst == 2) {
            puntx = round(100 * array_XY2[puntindex].x);
            punty = round(100 * array_XY2[puntindex].y);
        }
        //int puls = round(100 * interval_servo1);
        //int puls2 = round(100 * interval_servo2);
        //int hoek = round(100 * array_angle[puntindex].theta1);
        //int hoek2 = round(100 * array_angle[puntindex].theta2);
        */

        
        /*
        clearLCD();
        printIntToLCD(puls,0,0);
        printIntToLCD(puls2,1,0);
        printIntToLCD(puntindex,0,7);
        */
        
        _delay_ms(100);
        //gespeeld met delay, +- 50 zorgt voor onverwachte bewegingen (te dicht bij periode van pwm signaal -> ms foute signalen???)
    }


    return 0;
}




/**
 * @file main.c
 * @brief Main control program for the drawing arm.
 *
 * This file contains the main program that controls the drawing arm using various servo motors.
 * It includes initializations, interrupt service routines, and the main control loop.
 */

#include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "invken.h"
#include "lininterpol.h"
#include "cirkel.h"

#define LENGTH_ARM1 10.9 ///< Length of the short arm (lower arm | servo 1 | pc0)
#define LENGTH_ARM2 15.2 ///< Length of the long arm (upper arm | servo 2 | pc1)


volatile int state = -1; ///< state of PWM signal
volatile int lijst = 0; ///< List index for the current drawing pattern
volatile int servo3 = 0; ///< State of the third servo (pen control)
volatile int aantal_punten; ///< Number of points in the current pattern

Point_XY *currentArrayXY; ///< Array of current points in XY coordinates
volatile int totalNumberOfPoints; ///< Total number of points in the extended pattern
Point_pws *array_PW; ///< Array of points in pulse width format

/// Predefined patterns
Point_XY array1[] = {{12,-8},{-4,-8}}; 
Point_XY array2[] = {{11,-16},{-4,-16}};
Point_XY array3[] = {{8,-4},{8,-20}};
Point_XY array4[] = {{0,-4},{0,-20}};
Point_XY array5[] = {{1,-17},{1,-21},{5,-21},{8,-19},{5,-17},{5,-21},{5,-17},{1,-17},{1,-17.5},{3,-17.5},{3,-18.5},{1,-18.5}}; 
Point_XY array6[] = {{12, -12}, {9, -14}, {10.5, -9}, {10.5, -15}, {9, -10}, {12, -12}}; 
Point_XY *array7; ///< Dynamically generated pattern


float interval_servo1; ///< Pulse width interval for servo 1
float interval_servo2; ///< Pulse width interval for servo 2
float interval_pen_neer = 0.4; ///< Pulse width for pen down
float interval_pen_op = 0.9; ///< Pulse width for pen up
unsigned int ms_to_ticks_fact = 2000; ///< Factor to convert milliseconds to timer ticks


/**
 * @brief Timer 3 compare match interrupt service routine.
 *
 * This ISR handles the PWM signal generation for the servos and pen control.
 */
ISR(TIMER3_COMPA_vect){
    if (state == 0) {
        TCNT3 = 0x0000; //timer vanaf nul
        OCR3A = interval_servo1 * ms_to_ticks_fact; //timelimit is geschaalde versie van timeinterval servo 1
        PORTC |= _BV(PC0);
    }
    if (state == 1) {
        OCR3A += interval_servo2 * ms_to_ticks_fact; //tel geschaalde versie v interval 2 op voor nieuwe timelimit(puls voor servo 2)
        PORTC &= ~((_BV(PC0))); //zet pc0 op 0  , = 0x00; mag niet omdat we misschien te veel registers op nul zetten
        PORTC |= _BV(PC1); //switcheroo de bit voor servo 2 (puls voor servo 2)
    }
    if (state == 2){
        if (servo3 == 1) {
            //we heffen de pen op tot servo3 op nul gezet wordt
            OCR3A += interval_pen_op * ms_to_ticks_fact; //tel geschaalde versie v interval 2 op voor nieuwe timelimit(puls voor servo 2)
        } else {
            OCR3A += interval_pen_neer * ms_to_ticks_fact; //tel geschaalde versie v interval 2 op voor nieuwe timelimit(puls voor servo 2)
        }
        PORTC &= ~((_BV(PC1))); //zet pc1 op 0 
        PORTF |= (_BV(PF0)); // bitjes worden geswitcherood
    }
    if (state == 3) {
        OCR3A = 15 * ms_to_ticks_fact; //zet timelimit op 15ms totaal
        PORTF &= ~(_BV(PF0)); // PF0 op 0
        // alles af voor resterende tijd in de periode
    }
    if (state >= 0) {
        state = (state+1)%4;
    }

}

/**
 * @brief Load the next pattern into the current array.
 *
 * This function loads the next set of XY coordinates into the current array
 * based on the value of the `lijst` variable and converts them to pulse widths.
 */
void getNextArray(){
    if (lijst == 0){
        free(currentArrayXY);
        aantal_punten = sizeof(array1)/sizeof(array1[0]);
        currentArrayXY = malloc( aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array1[i];
        }
    }else if (lijst == 1){
        free(currentArrayXY);
        aantal_punten = sizeof(array2)/sizeof(array2[0]);
        currentArrayXY = malloc(aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array2[i];
        }
    } else if (lijst == 2){
        free(currentArrayXY);
        aantal_punten = sizeof(array3)/sizeof(array3[0]);
        currentArrayXY = malloc( aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array3[i];
        }
    }else if (lijst == 3){
        free(currentArrayXY);
        aantal_punten = sizeof(array4)/sizeof(array4[0]);
        currentArrayXY = malloc(aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array4[i];
        }
    }else if (lijst == 4){
        free(currentArrayXY);
        aantal_punten = sizeof(array5)/sizeof(array5[0]);
        currentArrayXY = malloc( aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array5[i];
        }
    }else if (lijst == 5){
        free(currentArrayXY);
        aantal_punten = sizeof(array6)/sizeof(array6[0]);
        currentArrayXY = malloc( aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array6[i];
        }
    }else if (lijst == 6){
        free(currentArrayXY);
        aantal_punten = sizeof(array7)/sizeof(array7[0]);
        currentArrayXY = malloc( aantal_punten * sizeof(Point_XY));
        for (int i = 0; i < aantal_punten; i++) {
            currentArrayXY[i] = array7[i];
        }
    }
    if (lijst != 6) {
    free(array_PW);
    int aantal_punten_extended;
    Point_XY* extendedArrayXY = extendArrayWithInterpolation(currentArrayXY, aantal_punten, 0.8, &aantal_punten_extended, 1);
    totalNumberOfPoints = aantal_punten_extended;
    array_PW = malloc(totalNumberOfPoints * sizeof(Point_pws));
    for (int j = 0; j < totalNumberOfPoints; j++ ) {
        Point_angles point_angle_j = XY_to_angles(extendedArrayXY[j],LENGTH_ARM1,LENGTH_ARM2);

        array_PW[j] = angles_to_pws(point_angle_j);
    }
    free(extendedArrayXY);
    }
}

/**
 * @brief Main function.
 *
 * This is the entry point of the program. It initializes the board, loads the initial pattern,
 * and enters the main control loop.
 *
 * @return int Always returns 0.
 */
int main(void) {

    array7 = generatePointsOnCircle(2,4,-12,25);

    array_PW = malloc(sizeof(Point_pws));

    getNextArray();
    

    initBoard();
    initLCD();
    clearLCD();
    backlightOn();


    DDRA = 0xFF;
    PORTA = 0x00;

    //configure PC0 & PC1 as output
    DDRC |= (_BV(PC0));
    DDRC |= (_BV(PC1));

    //add buttons as input
    DDRE &= ~(_BV(PE4)); // knop WEST
    PORTE |= _BV(PE4);
    DDRE &= ~(_BV(PE6)); // knop EAST
    PORTE |= _BV(PE6);
    DDRE &= ~(_BV(PE7)); // knop NORTH
    PORTE |= _BV(PE7);
    DDRE &= ~(_BV(PE5)); // knop SOUTH
    PORTE |= _BV(PE5);
    DDRC &= ~(_BV(PC7)); // knop Central
    PORTC |= _BV(PC7);

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

    int puntindex = 0;
    int iteratie = 0;
    int westknop = 0;
    int eastknop = 0;
    int northknop = 0;
    int southknop = 0;

    while (1) {
        //We willen niet dat onze tekentafel begint te tekenen vanaf we de verbinding maken met de microchip
        //dus hierdoor begint het pas als we op centrale knop duwen
        if (~PINC & _BV(PC7)) {
            TCNT3 = 0x0000; //reset timer
            state = 0;
        }
        if (~PINE & _BV(PE6)) {
            eastknop = 1;
            westknop = 0;
            northknop = 0;
            southknop = 0;
            lijst = 4;
            getNextArray();
            puntindex = 0;
            iteratie = 0;
            servo3 = 1;
        }
        if (~PINE & _BV(PE4)) {
            eastknop = 0;
            westknop = 1;
            northknop = 0;
            southknop = 0;
            lijst = 5;
            getNextArray();
            puntindex = 0;
            iteratie = 0;
            servo3 = 1;
        }
        if (~PINE & _BV(PE5)) {
            eastknop = 0;
            westknop = 0;
            northknop = 0;
            southknop = 1;
            lijst = 6;
            getNextArray();
            puntindex = 0;
            iteratie = 0;
            servo3 = 1;
        }
        if ( lijst >= 0 && lijst < 4) //rooster wordt getekent vanaf we op centrale knop duwen
        {
            if (puntindex == 1 && iteratie == 0) //we laten de tekenarm eerst naar de correcte positie gaan zonder dat de arm neer is
            {
                servo3 = 0;
            }
            interval_servo1 = array_PW[puntindex].pw1; // we geven de correcte pulswidths mee met de servo's
            interval_servo2 = array_PW[puntindex].pw2;
            puntindex = (puntindex+1)%totalNumberOfPoints; // dit geeft ons de mogelijkheid om het aantal iteraties te kiezen en een lijst meerdere keren te laten tekenen
            if ((puntindex+1) == totalNumberOfPoints && iteratie == 0) {
                lijst = (lijst+1);
                getNextArray();
                servo3 = 1;
                puntindex = 0;
                iteratie = 0;
            } else if ((puntindex+1) == totalNumberOfPoints ) {
                iteratie += 1;
            }
        }
        if (westknop || eastknop || southknop || northknop) // dit mag alleen maar gebeuren als er op een knop geduwd is
        {
            if (puntindex == 1 && iteratie == 0) {
                servo3 = 0;
            }
            interval_servo1 = array_PW[puntindex].pw1;
            interval_servo2 = array_PW[puntindex].pw2;
            puntindex = (puntindex+1)%totalNumberOfPoints;
        }


        /*
        Dit stukje code hebben we geschreven om te checken welke hoeken en welke pulsen er werden berekend en projecteerden ze zo op het LCDscherm
        */
        /*
        clearLCD();
        printIntToLCD(puntindex,0,0);
        printIntToLCD(totalNumberOfPoints,1,0);
        printIntToLCD(iteratie,0,7);
        printIntToLCD(lijst,1,7);
        printIntToLCD((int)(array_PW[puntindex].pw1*ms_to_ticks_fact),0,11);
        printIntToLCD((int)(array_PW[puntindex].pw2*ms_to_ticks_fact),1,11);
        */
        
        _delay_ms(100);
    }


    return 0;
}



