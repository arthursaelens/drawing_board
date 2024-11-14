#include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "invken.h"
#include "lininterpol.h"
#include "cirkel.h"
#include "figure.h"

#define LENGTH_ARM1 10.9 //10.9 hoofdletters
#define LENGTH_ARM2 15.2 //15.2  servo2 bovenste arm


volatile int state = -1; 
volatile int lijst = 0;
volatile int servo3 = 0;

//rndm testwaarden in format zoals we ze zullen verwachten
float interval_servo1;
float interval_servo2;
float interval_pen_neer = 0.5; //uiterst links is 0.5(pen neer) en uiterst rechts is 2.3(pen op), midden wellicht 1.5
float interval_pen_op = 0.9;
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
    

int main(void) {

    /*
    //these points draw a house:
    Point_XY array_XY[] = {{7,-6}, {8, -6}, {8, -9}, {9,-9}, {9,-6}, {12,-6}, {12,-11}, {7,-11}, {7,-6}, {9,-6}, {12,-6}, {12,-11}, {9.5, -13}, {7,-11}}; // puntenlijst voor huisje

    //Point_XY* circle = generatePointsOnCircle(2, 10, -5, 100);
    
    int aantal_punten = sizeof(array_XY)/sizeof(array_XY[0]);
    int aantal_punten_extended;
    Point_XY* extendedArrayXY = extendArrayWithInterpolation(array_XY, aantal_punten, 0.2, &aantal_punten_extended, 1);
    */

    Point_XY array_XY[] = {{17.106659, -12.576479}, {16.163183, -10.989997}, {14.728840, -8.367129}, {12.771336, -6.216915}, {12.856673, -4.067763}, {14.291194, -2.475373}, {13.511963, 0.404701}, {15.925131, 1.193924}, {15.495327, 4.060206}, {14.586705, 6.730167}, {17.388867, 6.220203}, {19.822976, 7.265038}, {18.664050, 4.513707}, {18.510121, 1.594483}, {21.231306, 1.122010}, {20.038850, -1.366305}, {20.065012, -4.296675}, {21.482544, -5.355032}, {20.079328, -7.003277}, {19.092964, -9.802717}, {17.493996, -12.334652}, {16.802230, -11.587895}, {12.746426, -4.982914}, {13.740771, -7.034901}, {13.277802, -5.922013}, {13.828293, -5.787710}, {14.029743, -5.259532}, {13.735130, -4.625915}, {13.984913, -4.196919}, {14.694045, 0.444270}, {14.107890, -1.510503}, {14.345569, -1.255558}, {14.660312, -1.757112}, {14.638307, -0.596043}, {15.114034, -0.435508}, {20.162324, -1.014262}, {20.680651, 1.004308}, {18.966676, 1.039364}, {19.226075, 0.159708}, {19.783266, -0.055716}, {19.563242, -0.789186}, {20.339817, -0.220475}, {20.518924, -6.598060}, {21.181197, -4.963507}, {20.251932, -6.284439}, {20.047870, -6.845462}, {19.979583, -4.498908}, {19.629955, -1.526148}, {18.520964, 1.258983}, {18.422417, 4.187189}, {19.658976, 6.914570}, {17.535254, 6.167235}, {14.744479, 6.663290}, {15.630749, 4.040084}, {15.443143, 5.358341}, {16.111240, 4.219702}, {15.679531, 5.730720}, {16.833116, 4.003796}, {16.747467, 4.540698}, {17.371061, 5.738609}, {17.618084, 4.557771}, {17.747098, 4.812769}, {18.366945, 5.095449}, {18.310585, 4.732977}, {17.747310, 2.794272}, {17.973421, 0.243114}, {17.520697, -2.099359}, {17.540940, -4.449882}, {18.615601, -6.556664}, {16.996474, -7.224820}, {16.416748, -5.053136}, {16.452664, -2.767884}, {16.688575, -0.390789}, {16.992234, 1.930407}, {16.368429, 3.630506}, {15.973712, 0.893343}, {14.882821, -1.896223}, {14.183844, -4.790555}, {14.594922, -7.746477}, {16.368885, -8.016518}, {18.974501, -9.024746}, {16.947990, 2.819506}, {16.856297, 0.500446}, {16.813041, -1.925798}, {16.474968, -4.209880}, {16.607667, -6.502952}, {17.323327, -5.617195}, {17.393427, -3.361059}, {17.460533, -1.122534}, {17.745247, 1.210228}, {17.509296, 3.647909}, {18.617157, -6.290857}, {18.311749, -5.991947}, {17.937240, -5.570560}, {17.418309, -5.534146}, {18.755799, -10.224967}, {17.632567, -8.105225}, {15.208242, -9.211231}, {16.703829, -11.570964}, {18.109451, -11.316665}, {17.401116, -12.300198}};
        
    //int aantal_punten_extended;
    //Point_XY* extendedArrayXY = extendArrayWithInterpolation(array_XY, 101, 1.5, &aantal_punten_extended, 1);
    
    /*
    Line line10;
    line10.points = temp10;
    line10.numPoints = 27;

    Line line11;
    line11.points = temp11;
    line11.numPoints = 99;
    */

    /*
    Line line11 = makeLine(temp11, 99);
    Line line12 = makeLine(temp12, 8);
    Line line13 = makeLine(temp13, 8);
    Line line14 = makeLine(temp14, 9);
    */

    //Line lines[2] = {line1, line1};

    //Figure figure1 = makeFigure(lines, 2);
    
    
    //the array initialized here will be the one we draw
    int totalNumberOfPoints = 101;
    Point_pws array_PW[totalNumberOfPoints];

    /*
    for (int j = 0; j < totalNumberOfPoints; j++ ) {
        Point_angles point_angle_j = XY_to_angles(line1.points[j],LENGTH_ARM1,LENGTH_ARM2);

        array_PW[j] = angles_to_pws(point_angle_j);
    }
    */
    

    
    for (int j = 0; j < totalNumberOfPoints; j++ ) {
        Point_angles point_angle_j = XY_to_angles(array_XY[j],LENGTH_ARM1,LENGTH_ARM2);

        array_PW[j] = angles_to_pws(point_angle_j);
    }
    
    

    /*
    for (int j = 0; j < 100; j++) {
        Point_angles pointJ = XY_to_angles(circle[j], LENGTH_ARM1, LENGTH_ARM2);

        array_PW[j] = angles_to_pws(pointJ);
        
    }
    */
    

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

    unsigned int puntindex = 0;

    while (1) {

        //We willen niet dat onze tekentafel begint te tekenen vanaf we de verbinding maken met de microchip
        //dus hierdoor begint het pas als we op W knop duwen
        if (~PINE & _BV(PE4)) {
            TCNT3 = 0x0000; //reset timer
            state = 0;
        }
        
        PORTA |= 0b00000100;
        
        int printVals = 1;
        clearLCD();
        // Check if interval_servo1 is out of bounds
        if (array_PW[puntindex].pw1 < 0.5 || array_PW[puntindex].pw1 > 2.5) {
            printStringToLCD("interval_servo1 out of bounds!", 0, 0);
            
            //set intervals to "default" values, so nothing crazy happens
            interval_servo1 = 1.5;
            interval_servo2 = 1.5;
            printVals = 0;

        }
        
        // Check if interval_servo2 is out of bounds
        if (array_PW[puntindex].pw2 < 0.5 || array_PW[puntindex].pw2 > 2.5) {
            printStringToLCD("interval_servo2 out of bounds!", 1, 0);
            interval_servo1 = 1.5;
            interval_servo2 = 1.5;
            printVals = 0;

        } else {
            interval_servo1 = array_PW[puntindex].pw1;
            interval_servo2 = array_PW[puntindex].pw2;
        }
        
        //interval_servo1 = array_PW[puntindex].pw1;
        //interval_servo2 = array_PW[puntindex].pw2;
        

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
        */
        int puls = round(100 * interval_servo1);
        int puls2 = round(100 * interval_servo2);
        //int hoek = round(100 * array_angle[puntindex].theta1);
        //int hoek2 = round(100 * array_angle[puntindex].theta2);
        

        
        if (printVals) {
            clearLCD();
            
            printIntToLCD(puls,0,0);
            printIntToLCD(puls2,1,0);

            //printIntToLCD(line1.points[puntindex].x,0, 4);
            //printIntToLCD(line1.points[puntindex].y,1, 4);

            //printIntToLCD(line1.points[2].x, 1, 10);
            
            printIntToLCD(puntindex,0,7);
            printIntToLCD(totalNumberOfPoints,1,7);

        }
        _delay_ms(100);
        //gespeeld met delay, +- 50 zorgt voor onverwachte bewegingen (te dicht bij periode van pwm signaal -> ms foute signalen???)
    
        puntindex = (puntindex+1)%totalNumberOfPoints;
    }


    return 0;
}
