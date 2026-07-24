/*
==========================================================
 ANTENNA TRACKER - PROGRAM FINAL v2 (BAGIAN 1-3)
----------------------------------------------------------
 Bagian 1 : Pin, variabel global, struct axis, setup(),
            loop(), baca PWM Pixhawk + failsafe, konversi
            PWM->sudut, konversi encoder->sudut signed,
            fungsi motor dasar (writeMotor/stopMotor/hardStop)
 Bagian 2 : PID PAN penuh (signed -180..+180, shortest-path
            yang AMAN terhadap hardstop, anti-windup
            conditional-integration, deadband halus +
            hysteresis, motor smoothing slew-rate,
            conditional braking)
 Bagian 3 : PID TILT penuh (0..90, logika sama seperti PAN
            tapi tanpa shortest-path), ISR encoder PAN/TILT,
            kalibrasi EEPROM, perintah Serial, telemetry
            debug lengkap

 CATATAN PENTING (WIRING TIDAK DIUBAH):
 Semua pin persis sama seperti program sebelumnya:
   PAN_PWM_PIN=2, TILT_PWM_PIN=3
   PAN_ENC_A=A4, PAN_ENC_B=A5, TILT_ENC_A=7, TILT_ENC_B=8
   PAN_RPWM=6, PAN_LPWM=5, TILT_RPWM=9, TILT_LPWM=10

 SHORTEST-PATH & HARDSTOP:
 PAN Anda punya hardstop fisik di sekitar -180/+180 derajat
 (tidak bisa berputar 360 penuh). Karena itu, PID PAN TIDAK
 melakukan wrap-around melewati titik -180/+180 (itu akan
 memaksa motor menabrak hardstop). Dengan rentang gerak yang
 dibatasi fisik seperti ini, error = target - posisi_sekarang
 SUDAH SECARA OTOMATIS merupakan jalur terpendek yang aman
 (tidak ada jalur alternatif yang legal secara mekanik).
 Fungsi wrap shortest-path generik tetap disediakan
 (wrapAngleDiff) dan bisa diaktifkan lewat flag
 PAN_CONTINUOUS_ROTATION jika suatu saat hardstop dilepas
 dan PAN dibuat bisa berputar 360 penuh (continuous rotation
 / slip-ring).

 MOTOR_MIN_PWM:
 Disetel untuk motor GM25-370 @ 12V melalui driver BTS7960.
 Nilai default (60/255) adalah titik awal yang wajar untuk
 mengatasi gesekan statis gearbox, TAPI WAJIB di-fine-tune
 di lapangan sesuai beban mekanik antena Anda (coba naik/
 turun di rentang 45-90 sambil amati motor mulai bergerak).
==========================================================
*/

#include <PinChangeInterrupt.h>
#include <EEPROM.h>

// ============================================================
// ================   BAGIAN 1   ================================
// Pin, variabel global, struct, setup(), loop(), baca PWM,
// konversi PWM->sudut, konversi encoder->sudut, fungsi motor dasar
// ============================================================

// ---------------- Konfigurasi Pin (TIDAK DIUBAH) ----------------
#define PAN_PWM_PIN     2
#define TILT_PWM_PIN    3

#define PAN_ENC_A       A4
#define PAN_ENC_B       A5
#define TILT_ENC_A      7
#define TILT_ENC_B      8

#define PAN_RPWM        5
#define PAN_LPWM        6
#define TILT_RPWM       9
#define TILT_LPWM       10

// ---------------- Failsafe & rentang sinyal PWM ----------------
#define PWM_MIN_VALID_US   900
#define PWM_MAX_VALID_US   2100
#define PWM_TIMEOUT_MS     1000     // sinyal Pixhawk hilang > ini -> failsafe

#define PWM_ANGLE_MIN_US   1000
#define PWM_ANGLE_MAX_US   2000

// Target PWM Pixhawk -> Sudut
#define PAN_ANGLE_MIN      -180.0   // PAN : signed -180..+180 (sesuai hardstop mekanik)
#define PAN_ANGLE_MAX       180.0
#define TILT_ANGLE_MIN      0.0     // TILT: tetap 0..90 (tidak signed)
#define TILT_ANGLE_MAX      90.0

// Set true HANYA jika suatu saat PAN dibuat continuous rotation
// (slip ring, tanpa hardstop). Default: false (ada hardstop).
#define PAN_CONTINUOUS_ROTATION false

// ---------------- Motor / PID tuning umum ----------------
#define MOTOR_MAX_PWM        255
#define MOTOR_MIN_PWM        60      // GM25-370 @ 12V via BTS7960 - FINE TUNE DI LAPANGAN
#define SLEW_RATE_STEP       8       // max perubahan PWM per siklus (smoothing)

