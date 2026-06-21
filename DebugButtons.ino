const int pins[] = {3, 5, 10, 17, 18, 36};
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

void printEvent(const char* eventName, uint16_t sig, uint16_t idle, uint16_t eventBits) {
  Serial.print("Evento: ");
  Serial.println(eventName);

  Serial.print("Assinatura: 0x");
  Serial.println(sig, HEX);

  Serial.print("Repouso base: 0x");
  Serial.println(idle, HEX);

  Serial.print("Bits do evento: 0x");
  Serial.println(eventBits, HEX);

  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  idleSig = scanSignature();

  Serial.print("Repouso inicial: 0x");
  Serial.println(idleSig, HEX);

  Serial.println("=== Renault SWC Interpreter v3 ===");
}

void loop() {
  uint16_t sig = scanSignature();

  if (isIdleState(sig)) {
    if (sig != idleSig) {
      idleSig = sig;
      Serial.print("Roda / novo repouso: 0x");
      Serial.println(idleSig, HEX);
    }

    lastSig = sig;
    delay(10);
    return;
  }

  uint16_t eventBits = sig & ~idleSig;

  if (sig != lastSig && millis() - lastEventTime > 120) {

  if (eventBits == 0x100 || eventBits == 0x120) {
    printEvent("VOL+", sig, idleSig, eventBits);

  } else if (eventBits == 0x080 || eventBits == 0x0A0) {
    printEvent("VOL-", sig, idleSig, eventBits);

  } else if (eventBits == 0x040 || eventBits == 0x060) {
    printEvent("POWER", sig, idleSig, eventBits);

  } else if (eventBits == 0x008 || eventBits == 0x00A) {
    printEvent("SOURCE L", sig, idleSig, eventBits);

  } else if (eventBits == 0x004 || eventBits == 0x006) {
    printEvent("SOURCE R", sig, idleSig, eventBits);

  } else if (eventBits == 0x010 || eventBits == 0x012) {
    printEvent("MUTE", sig, idleSig, eventBits);

    } else {
      Serial.print("Evento desconhecido - Assinatura: 0x");
      Serial.print(sig, HEX);
      Serial.print(" | Repouso: 0x");
      Serial.print(idleSig, HEX);
      Serial.print(" | Bits: 0x");
      Serial.println(eventBits, HEX);

      Serial.println("Use esse HEX para mapear Source/Mute.");
      Serial.println();
    }

    lastEventTime = millis();
  }

  lastSig = sig;
  delay(10);
}