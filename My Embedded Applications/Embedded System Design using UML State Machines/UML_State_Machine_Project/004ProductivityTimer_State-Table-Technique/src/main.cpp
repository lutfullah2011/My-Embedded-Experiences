#include "main.h"
#include "lcd.h"

static void protimer_event_dispatcher(protimer_t *const obj,event_t const *const e);
static uint8_t process_button_pad_value(uint8_t btn_pad_value);
static void display_init(void);
static void protimer_state_table_init(protimer_t *const obj);


/*Main application object created from main structure */
static protimer_t protimer;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // We are not going to use serial monitor but lets keep it
  display_init(); // LCD Init
  Serial.println("Productivity analysis Application");
  Serial.println("===========================");
  pinMode(PIN_BUTTON1,INPUT); 
  pinMode(PIN_BUTTON2,INPUT);
  pinMode(PIN_BUTTON3,INPUT);

  //Initialise the Productivity state machine with desired config and with state table approach
  protimer_state_table_init(&protimer);
  protimer_init(&protimer);
}


void loop() {
  // put your main code here, to run repeatedly:
  uint8_t b1,b2,b3,btn_pad_value;
  protimer_user_event_t ue; //User event struct tag
  static uint32_t current_time = millis(); // Take the initial time and keep it in "current_time" variable to calculate elapsed time
                                           //Since this variable static, it is not used for every loop, we are going to reset it later.
  static protimer_tick_event_t te; //System event struct tag generated from tick timer.

  //1. Read the button pad status
  b1 = digitalRead(PIN_BUTTON1); 
  b2 = digitalRead(PIN_BUTTON2);
  b3 = digitalRead(PIN_BUTTON3);

  btn_pad_value = (b1<<2)|(b2<<1)|b3; //Three button arranged in a row || 0 0 0 :

  //This function help to fix button de-bouncing with software 
  btn_pad_value = process_button_pad_value(btn_pad_value);


  //2.Generating event according to the pressed buttons 
  if(btn_pad_value){
    if(btn_pad_value == BTN_PAD_VALUE_INC_TIME){
      ue.super.sig = INC_TIME; //this is structure embedding or inheritance in OOP
    }else if(btn_pad_value == BTN_PAD_VALUE_DEC_TIME){
      ue.super.sig = DEC_TIME;
    }else if(btn_pad_value == BTN_PAD_VALUE_SP){
      ue.super.sig = START_PAUSE;
    }else if(btn_pad_value == BTN_PAD_VALUE_CANCEL){
      ue.super.sig = CANCEL;
    }

     //3.Use event dispatcher(sender) to distribute tasks.
    protimer_event_dispatcher(&protimer,&ue.super);
  }


  //4. Send the time tick event for every 100ms. What millis() function does is this, counting the miliseconds.
  //10 times millis() equals to 1 second!!
  if(millis() - current_time  >= 100){
    //100ms has passed since the application comes to this code line. 
    //Because we take the initial time and keep it in "current_time" variable to calculate elapsed time since the beginning of "void loop".
    current_time = millis(); //With this line, we reset the millis() count
    te.super.sig = TIME_TICK;
    if(++te.sub_sec > 10) te.sub_sec = 1; //every 1 second, reset the te.sub_sec element to 1.
    protimer_event_dispatcher(&protimer,&te.super);
  }

}




/*This function is a task distributor of events*/
static void protimer_event_dispatcher(protimer_t *const obj,event_t const *const e){
  
  event_status_t status; 
  protimer_state_t source, target;
  e_handler_t ehandler; //function pointer variable is declared.
  
  source = obj->active_state;
  ehandler = (e_handler_t) obj->state_table[obj->active_state * MAX_SIGNALS + e->sig];

  if(ehandler)// In state table, there is "NULL" events. Thats why we check.
    status = (*ehandler)(obj,e);//Jump to event handler Function with Function Pointer

  if(status == EVENT_TRANSITION){
    target = obj->active_state;
    event_t ee;
    //1. run the exit action for the source state
    ee.sig = EXIT;
    ehandler =(e_handler_t) obj->state_table[source*MAX_SIGNALS + EXIT];
    if(ehandler)// In state table, there is "NULL" events. Thats why we check.
      (*ehandler)(obj,&ee);//Jump to event handler Function with Function Pointer

    //2. run the entry action for the target state
    ee.sig = ENTRY;
    ehandler =(e_handler_t) obj->state_table[target * MAX_SIGNALS + ENTRY];
    if(ehandler)// In state table, there is "NULL" events. Thats why we check.
      (*ehandler)(obj,&ee);//Jump to event handler Function with Function Pointer
  }

} 


