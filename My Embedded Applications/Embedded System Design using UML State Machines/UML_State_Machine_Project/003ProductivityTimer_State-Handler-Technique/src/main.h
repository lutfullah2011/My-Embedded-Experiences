#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>


#define PIN_BUTTON1 2
#define PIN_BUTTON2 3
#define PIN_BUTTON3 4
#define PIN_BUZZER  12

//LCD connection MACROs
#define PIN_LCD_RS  5
#define PIN_LCD_RW  6
#define PIN_LCD_EN  7
#define PIN_LCD_D4  8
#define PIN_LCD_D5  9
#define PIN_LCD_D6  10
#define PIN_LCD_D7  11

#define BTN_PAD_VALUE_INC_TIME  4 // Three button are side to side. First: Inc_time, dec_time and Start_pause respectivly
#define BTN_PAD_VALUE_DEC_TIME  2
#define BTN_PAD_VALUE_CANCEL    6 //When you press INC & DEC button simultaneously
#define BTN_PAD_VALUE_SP        1


/*Button states enum for fixing de-bouncing*/
typedef enum{
    NOT_PRESSED,
    BOUNCE,
    PRESSED
}button_state_t;

/*Event status*/
typedef enum{
    EVENT_HANDLED,
    EVENT_IGNORED,
    EVENT_TRANSITION
}event_status_t;


/* Signals of the application*/
typedef enum{
    INC_TIME,
    DEC_TIME,
    TIME_TICK,
    START_PAUSE,
    CANCEL,
/*Internal activity signals (entry & exit)*/
    ENTRY,
    EXIT
}protimer_signal_t;

//forward declarations
struct protimer_tag;
struct event_tag;

//Function Pointer Declaration
typedef  event_status_t (*protimer_state_t)(struct protimer_tag  *const , struct event_tag const *const);


/* Main structure of the application */
typedef struct protimer_tag {
    uint32_t curr_time; // Mevcut zaman
    uint32_t elapsed_time; //Geçen süre
    uint32_t pro_time; //Verimli(üretken) zaman
    protimer_state_t active_state; //Function Pointer Variable 
}protimer_t;


/*Generic(Super) event structure */
typedef struct event_tag{
    uint8_t sig; //Signals
}event_t;

/* For user generated events // This struct contain another struct(event_t)
 This situation called "Structure Embedding" or "Inheritance in OOP" */
typedef struct{
    event_t super;
}protimer_user_event_t;

/* For tick event(Generated from the system) */
typedef struct{
    event_t super;
    uint8_t sub_sec;
}protimer_tick_event_t;




void protimer_init(protimer_t *obj);
event_status_t protimer_state_machine(protimer_t *const obj, event_t const *const e);


#endif