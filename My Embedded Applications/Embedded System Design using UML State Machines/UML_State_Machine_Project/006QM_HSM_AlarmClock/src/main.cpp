#include <Arduino.h>
#include "qpn.h"
#include "AlarmClock_HSM.h"
#include "lcd.h"
#include "main.h"

Q_DEFINE_THIS_FILE;

static void Timer1_setup(void);
static void display_init(void);
static uint8_t process_button_pad_value(uint8_t btn_pad_value);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  display_init();
  Alarm_Clock_ctor();
  QHSM_INIT(super_AlarmClock);
  Timer1_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  static uint32_t tick_time = millis();
  static uint32_t alarm_check_time = millis();
  uint8_t b1,b2,btn_pad_value;

  //1. read the button pad status
  b1 = digitalRead(PIN_BUTTON1); 
  b2 = digitalRead(PIN_BUTTON2);

  btn_pad_value = (b1<<1u)|b2;

  //software button de-bouncing 
  btn_pad_value = process_button_pad_value(btn_pad_value);

  while(millis() - tick_time >= 50 ){
    //send TICK event  every 50 ms
    tick_time = millis();
    Q_SIG(super_AlarmClock) = TICK_SIG; //Pointer of the super class derived by Q_SIG(super_AlarmClock)
    QHSM_DISPATCH(super_AlarmClock);
  }

  while(millis() - alarm_check_time >= 500 ){
    //send TICK event 
    alarm_check_time = millis();
    Q_SIG(super_AlarmClock) = ALARM_SIG;
    QHSM_DISPATCH(super_AlarmClock);
  }

    if(btn_pad_value){
      if(btn_pad_value == BTN_PAD_VALUE_SET){
        Q_SIG(super_AlarmClock) = SET_SIG; 
      }else if(btn_pad_value == BTN_PAD_VALUE_OK){
        Q_SIG(super_AlarmClock) = OK_SIG;
      }else if(btn_pad_value == BTN_PAD_VALUE_CANCEL){
        Q_SIG(super_AlarmClock) = CANCEL_SIG;
      }else 
      Q_SIG(super_AlarmClock) = IGNORE_SIG;
      
      QHSM_DISPATCH(super_AlarmClock);
    }
  
}



//Every 100ms , there will be interrupt using Timer1
static void Timer1_setup(void){
  TCCR1A = 0;                 //To select CTC mode, we need to configure TCCR1A register to 0.            
  TCCR1B = B00001100;        //prescaler=256,CTC mode
  TIMSK1 |= B00000010;       //Interrupt enable for OCR1A compare match (Timer Interrupt Mask Register)
  OCR1A = 6250-1;          //OC match value for 100ms time base generation
}


static void display_init(void)
{
  lcd_begin(16,2);
  lcd_clear();
  lcd_move_cursor_L_to_R();
  lcd_set_cursor(0,0);
  lcd_no_auto_scroll();
  lcd_cursor_off();
}


Q_NORETURN Q_onAssert	(	char_t const Q_ROM *const 	module,int_t const 	location ){

  Serial.println("Assertion failure!!");
  Serial.println((String)module);
  Serial.println(location);
  while(1);
}	


static uint8_t process_button_pad_value(uint8_t btn_pad_value)
{
  static button_state_t btn_sm_state = NOT_PRESSED;
  static uint32_t curr_time = millis();

  switch(btn_sm_state){
    case NOT_PRESSED:{
      if(btn_pad_value){
        btn_sm_state = BOUNCE;
        curr_time = millis();
      }
      break;
    }
    case BOUNCE:{
      if(millis() - curr_time >= 50 ){
        //50ms has passed 
        if(btn_pad_value){
          btn_sm_state = PRESSED;
          return btn_pad_value;
        }
        else
          btn_sm_state = NOT_PRESSED;
      }
      break;
    }
    case PRESSED:{
      if(!btn_pad_value){
        btn_sm_state = BOUNCE;
        curr_time = millis();
      }
      break;
    }

  }

  return 0;
}