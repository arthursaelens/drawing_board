#include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
#include <avr/io.h>
#include <util/delay.h>

int p2oef2(void) {
  DDRA = 0xFF; // Set all Port A pins as output 0b11111111
  PORTA = 0;   // Turn all LEDs off (they are connected to Port A)

  DDRE &= ~(1 << PE4); // Set PE4 as input 
  PORTE |= (1 << PE4);  // Activate pull-up resistor on PE4

  DDRE &= ~(1 << PC7); // Set PC7 as input 
  PORTE |= (1 << PC7);  // Activate pull-up resistor on PC7

  initBoard();
  initLCD();
  clearLCD();

  backlightOn();

  int totalT = 0;
  int T = 0;
  int N = 0; // Number of measurements

  while(1){
    printStringToLCD("T=", 0, 0);
    printIntToLCD(T, 0, 2); // Display the latest time
    printStringToLCD("Ta=", 0, 6);
    if (N == 0) {
        printStringToLCD("-", 0, 9); // Display dash if no measurements yet
    } else {
        printIntToLCD(totalT / N, 0, 9); // Display average time
    }
    printStringToLCD("N=", 0, 13);
    printIntToLCD(N, 0, 16); // Display number of measurements

    printStringToLCD("Press W to start", 1, 0);
      
    while(1){
      if(~PINE & (1 << PE4)){
        break;
      }
      _delay_ms(1);
    }
    
    clearLCD();
    printStringToLCD("LEDs -> Press C", 0, 0);
    _delay_ms(2000);
    
    PORTA = 255;
    

    int counter = 0;
    while(1){  
      counter++;
      if(~PINC & (1 << PC7)){
          PORTA = 0;
          
          break;
      }
      _delay_ms(1);
    }
    
    clearLCD();
    N++;
    totalT += counter;
    T = counter;
  }
  
  return 0;
}


#include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
#include <avr/io.h>
#include <util/delay.h>

int p2oef3(void) {
  DDRA = 0xFF; // Set all Port A pins as output 0b11111111
  PORTA = 0;   // Turn all LEDs off (they are connected to Port A)

  DDRE &= ~(1 << PE4); // Set PE4 as input 
  PORTE |= (1 << PE4);  // Activate pull-up resistor on PE4

  DDRE &= ~(1 << PC7); // Set PC7 as input 
  PORTE |= (1 << PC7);  // Activate pull-up resistor on PC7

  initBoard();
  initLCD();
  clearLCD();

  backlightOn();

  int totalT = 0;
  int T = 0;
  int N = 0; // Number of measurements

  while(1){
    printStringToLCD("T=", 0, 0);
    printIntToLCD(T, 0, 2); // Display the latest time
    printStringToLCD("Ta=", 0, 6);
    if (N == 0) {
        printStringToLCD("-", 0, 9); // Display dash if no measurements yet
    } else {
        printIntToLCD(totalT / N, 0, 9); // Display average time
    }
    printStringToLCD("N=", 0, 13);
    printIntToLCD(N, 0, 15); // Display number of measurements

    printStringToLCD("Press W to start", 1, 0);
      
    while(1){
      if(~PINE & (1 << PE4)){
        break;
      }
      _delay_ms(1);
    }
    
    clearLCD();
    printStringToLCD("LEDs -> Press C", 0, 0);
    _delay_ms(2000);
    
    PORTA = 255;
    TCCR1B &=~(0x07); //reset prescaler register to 0
    TCCR1B |=0x04; //set prescaler to 1/256 => aproxx 1 second
    TCNT1 = 0x0000;//reset timer to 0
    

    while(1){  
      if(~PINC & (1 << PC7)){
          PORTA = 0;
          
          break;
      }
      _delay_ms(1);
    }
    
    clearLCD();
    N++;
    totalT += TCNT1*0.016;
    T = TCNT1*0.016;
  }
  
  return 0;
}



#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
int latest_time=0;
int average_time=0;
int number_of_measurements=0;
volatile unsigned char interuptcontrole=0;

ISR(INT6_vect){//interupt bij knop E
  if (PORTA==0xFF){
    latest_time=TCNT1*0.016;//timer opslaan
    interuptcontrole=1;
  }
}