// Deadband halus (dengan hysteresis supaya tidak chattering di batas)
#define PAN_DEADBAND_DEG        0.3
#define PAN_DEADBAND_HYST       0.15
#define TILT_DEADBAND_DEG       0.3
#define TILT_DEADBAND_HYST      0.15

#define INTEGRAL_LIMIT       150.0   // batas anti-windup (clamp)

// ---- Conditional braking ----
#define BRAKE_SPEED_THRESHOLD_DEGPS 15.0
#define BRAKE_PWM                   120
#define BRAKE_DURATION_MS           40

#define PID_INTERVAL_MS      20      // ~50Hz loop PID

// ---------------- Alamat EEPROM ----------------
#define EEPROM_MAGIC_ADDR     0
#define EEPROM_MAGIC_VALUE    0xA6   // dinaikkan supaya EEPROM lama (v1) tidak "termakan" sebagai valid

#define EEPROM_ADDR_PAN_CPD    2
#define EEPROM_ADDR_TILT_CPD   6
#define EEPROM_ADDR_PAN_ZERO   10
#define EEPROM_ADDR_TILT_ZERO  14
#define EEPROM_ADDR_KP_PAN     18
#define EEPROM_ADDR_KI_PAN     22
#define EEPROM_ADDR_KD_PAN     26
#define EEPROM_ADDR_KP_TILT    30
#define EEPROM_ADDR_KI_TILT    34
#define EEPROM_ADDR_KD_TILT    38

// ---------------- Struct kontrol per axis ----------------
struct AxisControl {
  uint8_t rpwmPin;
  uint8_t lpwmPin;

  int   currentOutput;     // output PWM signed yang sedang dikirim ke motor (-255..255)
  float integral;
  float lastError;
  float prevAngle;

  unsigned long lastUpdateTime;  // dt dihitung TERPISAH per-axis (perbaikan bug dt gabungan)
  bool  inDeadband;              // status deadband dengan hysteresis

  bool  braking;
  int   brakeDir;
  unsigned long brakeEndTime;

  // untuk debug telemetry
  float lastPTerm, lastITerm, lastDTerm;
  float lastTargetAngle, lastActualAngle, lastErrorDeg;
  float lastDt;
};

AxisControl panAxis  = {PAN_RPWM,  PAN_LPWM,  0, 0, 0, 0, 0, false, false, 0, 0, 0,0,0, 0,0,0, 0};
AxisControl tiltAxis = {TILT_RPWM, TILT_LPWM, 0, 0, 0, 0, 0, false, false, 0, 0, 0,0,0, 0,0,0, 0};

// ---------------- Encoder (ISR di Bagian 3) ----------------
volatile long panCount = 0;
volatile long tiltCount = 0;
volatile byte panState = 0;
volatile byte tiltState = 0;

// ---------------- PWM input Pixhawk ----------------
int panPWM = 1500;
int tiltPWM = 1500;
unsigned long lastPanPWMValid_ms = 0;
unsigned long lastTiltPWMValid_ms = 0;
bool panLost = false;
bool tiltLost = false;
bool failsafeActive = false;

// ---------------- Kalibrasi (default aman) ----------------
float panCountsPerDeg  = 10.0;
float tiltCountsPerDeg = 10.0;
long  panZeroOffset  = 0;
long  tiltZeroOffset = 0;

// ---------------- PID gain (default) ----------------
float Kp_pan = 2.0,  Ki_pan = 0.05,  Kd_pan = 0.3;
float Kp_tilt = 2.0, Ki_tilt = 0.05, Kd_tilt = 0.3;

// ---------------- Target sudut hasil konversi PWM ----------------
float panTargetAngle  = 0.0;
float tiltTargetAngle = 0.0;

unsigned long lastPIDTrigger = 0;   // hanya untuk pemicu loop ~50Hz, BUKAN sumber dt axis
unsigned long lastPrint = 0;

