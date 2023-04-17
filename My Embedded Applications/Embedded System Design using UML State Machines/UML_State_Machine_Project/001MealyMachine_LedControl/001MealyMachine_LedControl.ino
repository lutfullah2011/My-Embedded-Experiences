/*
 * This application use Mealy machine to implement an LED Control 
 * changing intensity by PWM signal
 * 
 * Nested Switch method is used to implement state machine handler
 * 
 */


#define PIN_LED 9

#define LIGHT_BRIGHT_LOW   25
#define LIGHT_BRIGHT_MED   125
#define LIGHT_BRIGHT_HIGH  255
#define LIGHT_BRIGHT_OFF   0

enum event{
  ON,
  OFF
};

typedef enum {
  LIGHT_OFF,
  LIGHT_LOW,
  LIGHT_MEDIUM,
  LIGHT_HIGH
}light_state_t;

light_state_t curr_state = LIGHT_OFF;


void light_state_machine(uint8_t event);
void light_change_intensity(uint8_t pin, uint8_t intensity);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Light Application Test");
  Serial.println("-------------------------");
  Serial.println("Send 'a' for ON or 'k' for OFF ");
}

void loop() {
  uint8_t event;
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0){
    event = Serial.read();
    if(event == 'a'){
      light_state_machine(ON);
    }else if(event == 'k'){
      light_state_machine(OFF);
    }
  }

}

void light_state_machine(uint8_t event)
{
  
  switch(curr_state)
  {
    case LIGHT_OFF:{
      switch(event){
        case ON:{
          light_change_intensity(PIN_LED,LIGHT_BRIGHT_LOW);
          curr_state = LIGHT_LOW;
          break;
        }
      }
      break;
    }
    case LIGHT_LOW:{
      switch(event){
        case ON:{
          light_change_intensity(PIN_LED,LIGHT_BRIGHT_MED);
          curr_state = LIGHT_MEDIUM;
          break;
        }
        case OFF:{
          light_change_intensity(PIN_LED,LIGHT_BRIGHT_OFF);
          curr_state = LIGHT_OFF;
          break;
        }
      }
      break;
    }
    case LIGHT_MEDIUM:{
      switch(event){
        case ON:{
          light_change_intensity(PIN_LED,LIGHT_BRIGHT_HIGH);
          curr_state = LIGHT_HIGH;
          break;
        }
        case OFF:{
          light_change_intensity(PIN_LED,LIGHT_BRIGHT_OFF);
          curr_state = LIGHT_OFF;         
          break;
        }
      }
      break;
    }
    case LIGHT_HIGH:{
      switch(event){
        case ON:{
          light_change_intensity(PIN_LED,LIGHT_BRIGHT_LOW);
          curr_state = LIGHT_LOW;         
          break;
        }
        case OFF:{
          light_change_intensity(PIN_LED,LIGHT_BRIGHT_OFF);
          curr_state = LIGHT_OFF;          
          break;
        }
      }
      break;
    }
  }
}

void light_change_intensity(uint8_t pin, uint8_t intensity)
{
  analogWrite(pin,intensity);
}