int p2oef4(void) {
  initBoard();//initialize board
  initLCD();//initialize LCD 
  clearLCD();//clear LCD 
  backlightOn(); //put backlight on

  DDRE&=~(_BV(PE4)); //W button as input
  PORTE|=_BV(PE4); //pull up W button
  DDRA = 0xFF;//leds as output
  PINA= 0x00; //leds off
  DDRE&=~(_BV(PE6));//E button input
  PORTE|= _BV(PE6);//pull up E button

  SREG|=_BV(SREG_I);//global interrupt enabling
  EIMSK|=_BV(INT6);//activates interupt on button E
  EICRB|=_BV(ISC61);//configuration falling edge
  EICRB&=~(_BV(ISC60));//idem

  TCCR1B&=~(0x07); //reset prescaler register to 0
  TCCR1B|=0x04; //set prescaler to 1/256 => aproxx 1 second




  while (1) {
    //print stats on LCD
    printStringToLCD("T=",0,0);
    printIntToLCD(latest_time,0,2);
    printStringToLCD(";Ta=",0,5);
    printIntToLCD(average_time,0,9);
    printStringToLCD(";N=",0,12);
    printIntToLCD(number_of_measurements,0,15);
    printStringToLCD("Press W to start",1,0);
    _delay_ms(500);
    while(1){
      if (~PINE & _BV(PE4)){ //controleer of W ingedrukt wordt
        break;
      }
      _delay_ms(1);
    }
    clearLCD();
    printStringToLCD("LEDs -> Press C",0,0);
    char amountOf500msSleeps=4+latest_time%5;//semi random generator for time till leds
    for (char i=0;i<amountOf500msSleeps;i++){
      _delay_ms(500);
    }
    PORTA=0xFF; //leds on
    TCNT1=0x0000;//reset timer
    while(1){
      if (interuptcontrole){//na interupt breaken
        break;
    }
    }
    interuptcontrole=0;
    PORTA=0;//leds off
    //update metrics
    number_of_measurements+=1;
    average_time=(average_time*(number_of_measurements-1)+latest_time)/number_of_measurements;
  }
  return 0;
}



#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
int latest_time=0;
int average_time=0;
int number_of_measurements=0;
volatile unsigned char interrupt1controle=0;
volatile unsigned char interrupt2controle=0;
ISR(INT6_vect){//interupt bij knop E
  if (PORTA==0xFF){
    latest_time=TCNT1*0.016;//timer opslaan
    interrupt1controle=1;
    PORTA=0;//leds hier al uit zodat niet meerdere keren kan getriggerd worden
  }
}
ISR(TIMER3_COMPA_vect){//interupt2contorle
  PORTA=0xFF; //leds on
  TCNT1=0x0000;//reset timer in ISR zodat instant gebeurd na leds aangaan
  TCCR3B&=~(0x07); //reset prescaler register to 0 so timer stops and there are no false interrupts
  interrupt2controle=1;
}



int p2oef5(void) {
  initBoard();//initialize board
  initLCD();//initialize LCD 
  clearLCD();//clear LCD 
  backlightOn(); //put backlight on

  DDRE&=~(_BV(PE4)); //W button as input
  PORTE|=_BV(PE4); //pull up W button
  DDRA = 0xFF;//leds as output
  PINA= 0x00; //leds off
  DDRE&=~(_BV(PE6));//E button input
  PORTE|= _BV(PE6);//pull up E button

  SREG|=_BV(SREG_I);//global interrupt enabling
  EIMSK|=_BV(INT6);//activates interupt on button E
  EICRB|=_BV(ISC61);//configuration falling edge
  EICRB&=~(_BV(ISC60));//idem

  TCCR3A|=_BV(COM3A0)|_BV(COM3A1); //Set OC3A flag high when OCR3X matches timer TCNT3
  TIMSK3|=_BV(OCIE3A); //generate interupt when OC3A flag is high

  TCCR1B&=~(0x07); //reset prescaler register to 0
  TCCR1B|=0x04; //set prescaler to 1/256 => aproxx 1 second

  TCCR3B&=~(0x07); //reset prescaler register to 0 so timer stops and there are no false interrupts



  while (1) {
    //print stats on LCD
    printStringToLCD("T=",0,0);
    printIntToLCD(latest_time,0,2);
    printStringToLCD(";Ta=",0,5);
    printIntToLCD(average_time,0,9);
    printStringToLCD(";N=",0,12);
    printIntToLCD(number_of_measurements,0,15);
    printStringToLCD("Press W to start",1,0);
    _delay_ms(500);
    while(1){
      if (~PINE & _BV(PE4)){ //controleer of W ingedrukt wordt
        break;
      }
      _delay_ms(1);
    }
    clearLCD();
    printStringToLCD("LEDs -> Press C",0,0);
    char amountOf500msSleeps=4+latest_time%5;//semi random generator for time till leds
    OCR3A=amountOf500msSleeps*500/0.064;//timelimit for when interrupt should be triggered
    
    TCNT3=0x0000;//reset timer
    TCCR3B|=0x05; //set prescaler to 1/1048 (=> aproxx 4 seconds) so timer begins to work
    char placeW=0;
    while(1){
      if (interrupt2controle){//na interrupt2 breaken
        break;
      }
      else{
        printStringToLCD("               ",1,0);
        printStringToLCD("W",1,placeW%15);
        _delay_ms(50);
        placeW+=1;
      }
    }
    
    interrupt2controle=0;
    while(1){
      if (interrupt1controle){//na interrupt1 breaken
        break;
      }
    }
    interrupt1controle=0;
    //update metrics
    number_of_measurements+=1;
    average_time=(average_time*(number_of_measurements-1)+latest_time)/number_of_measurements;
  }
  return 0;
}  



