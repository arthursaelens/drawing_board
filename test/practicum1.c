#include <avr/io.h>
#include <util/delay.h>

// Practicum 1

int oef1(void) {
  DDRA = 0xFF; // Set all Port A pins as output 0b11111111
  PORTA = 0;   // Turn all LEDS off (they are connected to Port A)

  DDRE &= ~((1 << PE4) | (1 << PE6)); // Set all Port A pins as input 
  PORTE |= (1 << PE4) | (1 << PE6);  // Activate pull-up resistors on PE4 and PE6
  //PORTE &= ~((1 << PE4) | (1 << PE6)); // Deactivate pull-up resistors on PE4 and PE6
  while(1){
  if (!( PINE & 1 << PE4  )){ // controlleren of de westknop is ingedrukt
     PORTA |= 0b10000000; // Toggle some of the the LEDs  
    _delay_ms(0);
  } 
  
  if (!(PINE & 1<< PE6)){
     PORTA |= 0b00000001; 
    _delay_ms(0);
  } 
  }
  
  return 0;
}

int oef2(void) {
    DDRA = 0xFF; // Set all Port A pins as output 0b11111111
    PORTA = 0;   // Turn all LEDS off (they are connected to Port A)

    DDRE &= ~(1 << PE5); // Set PE5 as input 
    PORTE |= (1 << PE5);  // Activate pull-up resistor on PE5

    int button_pressed = 0; // Variable to track if the button was recently pressed

    while(1) {
        if ((~PINE & (1 << PE5))) { // Check if the PE5 button is pressed
            if (!button_pressed) { // Check if the button was not recently pressed
                PORTA ^= 0xFF; // Toggle all LEDs
                button_pressed = 1; // Update the button_pressed variable
                _delay_ms(50);
            }
        } else {
            button_pressed = 0; // Reset the button_pressed variable if the button is not pressed
            _delay_ms(50);
        }
    }
    return 0;
}

int oef3(void) {
    DDRA = 0xFF; // Set all Port A pins as output 0b11111111
    PORTA = 0;   // Turn all LEDS off (they are connected to Port A)

    DDRE &= ~(1 << PE5); // Set PE5 as input 
    PORTE |= (1 << PE5);  // Activate pull-up resistor on PE5

    int button_pressed = 0; // Variable to track if the button was recently pressed
    int counter = 0b00000000;

    while(1) {
        if ((~PINE & (1 << PE5))) { // Check if the PE5 button is pressed
            if (!button_pressed) { // Check if the button was not recently pressed
                counter++;
                PORTA = counter; // Toggle all LEDs
                button_pressed = 1; // Update the button_pressed variable
                _delay_ms(50);
            }
        } else {
            button_pressed = 0; // Reset the button_pressed variable if the button is not pressed
            _delay_ms(50);
        }
    }
    return 0;
}

int oef4(void) {
    DDRA = 0xFF; // Set all Port A pins as output 0b11111111
    PORTA = 0;   // Turn all LEDS off (they are connected to Port A)

    DDRE &= ~(1 << PE5); // Set PE5 as input 
    PORTE |= (1 << PE5);  // Activate pull-up resistor on PE5

    int button_pressed = 1;
    int counter = 0b00000001;
    int to_left = 1;


    while(1) {
        
        if ((~PINE & (1 << PE5))) { // Check if the PE5 button is pressed
            if (!button_pressed) { // Check if the button was not recently pressed
                button_pressed = 1; // Update the button_pressed variable
                _delay_ms(70);
            
            } else {
            button_pressed = 0; // Reset the button_pressed variable if the button is not pressed
            _delay_ms(70);
            }
        }

        if (button_pressed){
        if (to_left == 1) {
          PORTA = counter;
          counter <<= 1;
          _delay_ms(70);
          if (counter == 0b10000000){
            to_left = 0;
          }
        }
        if (to_left == 0) {
          PORTA = 255;
          PORTA = counter;
          counter >>= 1;
          _delay_ms(70);
          if (counter == 0b00000001){
            to_left = 1;
          }
        }
        }
    }
    return 0;
}
