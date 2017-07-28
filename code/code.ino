
/**** CONFIGURATION ****/
int ACK_LED = LED_BUILTIN;
int NACK_LED = 12;

int ACK = 0x06;
int NACK = 0x15;
int LINK_TEST = 0x0D;

int ACK_TIMEOUT = 10; // Seconds
int NACK_COUNT_LIMIT = 2; // Seconds

String signals[] = {
  "5&070211\"083516\"29\"9964\"050290620\"18140101001g",
  "5&070211\"083532\"34\"9970\"050341644\"18140101007g",
  "5&070211\"083645\"22\"9989\"050211447\"18140101002\"18357001001w",
  "5&070211\"083831\"08\"0019\"050080419\"18140100002U",
  "5&070211\"083953\"11\"0050\"050110874\"18140100004P",
  "5&070211\"084328\"05\"0112\"050055536\"18140101002Q",
  "5&070211\"084416\"06\"0130\"050067200\"18140101002G",
  "5&070211\"084420\"34\"0131\"050343797\"18140101002V",
  "5&070211\"084425\"31\"0134\"050311016\"18140101004H",
  "5&070211\"084512\"22\"0154\"050211688\"18113001003\"18313001003Y",
  "5&070211\"084553\"05\"0166\"050056150\"18140101003T",
  "5&070211\"084559\"11\"0170\"050111931\"18140101002P"
};

/*********************/

String currentSignal = "";
int currentSignalIDX = -1;
unsigned long signalSendTime = 0;  // The last time when a signal was sended
int nackCount = 0;
enum State {
  BEGIN,
  WAITING_FOR_ACK,
  ACK_RECEIVED,
  NACK_RECEIVED,
  COMPUTER_THROUBLE
} state;

/*********************/

void onACKReceived() {
  showACKReceived();
  state = ACK_RECEIVED;  
}

void onNACKReceived() {
  if (nackCount > NACK_COUNT_LIMIT) {
    state = COMPUTER_THROUBLE;
  } else {
    showNACKReceived();
    state = NACK_RECEIVED;    
  }
}

void showACKReceived() {
  showLedTransition(ACK_LED, NACK_LED, 2000);
}

void showNACKReceived() {
  showLedTransition(NACK_LED, ACK_LED, 2000);
}

void showLedTransition(int ledUp, int ledDown, int delayTime) {
  digitalWrite(ledDown, LOW);
  digitalWrite(ledUp, HIGH);
  delay(delayTime);
  digitalWrite(ledUp, LOW);
}

void sendSignal(int idx) {
  currentSignal = signals[idx];
  currentSignalIDX = idx;
  signalSendTime = millis();
  Serial.print(signals[idx]);
}

void sendHeartBeat() {
  Serial.print("01\r\n");
}




/***** SETUP / LOOP SECTION *********/

void setup() {
  pinMode(ACK_LED, OUTPUT);
  pinMode(NACK_LED, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {}
  randomSeed(analogRead(0));
}

void loop() {
  if (state == ACK_RECEIVED) {
    currentSignalIDX = random(300) % 4;
  }

  sendSignal(currentSignalIDX);
  state = WAITING_FOR_ACK;
  sendSignal(currentSignalIDX);
  while (true) {
    if (Serial.available() > 0) {
      int inByte = Serial.read();
      if (inByte == ACK) {
        onACKReceived();
        break;
      } else if (inByte == NACK) {
        onNACKReceived();
        break;
      }
    }
  }

}





