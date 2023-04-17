#include "main.h"
#include "lcd.h"


//Static(Helper)Function's Prototypes 
static void do_beep(void);
static void display_clear(void);
static void display_message(String s,uint8_t c , uint8_t r);
static void display_time(uint32_t time);

//STATE Handler's Prototypes
static event_status_t protimer_state_handler_IDLE(protimer_t  *const obj, event_t const *const e);
static event_status_t protimer_state_handler_TIME_SET(protimer_t  *const obj, event_t const *const e);
static event_status_t protimer_state_handler_COUNTDOWN(protimer_t  *const obj, event_t const *const e);
static event_status_t protimer_state_handler_PAUSE(protimer_t  *const obj, event_t const *const e);
static event_status_t protimer_state_handler_STAT(protimer_t  *const obj, event_t const *const e);


/*Application initial Function*/
void protimer_init(protimer_t *obj){
    event_t ee; // ee = Event entry 
    ee.sig = ENTRY;
    obj->active_state = IDLE;
    obj->pro_time = 0;
    protimer_state_machine(obj,&ee);
}


/*STATE MACHINE Implementation with Nested Switch Case*/
event_status_t protimer_state_machine(protimer_t *const obj, event_t const *const e){
    switch (obj->active_state){
        case IDLE:
            return protimer_state_handler_IDLE(obj,e);
        case TIME_SET:
            return protimer_state_handler_TIME_SET(obj,e);
        case COUNTDOWN:
            return protimer_state_handler_COUNTDOWN(obj,e);
        case PAUSE:
            return protimer_state_handler_PAUSE(obj,e);
        case STAT:
            return protimer_state_handler_STAT(obj,e);
    }
    return EVENT_IGNORED;
}


/*Helper function Handler of IDLE STATE return event status */
static event_status_t protimer_state_handler_IDLE(protimer_t *const obj, event_t const *const e){
    
    switch(e->sig){
        case ENTRY:{
            obj->curr_time = 0;
            obj->elapsed_time = 0;
            display_time(0);
            display_message("Set",0,0);
            display_message("time",0,1);
            return EVENT_HANDLED;
        }
        case EXIT:{
            display_clear();
            return EVENT_HANDLED;
        }
        case INC_TIME:{
            obj->curr_time += 60;
            obj->active_state = TIME_SET;
            return EVENT_TRANSITION;
        }
        case START_PAUSE:{
            obj->active_state = STAT;
            return EVENT_TRANSITION;
        }
        case TIME_TICK:{
            if( ((protimer_tick_event_t *)(e))->sub_sec == 5){ //if time =500 ms;
                do_beep();                                     //Member element(e) of event_t structure is typcasted
                
                return EVENT_HANDLED;
            }

            return EVENT_IGNORED;
        }
    }

    //Since switch ended, any other events will be ignored
    return EVENT_IGNORED;
}


/*Helper function Handler of TIME_SET STATE return event status */
static event_status_t protimer_state_handler_TIME_SET(protimer_t *const obj, event_t const *const e){
    switch(e->sig){
        case ENTRY:{
            display_time(obj->curr_time);
            return EVENT_HANDLED;
        }
        case EXIT:{
            display_clear();
            return EVENT_HANDLED;
        }
        case INC_TIME:{
            obj->curr_time += 60;
            display_time(obj->curr_time);
            return EVENT_HANDLED;
        }
        case DEC_TIME:{
            if(obj->curr_time >=60){
                obj->curr_time -= 60;
                display_time(obj->curr_time);
                return EVENT_HANDLED;
            }
            return EVENT_IGNORED;
        }
        case CANCEL:{
            obj->active_state = IDLE;
            return EVENT_TRANSITION;
        }
        case START_PAUSE:{
            if(obj->curr_time >=60){
                obj->active_state = COUNTDOWN;
                return EVENT_TRANSITION;
            }
            return EVENT_IGNORED;
        }
    }

    return EVENT_IGNORED;
}


/*Helper function Handler of COUNTDOWN STATE return event status */
static event_status_t protimer_state_handler_COUNTDOWN(protimer_t *const obj, event_t const *const e){
    switch(e->sig){
        case EXIT:{
            obj->pro_time += obj->elapsed_time;
            obj->elapsed_time = 0;
            return EVENT_HANDLED;
        }

        case TIME_TICK:{
            if(((protimer_tick_event_t*)(e))->sub_sec == 10){
                --obj->curr_time; //current time decremented
                ++obj->elapsed_time;//elapsed time incremented
                display_time(obj->curr_time);
                if(!obj->curr_time){ //if current time = 0
                    obj->active_state = IDLE;
                    return EVENT_TRANSITION;
                }
                return EVENT_HANDLED;
            }
            return EVENT_IGNORED;
        }
        case START_PAUSE:{
            obj->active_state = PAUSE;
            return EVENT_TRANSITION;
        }
        case CANCEL:{
            obj->active_state = IDLE;
            return EVENT_TRANSITION;
        }
    }
    return EVENT_IGNORED;
}


/*Helper function Handler of PAUSED STATE return event status */
static event_status_t protimer_state_handler_PAUSE(protimer_t *const obj, event_t const *const e){
    switch(e->sig){
        case ENTRY:{
            display_message("Paused",5,1);
            return EVENT_HANDLED;
        }
        case EXIT:{
            display_clear();
            return EVENT_HANDLED;
        }
        case INC_TIME:{
            obj->curr_time += 60;
            obj->active_state = TIME_SET;
            return EVENT_TRANSITION;
        }
        case DEC_TIME:{
            if(obj->curr_time >= 60){
                obj->curr_time -= 60;
                obj->active_state = TIME_SET;
                return EVENT_TRANSITION;
            }
            return EVENT_IGNORED;
        }
        case START_PAUSE:{
            obj->active_state = COUNTDOWN;
            return EVENT_TRANSITION;
        }
        case CANCEL:{
            obj->active_state = IDLE;
            return EVENT_TRANSITION;
        }
    }
    return EVENT_IGNORED;
}


/*Helper function Handler of STAT STATE return event status */
static event_status_t protimer_state_handler_STAT(protimer_t *const obj, event_t const *const e){
    static uint8_t tick_count;
    
    switch(e->sig){
        case ENTRY:{
            display_time(obj->pro_time);
            display_message("Productive time",1,1);
            return EVENT_HANDLED;
        }
        case EXIT:{
            display_clear();
            return EVENT_HANDLED;
        }
        case TIME_TICK:{
            if(++tick_count == 30){ //Stay 3 second in the STAT State
                tick_count = 0;
                obj->active_state = IDLE;
                return EVENT_TRANSITION;
            }
            return EVENT_IGNORED;
        }
    }
    return EVENT_IGNORED;
}





/*******************************************************************/
/*         Static(Helper)Functions by this driver - APIs	       */
/*******************************************************************/

/*Display time starting 5th column, write "Set time" in the beginning*/
static void display_time(uint32_t time){
  char buf[7];
  String time_msg;
  
  uint16_t m = time / 60;
  uint8_t s = time % 60;
  sprintf(buf,"%03d:%02d",m,s);
  
  time_msg = (String)buf;
  lcd_set_cursor(5,0);
  lcd_print_string(time_msg);
}

static void display_message(String s,uint8_t c , uint8_t r){
    lcd_set_cursor(c,r);
    lcd_print_string(s);
}

static void display_clear(void){
    lcd_clear();
}

static void do_beep(void){
    tone(PIN_BUZZER, 4000, 25); //4khz and 25 ms 
}