// ============================================================
// ============   SETUP   =========================================
// ============================================================
void setup() {
  Serial.begin(115200);

  pinMode(PAN_PWM_PIN, INPUT);
  pinMode(TILT_PWM_PIN, INPUT);

  pinMode(PAN_ENC_A, INPUT_PULLUP);
  pinMode(PAN_ENC_B, INPUT_PULLUP);
  pinMode(TILT_ENC_A, INPUT_PULLUP);
  pinMode(TILT_ENC_B, INPUT_PULLUP);

  pinMode(PAN_RPWM, OUTPUT);
  pinMode(PAN_LPWM, OUTPUT);
  pinMode(TILT_RPWM, OUTPUT);
  pinMode(TILT_LPWM, OUTPUT);
  stopMotor(panAxis);
  stopMotor(tiltAxis);

  attachPCINT(digitalPinToPCINT(PAN_ENC_A), panISR, CHANGE);
  attachPCINT(digitalPinToPCINT(PAN_ENC_B), panISR, CHANGE);
  attachPCINT(digitalPinToPCINT(TILT_ENC_A), tiltISR, CHANGE);
  attachPCINT(digitalPinToPCINT(TILT_ENC_B), tiltISR, CHANGE);

  loadCalibrationFromEEPROM();

  unsigned long now = millis();
  lastPanPWMValid_ms = now;
  lastTiltPWMValid_ms = now;
  lastPIDTrigger = now;
  panAxis.lastUpdateTime = now;
  tiltAxis.lastUpdateTime = now;

  Serial.println();
  Serial.println("=========================================");
  Serial.println("ANTENNA TRACKER - PROGRAM FINAL v2");
  Serial.println("Signed angle + shortest-path aman hardstop");
  Serial.println("=========================================");
  printCalibration();
  printSerialHelp();
  Serial.println();
}

// ============================================================
// ============   LOOP UTAMA   ====================================
// ============================================================
void loop() {
  handleSerialCommands();

  // ---------------- Baca PWM Pixhawk ----------------
  int rawPan  = pulseIn(PAN_PWM_PIN, HIGH, 30000);
  int rawTilt = pulseIn(TILT_PWM_PIN, HIGH, 30000);

  unsigned long now = millis();

  if (rawPan >= PWM_MIN_VALID_US && rawPan <= PWM_MAX_VALID_US) {
    panPWM = rawPan;
    lastPanPWMValid_ms = now;
  }
  if (rawTilt >= PWM_MIN_VALID_US && rawTilt <= PWM_MAX_VALID_US) {
    tiltPWM = rawTilt;
    lastTiltPWMValid_ms = now;
  }

  panLost  = (now - lastPanPWMValid_ms)  > PWM_TIMEOUT_MS;
  tiltLost = (now - lastTiltPWMValid_ms) > PWM_TIMEOUT_MS;
  failsafeActive = panLost || tiltLost;

  // Konversi PWM -> target sudut (hanya jika sinyal masih valid)
  if (!panLost) {
    panTargetAngle = pwmToAngle(panPWM, PWM_ANGLE_MIN_US, PWM_ANGLE_MAX_US,
                                 PAN_ANGLE_MIN, PAN_ANGLE_MAX);
  }
  if (!tiltLost) {
    tiltTargetAngle = pwmToAngle(tiltPWM, PWM_ANGLE_MIN_US, PWM_ANGLE_MAX_US,
                                  TILT_ANGLE_MIN, TILT_ANGLE_MAX);
  }

  // ---------------- PID + motor tiap PID_INTERVAL_MS ----------------
  if (now - lastPIDTrigger >= PID_INTERVAL_MS) {
    lastPIDTrigger = now;

    noInterrupts();
    long panEncNow  = panCount;
    long tiltEncNow = tiltCount;
    interrupts();

    float panAngleNow  = encoderToAngle(panEncNow,  panZeroOffset,  panCountsPerDeg);
    float tiltAngleNow = encoderToAngle(tiltEncNow, tiltZeroOffset, tiltCountsPerDeg);

    if (failsafeActive) {
      hardStop(panAxis);
      hardStop(tiltAxis);
    } else {
      updatePanAxis(panTargetAngle, panAngleNow);     // Bagian 2
      updateTiltAxis(tiltTargetAngle, tiltAngleNow);  // Bagian 3
    }
  }

  // ---------------- Telemetry debug tiap 200ms ----------------
  if (now - lastPrint > 200) {
    lastPrint = now;
    printTelemetry();
  }
}

// ---------------- Konversi PWM -> sudut ----------------
float pwmToAngle(int pwmUs, int pwmMin, int pwmMax, float angleMin, float angleMax) {
  pwmUs = constrain(pwmUs, pwmMin, pwmMax);
  float ratio = (float)(pwmUs - pwmMin) / (float)(pwmMax - pwmMin);
  return angleMin + ratio * (angleMax - angleMin);
}

