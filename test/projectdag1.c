#include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
#include <avr/io.h>
#include <util/delay.h>

int tt(void) {
  DDRA = 0xFF; // Set all Port A pins as output 0b11111111
  PORTA = 0;   // Turn all LEDs off (they are connected to Port A)

  DDRE &= ~(1 << PE4); // Set PE4 as input WEST
  PORTE |= (1 << PE4);  // Activate pull-up resistor on PE4
  DDRE &= ~(1 << PE6); // Set PE6 as input EAST
  PORTE |= (1 << PE6);  // Activate pull-up resistor on PE6
  DDRE &= ~(1 << PE7); // Set PE4 as input NORTH
  PORTE |= (1 << PE7);  // Activate pull-up resistor on PE4
  DDRE &= ~(1 << PE5); // Set PE6 as input SOUTH
  PORTE |= (1 << PE5);  // Activate pull-up resistor on PE6



  DDRC |= (1 << PC0); // Set PC0 as output
  //PORTC |= (1 << PC0); // pull up weerstand
  
  DDRC |= (1 << PC1); // Set PC1 as output
  //PORTC |= (1 << PC1); // pullup weerstand maar dit zorgt voor niet werken
    
  // theorie interrupters


  SREG |= (1<<SREG_I); // Allow interrupts to occur
  EIMSK |= (1<<INT5);  // Enable INT5

  // Configure INT5 to occur on falling edge
  EICRB |= (1<<ISC51); //ISC51 bit set
  EICRB &= ~(1<<ISC50); // ISC50 bit cleared

  while (1) {
    if ((~PINE & (1 << PE4))) { // WEST
      PORTA = 255;
      while(1) {
      
      PORTC = 0b00000010;
      _delay_ms(0.7);
      PORTC = 0;
      _delay_ms(19.3);
      }
    }
    PORTC = 0;

    if ((~PINE & (1 << PE6))) { //EAST
      PORTA = 255;
      while(1){
      
      PORTC = 0b00000010;
      _delay_ms(1.5);
      PORTC = 0;
      _delay_ms(18.5);
      }
    }
    PORTC = 0;

    if ((~PINE & (1 << PE7))) { // NORTH
      PORTA = 255;
      while(1){
      PORTC = 0b00000010;
      _delay_ms(2.3);
      PORTC = 0;
      _delay_ms(17.8);
      }
    }
    PORTC = 0;

    if ((~PINE & (1 << PE5))) { //SOUTH
      PORTA = 255;
      while(1){
      PORTC = 0b00000010;
      _delay_ms(2.6);
      PORTC = 0;
      _delay_ms(16.9);
      }
    }
    PORTC = 0;

    
    PORTA = 0;
    _delay_ms(1);
  }


  
/*  prescaler
  PORTA = 255;
  TCCR1B &=~(0x07); //reset prescaler register to 0
  TCCR1B |=0x04; //set prescaler to 1/256 => aproxx 1 second
  TCNT1 = 0x0000;//reset timer to 0
    
*/


    
  return 0;
}






  







  #include "dwenguinoBoard.h"
#include "dwenguinoLCD.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile unsigned char bezig = 0;

volatile unsigned char interuptw=0;
volatile unsigned char interupte=0;

ISR(INT4_vect){ // Interrupt bij knop W (PE4)
  interuptw = 1; // Zet de interrupt-controle vlag
  bezig = 1;
}

ISR(INT6_vect){ // Interrupt bij knop E (PE6)
  interupte = 1; // Zet de interrupt-controle vlag
  bezig = 1;
}

int main(void) {
  DDRA = 0xFF; // Set all Port A pins as output 0b11111111
  PORTA = 0;   // Turn all LEDs off (they are connected to Port A)

  DDRE &= ~(1 << PE4); // Set PE4 as input        WEST
  PORTE |= (1 << PE4);  // Activate pull-up resistor on PE4
  DDRE &= ~(1 << PE6); // Set PE6 as input        EAST
  PORTE |= (1 << PE6);  // Activate pull-up resistor on PE6
  DDRE &= ~(1 << PE7); // Set PE4 as input        NORTH
  PORTE |= (1 << PE7);  // Activate pull-up resistor on PE4
  DDRE &= ~(1 << PE5); // Set PE6 as input        SOUTH
  PORTE |= (1 << PE5);  // Activate pull-up resistor on PE6

  SREG |= (1<<SREG_I); // Allow interrupts to occur
  EIMSK |= (1<<INT4);  // Enable INT4
  EICRB |= (1<<ISC41); //ISC41 bit set  Configure INT4 to occur on falling edge
  EICRB &= ~(1<<ISC40); // ISC40 bit cleared

  EIMSK |= (1<<INT6);  // Enable INT6
  EICRB |= (1<<ISC61); //ISC61 bit set  Configure INT6 to occur on falling edge
  EICRB &= ~(1<<ISC60); // ISC60 bit cleared


  DDRC |= (1 << PC0); // Set PC0 as output
  //PORTC |= (1 << PC0); // pull up weerstand
  
  DDRC |= (1 << PC1); // Set PC1 as output
  //PORTC |= (1 << PC1); // pullup weerstand maar dit zorgt voor niet werken
    
  

  while (1) {

    if (interuptw) { // WEST
      PORTA = 255;
      interuptw = 0;
      bezig = 0;
      while(1) {
        PORTC = 0b00000010;
        _delay_ms(0.7);
        PORTC = 0;
        _delay_ms(19.3);
        if (bezig){
          break;
        }
      }
    }

    if (interupte) { // EAST
      PORTA = 255;
      interupte = 0;
      bezig = 0;
      while(1) {
        PORTC = 0b00000001;
        _delay_ms(0.7);
        PORTC = 0;
        _delay_ms(19.3);
        if (bezig){
          break;
        }
      }
    }
    PORTC = 0;

    PORTA = 0; // Zet alle LEDs uit
    _delay_ms(1);
  }

  return 0;
}




// eindpresentatie: waarom gebruik je geen delays binnen in de interupts antw: timing klopt voor de balln