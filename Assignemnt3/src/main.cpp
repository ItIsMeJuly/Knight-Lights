#include <Arduino.h>

// masks
#define BUTTON_MASK_A (1 << PD5)
#define BUTTON_MASK_B (1 << PD2)
#define BUILT_IN_LED_MASK (1 << PB5)
#define LED_MASK (1 << PB0)
#define SWEEP_B 0b00011110
#define CLEAR_KNIGHT ~(0b00011110)

// timers
long long timer;
long long wait;

// masks for sweep part 2
int sweepBMask = 0b11110000000; // initial state of the sweep B
byte sweepAMask = 0b00010000;   // initial state of the sweep A

// direction flags for part 2
bool leftA = false;
bool leftB = false;

void setup()
{
  // SET DDRB as outputs
  DDRB |= LED_MASK;
  DDRB |= BUILT_IN_LED_MASK;
  DDRB |= SWEEP_B;

  // DDRD pin 2 and pin 5 as input
  DDRD &= ~(BUTTON_MASK_A | BUTTON_MASK_B);

  // DDRD pin 2 and pin 5 with PULLUP
  PORTD |= (BUTTON_MASK_A | BUTTON_MASK_B);

  // start timers
  wait = millis();
  timer = millis();
}

void loop()
{
  // Assignment A

  // if BUTTON A is pressed we continuosly keep the built-in LED on
  PIND &BUTTON_MASK_A ? PORTB &= ~BUILT_IN_LED_MASK : PORTB |= BUILT_IN_LED_MASK;

  // if 125ms have passed, toggle LED on pin 8
  millis() - timer > 125 ? PORTB ^= LED_MASK, timer = millis() : 0;

  // Assignment B

  // change light every 250ms
  if (millis() - wait > 250)
  {
    // if button B is not pressed we are in SWEEP B
    if (PIND & BUTTON_MASK_B)
    {
      // check direction in order to shift correctly
      if (!leftA)
        sweepAMask >>= 1;
      else
        sweepAMask <<= 1;

      // apply the mask to PORTB while preserving the state of the first led
      PORTB |= sweepAMask;

      // if end the end of the sequence is reached, change direction
      if (sweepAMask == 0b00010000 || sweepAMask == 0b10)
        leftA = !leftA;
    }

    // if button B is pressed
    else
    {
      // check direction in order to shift correctly
      if (!leftB)
        sweepBMask >>= 1;
      else
        sweepBMask <<= 1;

      // apply the mask to PORTB while preserving the state of the first led
      PORTB |= sweepBMask;

      // if end the end of the sequence is reached, change direction
      if (sweepBMask == 0b11110000000 || sweepBMask == 0b11110)
        leftB = !leftB;
    }

    wait = millis();
  }
}