// ---------------- Konversi encoder -> sudut signed ----------------
float encoderToAngle(long counts, long zeroOffset, float countsPerDeg) {
  if (countsPerDeg == 0) countsPerDeg = 1; // proteksi div/0 kalau belum dikalibrasi
  return (float)(counts - zeroOffset) / countsPerDeg;
  // Catatan: hasil sudah otomatis signed (bisa negatif) karena counts-zeroOffset
  // bisa negatif. Tidak perlu wrap manual karena PAN dibatasi hardstop fisik
  // di -180/+180 (lihat PAN_CONTINUOUS_ROTATION di atas).
}

// ---------------- Fungsi motor dasar ----------------
void writeMotor(AxisControl &axis, int signedPWM) {
  signedPWM = constrain(signedPWM, -MOTOR_MAX_PWM, MOTOR_MAX_PWM);
  if (signedPWM > 0) {
    analogWrite(axis.rpwmPin, signedPWM);
    analogWrite(axis.lpwmPin, 0);
  } else if (signedPWM < 0) {
    analogWrite(axis.rpwmPin, 0);
    analogWrite(axis.lpwmPin, -signedPWM);
  } else {
    analogWrite(axis.rpwmPin, 0);
    analogWrite(axis.lpwmPin, 0);
  }
}

void stopMotor(AxisControl &axis) {
  analogWrite(axis.rpwmPin, 0);
  analogWrite(axis.lpwmPin, 0);
  axis.currentOutput = 0;
}

// Failsafe: matikan motor seketika, batalkan status braking, reset integral
void hardStop(AxisControl &axis) {
  axis.braking = false;
  axis.integral = 0;
  axis.inDeadband = false;
  stopMotor(axis);
}


// ============================================================
// ================   BAGIAN 2   ================================
// PID PAN penuh: signed angle, shortest-path AMAN terhadap
// hardstop, anti-windup conditional-integration, deadband
// halus + hysteresis, motor smoothing, conditional braking
// ============================================================

// Wrap generik shortest-path (HANYA dipakai kalau PAN_CONTINUOUS_ROTATION true,
// yaitu jika suatu saat hardstop PAN dilepas dan PAN bisa berputar 360 penuh)
float wrapAngleDiff(float target, float current) {
  float diff = target - current;
  while (diff > 180.0)  diff -= 360.0;
  while (diff < -180.0) diff += 360.0;
  return diff;
}

