/**** CONFIGURATION ****/
const int ACK_LED = LED_BUILTIN;
const int NACK_LED = 12;

const int ACK = 0x06;
const int NACK = 0x15;
const int LINK_TEST = 0x0D;

const int ACK_TIMEOUT = 10; // Seconds
const int NACK_COUNT_LIMIT = 2; // Seconds

const String END_STRING = "\r";
const int TIME_BETW_MSG = 2000;

int SEQUENCE = 0;

String signals[] = {
  "5&070211\"083516\"29\"{SEQ}\"050290620\"18140101001g",
  "5&070211\"083532\"34\"{SEQ}\"050341644\"18140101007g",
  "5&070211\"083645\"22\"{SEQ}\"050211447\"18140101002\"18357001001w",
  "5&070211\"083831\"08\"{SEQ}\"050080419\"18140100002U",
  "5&070211\"083953\"11\"{SEQ}\"050110874\"18140100004P",
  "5&070211\"084328\"05\"{SEQ}\"050055536\"18140101002Q",
  "5&070211\"084416\"06\"{SEQ}\"050067200\"18140101002G",
  "5&070211\"084420\"34\"{SEQ}\"050343797\"18140101002V",
  "5&070211\"084425\"31\"{SEQ}\"050311016\"18140101004H",
  "5&070211\"084512\"22\"{SEQ}\"050211688\"18113001003\"18313001003Y",
  "5&070211\"084553\"05\"{SEQ}\"050056150\"18140101003T",
  "5&070211\"084559\"11\"{SEQ}\"050111931\"18140101002P",
  "5&070211\"083507\"05\"{SEQ}\"050051416\"18140101004^",
  "5&070211\"083512\"26\"{SEQ}\"050260478\"18140101008l",
  "5&070211\"083516\"29\"{SEQ}\"050290620\"18140101001g",
  "5&070211\"083907\"05\"{SEQ}\"050056569\"18140101005`",
  "5&070211\"083907\"34\"{SEQ}\"050342486\"18140101003^",
  "5&070211\"083910\"16\"{SEQ}\"050160428\"18313001017Y",
  "5&070211\"083912\"18\"{SEQ}\"050180635\"18113101023\"",
  "5&070211\"083913\"11\"{SEQ}\"050110041\"18140101002D",
  "5&070211\"083919\"17\"{SEQ}\"050170324\"18313001009Z",
  "5&070211\"083930\"34\"{SEQ}\"050340240\"18140101002G"
};

/*********************/

String currentSignal = signals[0];
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
    sendHeartBeat();
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

String getSequence(){
  char *seq = (char *)"    ";
  sprintf(seq, "%04d", SEQUENCE);  
  return String(seq);
}

void sendSignal(int idx) {
  currentSignal = signals[idx];
  currentSignalIDX = idx;
  signalSendTime = millis();
  String seq = getSequence();
  String signal = String(signals[idx]);
  signal.replace("{SEQ}",seq);
  Serial.print(signal+END_STRING);
}

void sendHeartBeat() {
  Serial.print("01"+END_STRING);
}




/***** SETUP / LOOP SECTION *********/

void setup() {
  pinMode(ACK_LED, OUTPUT);
  pinMode(NACK_LED, OUTPUT);

  Serial.begin(600);
  while (!Serial) {}
  randomSeed(analogRead(0));
}

void loop() {

  
  if (state == NACK_RECEIVED) {    
    sendSignal(currentSignalIDX);
  }
   
  if (state == ACK_RECEIVED) {
    SEQUENCE++;
    delay(TIME_BETW_MSG);
    int newIDX = 0;
    while ( (newIDX = random(300) % 4) == currentSignalIDX){}    
    currentSignalIDX = newIDX;
    sendSignal(currentSignalIDX);    
  }

  state = WAITING_FOR_ACK;  
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

