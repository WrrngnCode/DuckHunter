
#ifndef UTILS_H_
#define UTILS_H_

#define TOGGLE_BUILTIN_LED PORTB ^= (1 << PORTB5)
#define TOGGLE_PIN12 PORTB ^= (1 << PORTB4)

// Turn prescaler on/off to stop the timer
#define TOGGLE_T1_ON_OFF TCCR1B ^= ((1 << CS12) | (1 << CS10))
#define TOGGLE_T2_ON_OFF TCCR2B ^= ((1 << CS22) | (1 << CS21) | (1 << CS20))

#endif