void updatePanAxis(float targetAngle, float currentAngle) {
  unsigned long now = millis();
  float dt = (now - panAxis.lastUpdateTime) / 1000.0;
  if (dt <= 0) dt = PID_INTERVAL_MS / 1000.0; // proteksi dt=0 di siklus pertama
  panAxis.lastUpdateTime = now;

  // ---- Hitung error dengan shortest-path yang AMAN terhadap hardstop ----
  float error;
  if (PAN_CONTINUOUS_ROTATION) {
    // PAN bebas berputar penuh -> boleh ambil jalur terpendek lewat wrap ±180
    error = wrapAngleDiff(targetAngle, currentAngle);
  } else {
    // PAN punya hardstop fisik -> TIDAK boleh wrap lewat ±180 (akan menabrak).
    // Karena rentang target & posisi sama-sama dibatasi -180..+180,
    // error langsung ini SUDAH merupakan jalur terpendek yang legal.
    error = targetAngle - currentAngle;
  }

  // ---- Deadband halus dengan hysteresis (anti-chattering) ----
  if (!panAxis.inDeadband) {
    if (fabs(error) < PAN_DEADBAND_DEG) panAxis.inDeadband = true;
  } else {
    if (fabs(error) > (PAN_DEADBAND_DEG + PAN_DEADBAND_HYST)) panAxis.inDeadband = false;
  }

  int desiredOutput = 0;
  float pTerm = 0, iTerm = 0, dTerm = 0;

  if (panAxis.inDeadband) {
    panAxis.integral = 0;
    panAxis.lastError = 0;
  } else {
    // ---- PID ----
    pTerm = Kp_pan * error;

    float derivative = (error - panAxis.lastError) / dt;
    dTerm = Kd_pan * derivative;
    panAxis.lastError = error;

    float pidOutRaw = pTerm + (Ki_pan * panAxis.integral) + dTerm;

    // ---- Anti-windup: conditional integration ----
    // Hanya integrasikan error kalau output belum saturasi ke arah yang sama,
    // supaya integral tidak terus menumpuk (windup) saat motor sudah mentok max.
    bool saturated  = fabs(pidOutRaw) >= MOTOR_MAX_PWM;
    bool sameSign   = (pidOutRaw > 0 && error > 0) || (pidOutRaw < 0 && error < 0);
    if (!(saturated && sameSign)) {
      panAxis.integral += error * dt;
      panAxis.integral = constrain(panAxis.integral, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);
    }
    iTerm = Ki_pan * panAxis.integral;

    float pidOut = pTerm + iTerm + dTerm;
    desiredOutput = (int)constrain(pidOut, -MOTOR_MAX_PWM, MOTOR_MAX_PWM);

    // Ambang minimum agar motor GM25-370 benar-benar bergerak (gesekan statis)
    if (desiredOutput != 0 && abs(desiredOutput) < MOTOR_MIN_PWM) {
      desiredOutput = (desiredOutput > 0) ? MOTOR_MIN_PWM : -MOTOR_MIN_PWM;
    }
  }

  // ---- Kecepatan sudut aktual untuk logika braking ----
  float speedDegPerSec = (currentAngle - panAxis.prevAngle) / dt;
  panAxis.prevAngle = currentAngle;

  // ---- Conditional braking ----
  if (desiredOutput == 0 && !panAxis.braking && fabs(speedDegPerSec) > BRAKE_SPEED_THRESHOLD_DEGPS) {
    panAxis.braking = true;
    panAxis.brakeDir = (panAxis.currentOutput > 0) ? -1 : 1;
    panAxis.brakeEndTime = millis() + BRAKE_DURATION_MS;
  }
  if (panAxis.braking) {
    if (millis() < panAxis.brakeEndTime) {
      desiredOutput = panAxis.brakeDir * BRAKE_PWM;
    } else {
      panAxis.braking = false;
      desiredOutput = 0;
    }
  }

  // ---- Motor smoothing (slew-rate ramping) ----
  if (panAxis.currentOutput < desiredOutput) {
    panAxis.currentOutput = min(panAxis.currentOutput + SLEW_RATE_STEP, desiredOutput);
  } else if (panAxis.currentOutput > desiredOutput) {
    panAxis.currentOutput = max(panAxis.currentOutput - SLEW_RATE_STEP, desiredOutput);
  }

  writeMotor(panAxis, panAxis.currentOutput);

  // ---- Simpan data untuk debug telemetry ----
  panAxis.lastPTerm = pTerm;
  panAxis.lastITerm = iTerm;
  panAxis.lastDTerm = dTerm;
  panAxis.lastTargetAngle = targetAngle;
  panAxis.lastActualAngle = currentAngle;
  panAxis.lastErrorDeg = error;
  panAxis.lastDt = dt;
}


// ============================================================
// ================   BAGIAN 3   ================================
// PID TILT penuh, ISR encoder, kalibrasi EEPROM, Serial
// command, telemetry debug lengkap
// ============================================================

void updateTiltAxis(float targetAngle, float currentAngle) {
  unsigned long now = millis();
  float dt = (now - tiltAxis.lastUpdateTime) / 1000.0;
  if (dt <= 0) dt = PID_INTERVAL_MS / 1000.0;
  tiltAxis.lastUpdateTime = now;

  // TILT tidak butuh shortest-path (rentang 0..90, tidak signed, tidak wrap)
  float error = targetAngle - currentAngle;

  // ---- Deadband halus dengan hysteresis ----
  if (!tiltAxis.inDeadband) {
    if (fabs(error) < TILT_DEADBAND_DEG) tiltAxis.inDeadband = true;
  } else {
    if (fabs(error) > (TILT_DEADBAND_DEG + TILT_DEADBAND_HYST)) tiltAxis.inDeadband = false;
  }

  int desiredOutput = 0;
  float pTerm = 0, iTerm = 0, dTerm = 0;

  if (tiltAxis.inDeadband) {
    tiltAxis.integral = 0;
    tiltAxis.lastError = 0;
  } else {
    pTerm = Kp_tilt * error;

    float derivative = (error - tiltAxis.lastError) / dt;
    dTerm = Kd_tilt * derivative;
    tiltAxis.lastError = error;

    float pidOutRaw = pTerm + (Ki_tilt * tiltAxis.integral) + dTerm;

    bool saturated = fabs(pidOutRaw) >= MOTOR_MAX_PWM;
    bool sameSign  = (pidOutRaw > 0 && error > 0) || (pidOutRaw < 0 && error < 0);
    if (!(saturated && sameSign)) {
      tiltAxis.integral += error * dt;
      tiltAxis.integral = constrain(tiltAxis.integral, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);
    }
    iTerm = Ki_tilt * tiltAxis.integral;

    float pidOut = pTerm + iTerm + dTerm;
    desiredOutput = (int)constrain(pidOut, -MOTOR_MAX_PWM, MOTOR_MAX_PWM);

    if (desiredOutput != 0 && abs(desiredOutput) < MOTOR_MIN_PWM) {
      desiredOutput = (desiredOutput > 0) ? MOTOR_MIN_PWM : -MOTOR_MIN_PWM;
    }
  }

  float speedDegPerSec = (currentAngle - tiltAxis.prevAngle) / dt;
  tiltAxis.prevAngle = currentAngle;

  if (desiredOutput == 0 && !tiltAxis.braking && fabs(speedDegPerSec) > BRAKE_SPEED_THRESHOLD_DEGPS) {
    tiltAxis.braking = true;
    tiltAxis.brakeDir = (tiltAxis.currentOutput > 0) ? -1 : 1;
    tiltAxis.brakeEndTime = millis() + BRAKE_DURATION_MS;
  }
  if (tiltAxis.braking) {
    if (millis() < tiltAxis.brakeEndTime) {
      desiredOutput = tiltAxis.brakeDir * BRAKE_PWM;
    } else {
      tiltAxis.braking = false;
      desiredOutput = 0;
    }
  }

  if (tiltAxis.currentOutput < desiredOutput) {
    tiltAxis.currentOutput = min(tiltAxis.currentOutput + SLEW_RATE_STEP, desiredOutput);
  } else if (tiltAxis.currentOutput > desiredOutput) {
    tiltAxis.currentOutput = max(tiltAxis.currentOutput - SLEW_RATE_STEP, desiredOutput);
  }

  writeMotor(tiltAxis, tiltAxis.currentOutput);

  tiltAxis.lastPTerm = pTerm;
  tiltAxis.lastITerm = iTerm;
  tiltAxis.lastDTerm = dTerm;
  tiltAxis.lastTargetAngle = targetAngle;
  tiltAxis.lastActualAngle = currentAngle;
  tiltAxis.lastErrorDeg = error;
  tiltAxis.lastDt = dt;
}

