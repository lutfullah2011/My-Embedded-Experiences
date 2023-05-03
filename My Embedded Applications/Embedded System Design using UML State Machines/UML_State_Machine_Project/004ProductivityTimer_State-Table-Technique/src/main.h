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


/* Signals of the application*/
typedef enum{
    INC_TIME,
    DEC_TIME,
    TIME_TICK,
    START_PAUSE,
    CANCEL,
/*Internal activity signals (entry & exit)*/
    ENTRY,
    EXIT,
    MAX_SIGNALS //It is created for to show number of signals. Its value is MAX_SIGNALS = 7
}protimer_signal_t;



/*STATE'S of the application*/
typedef enum{
    IDLE,
    TIME_SET,
    COUNTDOWN,
    PAUSE,
    STAT,
    MAX_STATES //It is created for to show number of states. Its value is MAX_STATES= 6
}protimer_state_t;


/* Main structure of the application */
typedef struct {
    uint32_t curr_time; // Mevcut zaman
    uint32_t elapsed_time; //Geçen süre
    uint32_t pro_time; //Verimli(üretken) zaman
    protimer_state_t active_state; //Structure tag created for states
    uintptr_t *state_table; // "uintptr_t" is unsigned integer data type to hold a pointer variables.
    //It is used to hold the address of 2D array!!
}protimer_t;


/*Generic(Super) event structure */
typedef struct{
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

/*Event status*/
typedef enum{
    EVENT_HANDLED,
    EVENT_IGNORED,
    EVENT_TRANSITION
}event_status_t;

//Function Pointer Declaration for Event Handlers
typedef event_status_t  (*e_handler_t)(protimer_t *const obj, event_t const *const e);

void protimer_init(protimer_t *obj);
event_status_t protimer_state_machine(protimer_t *const obj, event_t const *const e);


//Event handlers Prototypes for every STATE and their specific EVENTS
event_status_t IDLE_Inc_time(protimer_t *const obj, event_t const *const e);
event_status_t IDLE_Time_tick(protimer_t *const obj, event_t const *const e);
event_status_t IDLE_Start_pause(protimer_t *const obj, event_t const *const e);
event_status_t IDLE_Entry(protimer_t *const obj, event_t const *const e);
event_status_t IDLE_Exit(protimer_t *const obj, event_t const *const e);

event_status_t	COUNTDOWN_Start_pause(protimer_t *const obj, event_t const *const e);
event_status_t	COUNTDOWN_Cancel(protimer_t *const obj, event_t const *const e);
event_status_t  COUNTDOWN_Exit(protimer_t *const obj, event_t const *const e);
event_status_t  COUNTDOWN_Time_tick(protimer_t *const obj, event_t const *const e);

event_status_t PAUSE_Inc_time(protimer_t *const obj, event_t const *const e);
event_status_t PAUSE_Dec_time(protimer_t *const obj, event_t const *const e);
event_status_t PAUSE_Start_pause(protimer_t *const obj, event_t const *const e);
event_status_t PAUSE_Cancel(protimer_t *const obj, event_t const *const e);
event_status_t PAUSE_Entry(protimer_t *const obj, event_t const *const e);
event_status_t PAUSE_Exit(protimer_t *const obj, event_t const *const e);


event_status_t TIME_SET_Inc_time(protimer_t *const obj, event_t const *const e);
event_status_t TIME_SET_Dec_time(protimer_t *const obj, event_t const *const e);
event_status_t TIME_SET_Start_pause(protimer_t *const obj, event_t const *const e);
event_status_t TIME_SET_Cancel(protimer_t *const obj, event_t const *const e);
event_status_t TIME_SET_Entry(protimer_t *const obj, event_t const *const e);
event_status_t TIME_SET_Exit(protimer_t *const obj, event_t const *const e);

event_status_t STAT_Time_tick(protimer_t *const obj, event_t const *const e);
event_status_t STAT_Time_tick(protimer_t *const obj, event_t const *const e);
event_status_t STAT_Entry(protimer_t *const obj, event_t const *const e);
event_status_t STAT_Exit(protimer_t *const obj, event_t const *const e);

#endif