/*

  Button sequences:
  0. YELLOW
  1. BLUE
  2. GREEN
  3. RED
  4. WHITE

  Speaker 1:       button 1 and remote 1
  Speaker 2:       button 2 and remote 2
  Speaker 3:       button 3 and remote 3
  Amplifier:       button 4 and remote 4

  Instruction
  Step 1: Press button 0 to start the learning protocol.
  Step 2: Now press remote button 1 to assign it to Relay 1.
  Step 3: Press button 0 again and then press remote button 2 to assign it to relay 2.
  Step 4: Consequently with remote button 3 and 4.
  Step 5: Press button 0 again to exit the learning protocol
  Now the remote can be used normally

*/




#include "IRremote.h"
#include "Button.h"
#include <EEPROM.h>
#include "TM1637.h"
#include "EzButton.h"

const int RECV_PIN = 2;
const int pinRelay1 = 3;
const int pinRelay2 = 4;
const int pinRelay3 = 5;
const int pinRelay4 = 6;

button btn1;
button btn2;
button btn3;
button btn4;

const int pinLearning = 7;
const int BT_PIN = 8;
unsigned long lastTime = millis();

uint32_t dataRemote1;
uint32_t dataRemote2;
uint32_t dataRemote3;
uint32_t dataRemote4;

bool learning = false;
bool ledState = false;

bool remote1State = false;
bool remote2State = false;
bool remote3State = false;
bool remote4State = false;

bool btn1State = false;
bool btn2State = false;
bool btn3State = false;
bool btn4State = false;

int indexRemote = 0;

#define PULLUP true
#define INVERT true
#define DEBOUNCE_MS 20

Button btnLearn(BT_PIN, PULLUP, INVERT, DEBOUNCE_MS);

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  EEPROM.begin();
  Serial.begin(115200);

  pinMode(pinRelay1, OUTPUT);
  pinMode(pinRelay2, OUTPUT);
  pinMode(pinRelay3, OUTPUT);
  pinMode(pinRelay4, OUTPUT);

  pinMode(pinLearning, OUTPUT);
  digitalWrite(pinRelay1, LOW);
  digitalWrite(pinRelay2, LOW);
  digitalWrite(pinRelay3, LOW);
  digitalWrite(pinRelay4, LOW);
  digitalWrite(pinLearning, LOW);

  //Initiate the reciver
  irrecv.blink13(true);
  irrecv.enableIRIn();

  //Extract the data from eeprom
  dataRemote1 = get_remote_eeprom(1);
  dataRemote2 = get_remote_eeprom(2);
  dataRemote3 = get_remote_eeprom(3);
  dataRemote4 = get_remote_eeprom(4);

  Serial.println(dataRemote1);
  Serial.println(dataRemote2);
  Serial.println(dataRemote3);
  Serial.println(dataRemote4);

  btn1.begin(9);
  btn2.begin(10);
  btn3.begin(11);
  btn4.begin(12);

}

void loop() {
  //=============================Button====================================
  if (btn1.debounce()) {
    btn1State = !btn1State;
  }
  if (btn2.debounce()) {
    btn2State = !btn2State;
  }
  if (btn3.debounce()) {
    btn3State = !btn3State;
  }
  if (btn4.debounce()) {
    btn4State = !btn4State;
  }
  //=============================Relay====================================
  if (btn1State != remote1State) {
    digitalWrite(pinRelay1, HIGH);
  } else {
    digitalWrite(pinRelay1, LOW);
  }
  if (btn2State != remote2State) {
    digitalWrite(pinRelay2, HIGH);
  } else {
    digitalWrite(pinRelay2, LOW);
  }
  if (btn3State != remote3State) {
    digitalWrite(pinRelay3, HIGH);
  } else {
    digitalWrite(pinRelay3, LOW);
  }
  if (btn4State != remote4State) {
    digitalWrite(pinRelay4, HIGH);
  } else {
    digitalWrite(pinRelay4, LOW);
  }
  //=============================Remote====================================
  btnLearn.read();
  if (irrecv.decode(&results)) {
    uint32_t dataRemote = results.value;
    if (dataRemote > 0 && dataRemote != REPEAT) {
      if (millis() - lastTime > 250) {

        if (!learning) {                                                                //Control part
          if (dataRemote == dataRemote1) {
            remote1State = !remote1State;
          } else if (dataRemote == dataRemote2) {
            remote2State = !remote2State;
          } else if (dataRemote == dataRemote3) {
            remote3State = !remote3State;
          } else if (dataRemote == dataRemote4) {
            remote4State = !remote4State;
          }
        }
        else {
          save_remote_eeprom(indexRemote, dataRemote);
        }
      }
    }
    lastTime = millis();
    irrecv.resume(); // Allow to recive new data
  }
  if (btnLearn.wasPressed()) {                                                       //Learning part
    if (!learning) {
      learning = true;
      indexRemote = 1;
      digitalWrite(pinLearning, HIGH);
      digitalWrite(pinRelay1, HIGH);
      digitalWrite(pinRelay2, LOW);
      digitalWrite(pinRelay3, LOW);
      digitalWrite(pinRelay4, LOW);
    }
    else {
      indexRemote++;
      switch (indexRemote) {
        case 1:
          digitalWrite(pinRelay1, HIGH);
          digitalWrite(pinRelay2, LOW);
          digitalWrite(pinRelay3, LOW);
          digitalWrite(pinRelay4, LOW);
          break;
        case 2:
          digitalWrite(pinRelay1, LOW);
          digitalWrite(pinRelay2, HIGH);
          digitalWrite(pinRelay3, LOW);
          digitalWrite(pinRelay4, LOW);
          break;
        case 3:
          digitalWrite(pinRelay1, LOW);
          digitalWrite(pinRelay2, LOW);
          digitalWrite(pinRelay3, HIGH);
          digitalWrite(pinRelay4, LOW);
          break;
        case 4:
          digitalWrite(pinRelay1, LOW);
          digitalWrite(pinRelay2, LOW);
          digitalWrite(pinRelay3, LOW);
          digitalWrite(pinRelay4, HIGH);
          break;
      }
      if (indexRemote > 4) {
        learning = false;
        dataRemote1 = get_remote_eeprom(1);
        dataRemote2 = get_remote_eeprom(2);
        dataRemote3 = get_remote_eeprom(3);
        dataRemote4 = get_remote_eeprom(4);

        digitalWrite(pinLearning, LOW);
        digitalWrite(pinRelay1, LOW);
        digitalWrite(pinRelay2, LOW);
        digitalWrite(pinRelay3, LOW);
        digitalWrite(pinRelay4, LOW);
      }
    }
  }
}
//==============================================================
void save_remote_eeprom(int pos, uint32_t data) {
  int address = pos * 4;
  EEPROM.write(address, ((byte *)&data)[0]);
  EEPROM.write(address + 1, ((byte *)&data)[1]);
  EEPROM.write(address + 2, ((byte *)&data)[2]);
  EEPROM.write(address + 3, ((byte *)&data)[3]);
}
//=============================================================
uint32_t get_remote_eeprom(int pos) {
  int address = pos * 4;
  uint32_t data = 0;
  ((byte *)&data)[0] = EEPROM.read(address);
  ((byte *)&data)[1] = EEPROM.read(address + 1);
  ((byte *)&data)[2] = EEPROM.read(address + 2);
  ((byte *)&data)[3] = EEPROM.read(address + 3);
  return data;
}