// ---------------- ISR Encoder (quadrature, tabel arah) ----------------
void panISR() {
  byte A = digitalRead(PAN_ENC_A);
  byte B = digitalRead(PAN_ENC_B);
  byte current = (A << 1) | B;
  static const int8_t table[16] = {
    0, -1, 1, 0,
    1, 0, 0, -1,
    -1, 0, 0, 1,
    0, 1, -1, 0
  };
  byte index = (panState << 2) | current;
  panCount += table[index];
  panState = current;
}

void tiltISR() {
  byte A = digitalRead(TILT_ENC_A);
  byte B = digitalRead(TILT_ENC_B);
  byte current = (A << 1) | B;
  static const int8_t table[16] = {
    0, -1, 1, 0,
    1, 0, 0, -1,
    -1, 0, 0, 1,
    0, 1, -1, 0
  };
  byte index = (tiltState << 2) | current;
  tiltCount += table[index];
  tiltState = current;
}

// ---------------- Kalibrasi & EEPROM ----------------
void loadCalibrationFromEEPROM() {
  byte magic = EEPROM.read(EEPROM_MAGIC_ADDR);
  if (magic == EEPROM_MAGIC_VALUE) {
    EEPROM.get(EEPROM_ADDR_PAN_CPD, panCountsPerDeg);
    EEPROM.get(EEPROM_ADDR_TILT_CPD, tiltCountsPerDeg);
    EEPROM.get(EEPROM_ADDR_PAN_ZERO, panZeroOffset);
    EEPROM.get(EEPROM_ADDR_TILT_ZERO, tiltZeroOffset);
    EEPROM.get(EEPROM_ADDR_KP_PAN, Kp_pan);
    EEPROM.get(EEPROM_ADDR_KI_PAN, Ki_pan);
    EEPROM.get(EEPROM_ADDR_KD_PAN, Kd_pan);
    EEPROM.get(EEPROM_ADDR_KP_TILT, Kp_tilt);
    EEPROM.get(EEPROM_ADDR_KI_TILT, Ki_tilt);
    EEPROM.get(EEPROM_ADDR_KD_TILT, Kd_tilt);
    Serial.println("[EEPROM] Kalibrasi & PID dimuat dari memori.");
  } else {
    Serial.println("[EEPROM] Belum ada data tersimpan (atau format lama), memakai nilai default.");
    saveCalibrationToEEPROM();
  }
}