// State table Implementation with 2D Array([5][7]) according to the state table created pdf.
// First initialization, state table is ready in "state table" pointer variable declared inside the "protimer_t" struct
static void protimer_state_table_init(protimer_t *const obj)
{
  static e_handler_t protimer_state_table[MAX_STATES][MAX_SIGNALS] = { //Function pointer type 2D array
    // 2D array implemented also like that:  first, row number or name and than their initialization
    [IDLE]	  	  = {&IDLE_Inc_time,NULL,&IDLE_Time_tick,&IDLE_Start_pause,NULL,&IDLE_Entry,&IDLE_Exit}, // Given the function's adresses
    [TIME_SET] 	  = {&TIME_SET_Inc_time,&TIME_SET_Dec_time,NULL,&TIME_SET_Start_pause,&TIME_SET_Cancel,&TIME_SET_Entry,&TIME_SET_Exit},
    [COUNTDOWN] 	= {NULL,NULL,&COUNTDOWN_Time_tick,&COUNTDOWN_Start_pause,&COUNTDOWN_Cancel,NULL,COUNTDOWN_Exit},
    [PAUSE] 		  = {&PAUSE_Inc_time,&PAUSE_Dec_time,NULL,&PAUSE_Start_pause,&PAUSE_Cancel,&PAUSE_Entry,&PAUSE_Exit},
    [STAT] 		    = {NULL,NULL,&STAT_Time_tick,NULL,NULL,&STAT_Entry,STAT_Exit}
};

   // Storing state table pointer to use in main application "obj" for dispatcher function.
   obj->state_table = (uintptr_t*) &protimer_state_table[0][0]; // typecasted (uintptr_t*) state table base address given for initially

}



//This helper function help to fix button de-bouncing with software and return the btn_pad_value
// 3 States of button is NOT_PRESSED(Initially), BOUNCE, and PRESSED.
// There is two bouncing situation. First, when button pressed and second, when button released. 
static uint8_t process_button_pad_value(uint8_t btn_pad_value)
{
  static button_state_t btn_sm_state = NOT_PRESSED; //Initially button statemachine state is not pressed
  static uint32_t curr_time = millis(); //millis() indicates number of miliseconds. 10 times millis() equals to 1 second!!

  switch(btn_sm_state){
    case NOT_PRESSED:{
      if(btn_pad_value){ //If btn_pad_value is non-zero, It means there is a transition, It is pressed though.
        btn_sm_state = BOUNCE; //In this line, state transition to BOUNCE, look at there!!
        curr_time = millis(); // We are going to measure 50 ms for bouncing, thats why we ara using "curr_time". Time count has started!
      }
      break;
    }
    case BOUNCE:{
      if(millis() - curr_time >= 50 ){ //In BOUNCE state, we need to wait 50 ms for fixing bouncing
        //50ms has passed 
        if(btn_pad_value){ // If btn_pad_value is non-zero, transition happens through PRESSED State.
          btn_sm_state = PRESSED;
          return btn_pad_value; // Exactly here, function returns the value
        }
        else // If btn_pad_value is zero, transition happens through NOT_PRESSED State.
          btn_sm_state = NOT_PRESSED;
      }
      break;
    }
    case PRESSED:{
      if(!btn_pad_value){ // If btn_pad_value is zero, transition happens through BOUNCE State. This happens when button releasing bounce effect
        btn_sm_state = BOUNCE;
        curr_time = millis(); // start time tick 
      }
      break;
    }

  }

  return 0; // It returns 0, normally
}



/*This function set LCD CONFIG*/
static void display_init(void)
{
  lcd_begin(16,2); //First we need to use lcd_begin.
  lcd_clear();
  lcd_move_cursor_L_to_R(); // We are going to print "Set time" and the time.
  lcd_set_cursor(0,0);
  lcd_no_auto_scroll();
  lcd_cursor_off();
}



