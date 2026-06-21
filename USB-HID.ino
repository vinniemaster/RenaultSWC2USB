#include "USB.h"
#include "USBHIDConsumerControl.h"

USBHIDConsumerControl Consumer;

const int pins[] = {3, 5, 10, 17, 18, 36};
// Preto, Vermelho, Marrom, Verde, Amarelo, Cinza
const int N = 6;

uint16_t idleSig = 0;
uint16_t lastSig = 0xFFFF;
unsigned long lastEventTime = 0;

uint16_t scanSignature() {
  bool connected[N][N] = {};

  for (int drive = 0; drive < N; drive++) {
    for (int i = 0; i < N; i++) pinMode(pins[i], INPUT_PULLUP);

    pinMode(pins[drive], OUTPUT);
    digitalWrite(pins[drive], LOW);
    delayMicroseconds(1500);

    for (int read = 0; read < N; read++) {
      if (read == drive) continue;

      if (digitalRead(pins[read]) == LOW) {
        int a = min(drive, read);
        int b = max(drive, read);
        connected[a][b] = true;
      }
    }

    pinMode(pins[drive], INPUT_PULLUP);
  }

  uint16_t sig = 0;
  int idx = 0;

  for (int i = 0; i < N; i++) {
    for (int j = i + 1; j < N; j++, idx++) {
      if (connected[i][j]) sig |= (1 << idx);
    }
  }

  return sig;
}

bool isIdleState(uint16_t sig) {
  return sig == 0x200 || sig == 0x400 || sig == 0x800;
}

void sendHID(uint16_t key) {
  Consumer.press(key);
  delay(40);
  Consumer.release();
}

void handleEvent(uint16_t eventBits) {
  if (eventBits == 0x100 || eventBits == 0x120) {
    Serial.println("HID: VOL+");
    sendHID(HID_USAGE_CONSUMER_VOLUME_INCREMENT);

  } else if (eventBits == 0x080 || eventBits == 0x0A0) {
    Serial.println("HID: VOL-");
    sendHID(HID_USAGE_CONSUMER_VOLUME_DECREMENT);

  } else if (eventBits == 0x040 || eventBits == 0x060) {
    Serial.println("HID: PLAY/PAUSE");
    sendHID(HID_USAGE_CONSUMER_PLAY_PAUSE);

  } else if (eventBits == 0x008 || eventBits == 0x00A) {
    Serial.println("HID: PREVIOUS");
    sendHID(HID_USAGE_CONSUMER_SCAN_PREVIOUS_TRACK);

  } else if (eventBits == 0x004 || eventBits == 0x006) {
    Serial.println("HID: NEXT");
    sendHID(HID_USAGE_CONSUMER_SCAN_NEXT_TRACK);

  } else if (eventBits == 0x010 || eventBits == 0x012) {
    Serial.println("HID: MUTE");
    sendHID(HID_USAGE_CONSUMER_MUTE);

  } else {
    Serial.print("Evento desconhecido bits: 0x");
    Serial.println(eventBits, HEX);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Consumer.begin();
  USB.begin();

  idleSig = scanSignature();

  Serial.println("=== Renault SWC USB HID ===");
  Serial.print("Repouso inicial: 0x");
  Serial.println(idleSig, HEX);
}

void loop() {
  uint16_t sig = scanSignature();

  if (isIdleState(sig)) {
    idleSig = sig;
    lastSig = sig;
    delay(10);
    return;
  }

  uint16_t eventBits = sig & ~idleSig;

  if (sig != lastSig && millis() - lastEventTime > 180) {
    handleEvent(eventBits);
    lastEventTime = millis();
  }

  lastSig = sig;
  delay(10);
}