void saveCalibrationToEEPROM() {
  EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
  EEPROM.put(EEPROM_ADDR_PAN_CPD, panCountsPerDeg);
  EEPROM.put(EEPROM_ADDR_TILT_CPD, tiltCountsPerDeg);
  EEPROM.put(EEPROM_ADDR_PAN_ZERO, panZeroOffset);
  EEPROM.put(EEPROM_ADDR_TILT_ZERO, tiltZeroOffset);
  EEPROM.put(EEPROM_ADDR_KP_PAN, Kp_pan);
  EEPROM.put(EEPROM_ADDR_KI_PAN, Ki_pan);
  EEPROM.put(EEPROM_ADDR_KD_PAN, Kd_pan);
  EEPROM.put(EEPROM_ADDR_KP_TILT, Kp_tilt);
  EEPROM.put(EEPROM_ADDR_KI_TILT, Ki_tilt);
  EEPROM.put(EEPROM_ADDR_KD_TILT, Kd_tilt);
  Serial.println("[EEPROM] Data disimpan.");
}

void printCalibration() {
  Serial.println("---- KALIBRASI SAAT INI ----");
  Serial.print("PAN  countsPerDeg : "); Serial.println(panCountsPerDeg, 4);
  Serial.print("TILT countsPerDeg : "); Serial.println(tiltCountsPerDeg, 4);
  Serial.print("PAN  zeroOffset   : "); Serial.println(panZeroOffset);
  Serial.print("TILT zeroOffset   : "); Serial.println(tiltZeroOffset);
  Serial.print("PID PAN  Kp/Ki/Kd : ");
  Serial.print(Kp_pan); Serial.print(" / "); Serial.print(Ki_pan); Serial.print(" / "); Serial.println(Kd_pan);
  Serial.print("PID TILT Kp/Ki/Kd : ");
  Serial.print(Kp_tilt); Serial.print(" / "); Serial.print(Ki_tilt); Serial.print(" / "); Serial.println(Kd_tilt);
  Serial.print("PAN continuous rotation : "); Serial.println(PAN_CONTINUOUS_ROTATION ? "YA (wrap aktif)" : "TIDAK (hardstop, no-wrap)");
  Serial.println("----------------------------");
}

void printSerialHelp() {
  Serial.println("---- PERINTAH SERIAL (kalibrasi) ----");
  Serial.println("Z            -> set posisi sekarang sebagai titik 0 derajat (PAN & TILT)");
  Serial.println("P<sudut>     -> contoh: P90.0  (setelah PAN diputar manual ke sudut itu)");
  Serial.println("T<sudut>     -> contoh: T45.0  (setelah TILT diputar manual ke sudut itu)");
  Serial.println("KP=p,i,d     -> contoh: KP=2.0,0.05,0.3   set PID PAN");
  Serial.println("KT=p,i,d     -> contoh: KT=2.0,0.05,0.3   set PID TILT");
  Serial.println("D            -> tampilkan kalibrasi & PID saat ini");
  Serial.println("R            -> reset EEPROM ke default");
  Serial.println("--------------------------------------");
}

void handleSerialCommands() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  noInterrupts();
  long panEncNow  = panCount;
  long tiltEncNow = tiltCount;
  interrupts();

  char cmd = line.charAt(0);

  if (cmd == 'Z' || cmd == 'z') {
    panZeroOffset  = panEncNow;
    tiltZeroOffset = tiltEncNow;
    saveCalibrationToEEPROM();
    Serial.println("[CAL] Titik nol PAN & TILT diset ke posisi sekarang.");
  }
  else if (cmd == 'P' || cmd == 'p') {
    float knownAngle = line.substring(1).toFloat();
    long delta = panEncNow - panZeroOffset;
    if (knownAngle != 0 && delta != 0) {
      panCountsPerDeg = (float)delta / knownAngle;
      saveCalibrationToEEPROM();
      Serial.print("[CAL] PAN countsPerDeg = "); Serial.println(panCountsPerDeg, 4);
    } else {
      Serial.println("[CAL] Gagal: sudut 0 atau encoder belum berubah dari titik nol.");
    }
  }
  else if (cmd == 'T' || cmd == 't') {
    float knownAngle = line.substring(1).toFloat();
    long delta = tiltEncNow - tiltZeroOffset;
    if (knownAngle != 0 && delta != 0) {
      tiltCountsPerDeg = (float)delta / knownAngle;
      saveCalibrationToEEPROM();
      Serial.print("[CAL] TILT countsPerDeg = "); Serial.println(tiltCountsPerDeg, 4);
    } else {
      Serial.println("[CAL] Gagal: sudut 0 atau encoder belum berubah dari titik nol.");
    }
  }
  else if (line.startsWith("KP=") || line.startsWith("kp=")) {
    parsePID(line.substring(3), Kp_pan, Ki_pan, Kd_pan);
    saveCalibrationToEEPROM();
    Serial.println("[CAL] PID PAN diperbarui.");
  }
  else if (line.startsWith("KT=") || line.startsWith("kt=")) {
    parsePID(line.substring(3), Kp_tilt, Ki_tilt, Kd_tilt);
    saveCalibrationToEEPROM();
    Serial.println("[CAL] PID TILT diperbarui.");
  }
  else if (cmd == 'D' || cmd == 'd') {
    printCalibration();
  }
  else if (cmd == 'R' || cmd == 'r') {
    EEPROM.write(EEPROM_MAGIC_ADDR, 0xFF);
    Serial.println("[CAL] EEPROM direset. Restart board untuk memakai nilai default.");
  }
  else {
    Serial.println("[CAL] Perintah tidak dikenal. Ketik 'D' untuk lihat status atau lihat daftar perintah di atas.");
  }
}

