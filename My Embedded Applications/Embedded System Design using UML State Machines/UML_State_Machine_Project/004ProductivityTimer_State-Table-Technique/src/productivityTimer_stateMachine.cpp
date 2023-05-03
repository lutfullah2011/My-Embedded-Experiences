#include "main.h"
#include "lcd.h"


/*******************************************************************/

//In this project, STATE TABLE Approach used with the help of Function Pointers.
//Discrete event handlers used for every State using 2D Arrays.
	       
/*******************************************************************/


//Static(Helper)Function's Prototypes 
static void do_beep(void);
static void display_clear(void);
static void display_message(String s,uint8_t c , uint8_t r);
static void display_time(uint32_t time);


/*Application initial Function*/
void protimer_init(protimer_t *obj){
    event_t ee; // ee = Event entry 
    e_handler_t ehandler; //function pointer variable is declared.
    ee.sig = ENTRY;
    obj->active_state = IDLE;
    obj->pro_time = 0;
    //Initially, we set the pointer variable(ehandler) to IDLE state and Entry signal.
    //To do that,use 1D array table implementation technique. It is covered in the course
    ehandler = (e_handler_t) obj->state_table[IDLE * MAX_SIGNALS + ENTRY]; 
    (*ehandler)(obj,&ee); //Jump to event handler Function with Function Pointer
}




/*******************************************************************/
/*       Discrete Event Handlers with STATE TABLE APPROACH         */
/*******************************************************************/


/*Event Handlers of IDLE STATE return event status */
event_status_t IDLE_Inc_time(protimer_t *const obj, event_t const *const e){
    obj->curr_time += 60;
    obj->active_state = TIME_SET;
    return EVENT_TRANSITION;
}

event_status_t IDLE_Time_tick(protimer_t *const obj, event_t const *const e){
    if( ((protimer_tick_event_t *)(e))->sub_sec == 5){  //if time =500 ms;
        do_beep();                                      //Member element(e) of event_t structure is typcasted
        return EVENT_HANDLED;
    }
    //Any other events will be ignored
    return EVENT_IGNORED;
}

event_status_t IDLE_Start_pause(protimer_t *const obj, event_t const *const e){
    obj->active_state = STAT;
    return EVENT_TRANSITION;
}

event_status_t IDLE_Entry(protimer_t *const obj, event_t const *const e){
    obj->curr_time = 0;
    obj->elapsed_time = 0;
    display_time(0);
    display_message("Set",0,0);
    display_message("time",0,1);
    return EVENT_HANDLED;
}
event_status_t IDLE_Exit(protimer_t *const obj, event_t const *const e){
    display_clear();
    return EVENT_HANDLED;
}





/*Event Handlers of TIME_SET STATE return event status */
event_status_t TIME_SET_Inc_time(protimer_t *const obj, event_t const *const e){
    obj->curr_time += 60;
    display_time(obj->curr_time);
    return EVENT_HANDLED;
}

event_status_t TIME_SET_Dec_time(protimer_t *const obj, event_t const *const e){
    if(obj->curr_time >=60){
        obj->curr_time -= 60;
        display_time(obj->curr_time);
        return EVENT_HANDLED;
    }
    return EVENT_IGNORED;
}

event_status_t TIME_SET_Start_pause(protimer_t *const obj, event_t const *const e){
    if(obj->curr_time >=60){
        obj->active_state = COUNTDOWN;
        return EVENT_TRANSITION;
    }
    return EVENT_IGNORED;
}

event_status_t TIME_SET_Cancel(protimer_t *const obj, event_t const *const e){
    obj->active_state = IDLE;
    return EVENT_TRANSITION;
}

event_status_t TIME_SET_Entry(protimer_t *const obj, event_t const *const e){
    display_time(obj->curr_time);
    return EVENT_HANDLED;
}

event_status_t TIME_SET_Exit(protimer_t *const obj, event_t const *const e){
    display_clear();
    return EVENT_HANDLED;
}






/*Event Handlers of COUNTDOWN STATE return event status */
event_status_t	COUNTDOWN_Start_pause(protimer_t *const obj, event_t const *const e){
    obj->active_state = PAUSE;
    return EVENT_TRANSITION;
}

event_status_t	COUNTDOWN_Cancel(protimer_t *const obj, event_t const *const e){
    obj->active_state = IDLE;
    return EVENT_TRANSITION;
}

event_status_t  COUNTDOWN_Exit(protimer_t *const obj, event_t const *const e){
    obj->pro_time += obj->elapsed_time;
    obj->elapsed_time = 0;
    return EVENT_HANDLED;
}

event_status_t  COUNTDOWN_Time_tick(protimer_t *const obj, event_t const *const e){
    if(((protimer_tick_event_t*)(e))->sub_sec == 10){
        --obj->curr_time; //current time decremented
        ++obj->elapsed_time; //elapsed time incremented
        display_time(obj->curr_time);
        if(!obj->curr_time){ //if current time = 0
            obj->active_state = IDLE;
            return EVENT_TRANSITION;
        }
        return EVENT_HANDLED;
    }
    return EVENT_IGNORED;
}




/*Event Handlers of PAUSE STATE return event status */
event_status_t PAUSE_Inc_time(protimer_t *const obj, event_t const *const e){
    obj->curr_time += 60;
    obj->active_state = TIME_SET;
    return EVENT_TRANSITION;
}

event_status_t PAUSE_Dec_time(protimer_t *const obj, event_t const *const e){
    if(obj->curr_time >= 60){
        obj->curr_time -= 60;
        obj->active_state = TIME_SET;
        return EVENT_TRANSITION;
    }
    return EVENT_IGNORED;
}

event_status_t PAUSE_Start_pause(protimer_t *const obj, event_t const *const e){
    obj->active_state = COUNTDOWN;
    return EVENT_TRANSITION;
}

event_status_t PAUSE_Cancel(protimer_t *const obj, event_t const *const e){
    obj->active_state = IDLE;
    return EVENT_TRANSITION;
}

event_status_t PAUSE_Entry(protimer_t *const obj, event_t const *const e){
    display_message("Paused",5,1);
    return EVENT_HANDLED;
}

event_status_t PAUSE_Exit(protimer_t *const obj, event_t const *const e){
    display_clear();
    return EVENT_HANDLED;
}







/*Event Handlers of STAT STATE return event status */
event_status_t STAT_Time_tick(protimer_t *const obj, event_t const *const e){
    static int tick_count = 0;
    if(++tick_count == 30){ //Stay 3 second in the STAT State
        tick_count = 0;
        obj->active_state = IDLE;
        return EVENT_TRANSITION;
    }
    return EVENT_IGNORED;
}

event_status_t STAT_Entry(protimer_t *const obj, event_t const *const e){
    display_time(obj->pro_time);
    display_message("Productive Time",1,1);
    return EVENT_HANDLED;
}

event_status_t STAT_Exit(protimer_t *const obj, event_t const *const e){
    display_clear();
    return EVENT_HANDLED;
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