void parsePID(String params, float &kp, float &ki, float &kd) {
  int firstComma = params.indexOf(',');
  int secondComma = params.indexOf(',', firstComma + 1);
  if (firstComma == -1 || secondComma == -1) {
    Serial.println("[CAL] Format salah. Contoh: KP=2.0,0.05,0.3");
    return;
  }
  kp = params.substring(0, firstComma).toFloat();
  ki = params.substring(firstComma + 1, secondComma).toFloat();
  kd = params.substring(secondComma + 1).toFloat();
}

// ---------------- Telemetry debug lengkap ----------------
void printTelemetry() {
  Serial.println(F("--------------------------------------------------"));
  Serial.print(failsafeActive ? F("[FAILSAFE] ") : F("[OK] "));
  Serial.print(F("t=")); Serial.println(millis());

  // ---- PAN ----
  noInterrupts();
  long panEncNowDbg = panCount;
  interrupts();

  const char* panDirLabel;
  if (panAxis.currentOutput > 0)      panDirLabel = "KANAN";
  else if (panAxis.currentOutput < 0) panDirLabel = "KIRI";
  else                                 panDirLabel = "DIAM";

  Serial.print(F("PAN  | PWM:")); Serial.print(panPWM);
  Serial.print(F(" lost:")); Serial.print(panLost ? "Y" : "N");
  Serial.print(F(" | Target(deg):")); Serial.print(panAxis.lastTargetAngle, 2);
  Serial.print(F(" Current(deg):")); Serial.print(panAxis.lastActualAngle, 2);
  Serial.print(F(" EncCount:")); Serial.print(panEncNowDbg);
  Serial.print(F(" Error(deg):")); Serial.print(panAxis.lastErrorDeg, 2);
  Serial.print(F(" dt:")); Serial.print(panAxis.lastDt, 4);
  Serial.print(F(" | P:")); Serial.print(panAxis.lastPTerm, 1);
  Serial.print(F(" I:")); Serial.print(panAxis.lastITerm, 1);
  Serial.print(F(" D:")); Serial.print(panAxis.lastDTerm, 1);
  Serial.print(F(" | OutputPWM:")); Serial.print(panAxis.currentOutput);
  Serial.print(F(" Direction:")); Serial.print(panDirLabel);
  Serial.print(F(" deadband:")); Serial.print(panAxis.inDeadband ? "Y" : "N");
  Serial.print(F(" brake:")); Serial.println(panAxis.braking ? "Y" : "N");

  // ---- TILT ----
  Serial.print(F("TILT | PWMraw:")); Serial.print(tiltPWM);
  Serial.print(F(" lost:")); Serial.print(tiltLost ? "Y" : "N");
  Serial.print(F(" | tgt:")); Serial.print(tiltAxis.lastTargetAngle, 2);
  Serial.print(F(" act:")); Serial.print(tiltAxis.lastActualAngle, 2);
  Serial.print(F(" err:")); Serial.print(tiltAxis.lastErrorDeg, 2);
  Serial.print(F(" dt:")); Serial.print(tiltAxis.lastDt, 4);
  Serial.print(F(" | P:")); Serial.print(tiltAxis.lastPTerm, 1);
  Serial.print(F(" I:")); Serial.print(tiltAxis.lastITerm, 1);
  Serial.print(F(" D:")); Serial.print(tiltAxis.lastDTerm, 1);
  Serial.print(F(" | out:")); Serial.print(tiltAxis.currentOutput);
  Serial.print(F(" deadband:")); Serial.print(tiltAxis.inDeadband ? "Y" : "N");
  Serial.print(F(" brake:")); Serial.println(tiltAxis.braking ? "Y" : "N");
}
