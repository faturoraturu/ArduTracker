# ArduTracker

An open-source Arduino and Pixhawk-based Antenna Tracker featuring closed-loop PID control, encoder feedback, MAVLink communication, and ArduPlane SITL simulation for real and simulated UAV tracking.

---

## Features

- 🎯 Closed-loop PID Position Control
- 📡 MAVLink Telemetry Support
- 🚁 Real UAV Tracking
- 💻 ArduPlane SITL Simulation
- 🔄 MAVProxy Multi-Vehicle Routing
- 📍 Pixhawk ArduTracker Support
- ⚙️ Arduino + BTS7960 Motor Control
- 📈 Incremental Encoder Feedback
- 🛰️ Mission Planner Integration
- 🔧 EEPROM-based PID Calibration

---

# Operation Modes

Sistem Antenna Tracker ini dirancang agar dapat beroperasi dalam dua mode yang berbeda, yaitu menggunakan **UAV fisik (Real Flight)** maupun **Software In The Loop (SITL)**. Kedua mode tersebut menggunakan algoritma pelacakan dan firmware yang sama sehingga proses pengembangan dapat dilakukan secara aman menggunakan simulasi sebelum diimplementasikan pada sistem nyata.

---

# Mode 1 - UAV Nyata (SpeedyBee F405 + SIYI HM30 + Pixhawk ArduTracker)

Pada mode ini sistem bekerja menggunakan pesawat UAV sebenarnya. Seluruh data navigasi diperoleh secara langsung dari flight controller yang terpasang pada pesawat.

## Cara Kerja

Flight Controller (SpeedyBee F405) membaca data dari sensor GPS, IMU, Compass, Barometer, dan sensor penerbangan lainnya. Informasi tersebut diproses oleh ArduPlane kemudian dikirim dalam bentuk paket **MAVLink Telemetry**.

Telemetri kemudian dikirim melalui **SIYI HM30 Air Unit** menuju **SIYI HM30 Ground Unit** menggunakan komunikasi radio jarak jauh.

HM30 Ground Unit meneruskan data MAVLink menuju **Pixhawk 2.4.8** yang telah diprogram menggunakan firmware **ArduPilot Antenna Tracker**.

Firmware ArduTracker melakukan beberapa proses utama:

- membaca posisi Ground Station
- membaca posisi UAV
- menghitung Azimuth (PAN)
- menghitung Elevation (TILT)
- menghasilkan target sudut servo

Target sudut tersebut kemudian dikonversi menjadi sinyal PWM.

Selanjutnya PWM dikirim menuju Arduino sebagai kontrol posisi motor.

Arduino tidak langsung menggerakkan motor berdasarkan PWM tersebut, tetapi menggunakannya sebagai **target position**.

Arduino membaca posisi aktual motor menggunakan **Incremental Encoder**, kemudian menjalankan algoritma PID Closed Loop untuk mengurangi error hingga mendekati nol.

Output PID digunakan untuk mengendalikan driver motor BTS7960 sehingga motor PAN dan TILT bergerak mengikuti posisi pesawat secara presisi.

---

## Arsitektur Komunikasi

```text
                   GPS + IMU
                       │
                       ▼
              SpeedyBee F405 (ArduPlane)
                       │
                MAVLink Telemetry
                       │
                 SIYI HM30 Air
                       │
             Wireless Communication
                       │
                SIYI HM30 Ground
                       │
                MAVLink Serial
                       │
               Pixhawk ArduTracker
                       │
                 PWM Servo Output
                       │
                    Arduino
                       │
             PID + Encoder Feedback
                       │
                   BTS7960 Driver
                       │
                 Pan / Tilt Motor
                       │
                  Directional Antenna
```

---

## Kelebihan Mode UAV Nyata

- Menggunakan data GPS sebenarnya.
- Dapat digunakan pada penerbangan sesungguhnya.
- Menguji performa sistem pada kondisi nyata.
- Menguji kualitas komunikasi radio.
- Menguji akurasi tracking terhadap UAV yang sedang terbang.

---

# Mode 2 - Software In The Loop (ArduPlane SITL)

Selain menggunakan UAV fisik, sistem ini juga dapat diuji menggunakan **Software In The Loop (SITL)** yang disediakan oleh ArduPilot.

SITL merupakan simulator yang menjalankan firmware ArduPlane secara virtual di dalam komputer tanpa memerlukan flight controller maupun pesawat sebenarnya.

Pada mode ini seluruh sensor pesawat seperti:

- GPS
- IMU
- Compass
- Barometer
- Airspeed
- RC Input

disimulasikan secara otomatis oleh ArduPilot.

Dengan demikian ArduPlane menghasilkan paket MAVLink yang identik dengan pesawat asli.

Karena format data MAVLink yang dihasilkan sama persis, Pixhawk ArduTracker tidak dapat membedakan apakah telemetri berasal dari UAV nyata atau dari simulator.

Hal inilah yang memungkinkan seluruh sistem Antenna Tracker diuji tanpa harus menerbangkan pesawat.

---

## Cara Kerja

ArduPlane dijalankan menggunakan:

```
Tools/autotest/sim_vehicle.py -v ArduPlane
```

Perintah tersebut akan:

- menjalankan firmware ArduPlane
- membuat pesawat virtual
- menghasilkan GPS virtual
- menghasilkan data AHRS
- menghasilkan telemetri MAVLink

Telemetri kemudian diteruskan menuju **MAVProxy**.

MAVProxy berfungsi sebagai **MAVLink Router** yang dapat meneruskan data menuju lebih dari satu perangkat secara bersamaan.

Dalam proyek ini MAVProxy mengirim data ke dua tujuan sekaligus.

1. Mission Planner (UDP)
2. Pixhawk ArduTracker (USB Serial)

Dengan konfigurasi tersebut:

Mission Planner dapat menampilkan posisi pesawat virtual.

Sedangkan Pixhawk menerima data MAVLink yang sama seperti saat menggunakan UAV sebenarnya.

Pixhawk kemudian menghitung sudut PAN dan TILT lalu menghasilkan PWM menuju Arduino.

Arduino menjalankan algoritma PID sehingga motor bergerak mengikuti posisi pesawat virtual secara real-time.

---

## Arsitektur Komunikasi

```text
                  ArduPlane SITL
                       │
                 MAVLink UDP
                       │
                   MAVProxy
              ┌────────┴─────────┐
              │                  │
              ▼                  ▼
      Mission Planner      Pixhawk Tracker
        UDP :14550          USB /dev/ttyACM0
                                  │
                           PWM Servo Output
                                  │
                               Arduino
                                  │
                     PID + Encoder Feedback
                                  │
                             BTS7960 Driver
                                  │
                           Pan / Tilt Motor
```

---

## Fungsi MAVProxy

Pada sistem ini MAVProxy memiliki peran yang sangat penting.

MAVProxy bertindak sebagai **MAVLink Router** yang bertugas:

- menerima telemetri dari SITL
- mendistribusikan telemetri ke beberapa perangkat
- menjaga komunikasi MAVLink tetap sinkron
- memungkinkan Mission Planner dan ArduTracker menerima data yang sama secara bersamaan

Tanpa MAVProxy, data SITL hanya dapat dikirim menuju satu perangkat.

---

## Kelebihan Menggunakan SITL

- Tidak memerlukan UAV fisik.
- Tidak memerlukan GPS sebenarnya.
- Aman untuk proses pengembangan.
- Menghemat biaya pengujian.
- Dapat melakukan simulasi misi otomatis berkali-kali.
- Mempermudah debugging komunikasi MAVLink.
- Mempermudah tuning PID Arduino.
- Mempermudah pengujian firmware ArduTracker.
- Mengurangi risiko kerusakan pesawat saat proses pengembangan.

---

# Perbandingan Kedua Mode

| Fitur | UAV Nyata | ArduPlane SITL |
|---------|-----------|----------------|
| Menggunakan pesawat fisik | ✅ | ❌ |
| Menggunakan GPS asli | ✅ | ❌ (GPS Virtual) |
| Menggunakan Flight Controller fisik | ✅ | ❌ |
| Menghasilkan data MAVLink | ✅ | ✅ |
| Dapat digunakan Mission Planner | ✅ | ✅ |
| Dapat digunakan ArduTracker | ✅ | ✅ |
| Dapat menggerakkan motor PAN/TILT | ✅ | ✅ |
| Aman digunakan di dalam ruangan | ❌ | ✅ |
| Cocok untuk debugging | ⚠️ Terbatas | ✅ |
| Cocok untuk tuning PID | ⚠️ | ✅ |
| Risiko kerusakan pesawat | Tinggi | Sangat Rendah |

---

# Kesimpulan

Sistem Antenna Tracker ini dirancang agar memiliki fleksibilitas tinggi dengan mendukung **dua mode operasi**, yaitu **Mode UAV Nyata** dan **Mode Software In The Loop (SITL)**.

Pada mode UAV nyata, sistem menerima telemetri secara langsung dari Flight Controller melalui radio telemetry SIYI HM30 sehingga antena dapat mengikuti posisi pesawat sebenarnya.

Sedangkan pada mode SITL, seluruh data penerbangan dihasilkan secara virtual oleh ArduPilot sehingga proses pengembangan, pengujian komunikasi MAVLink, validasi algoritma ArduTracker, serta tuning sistem kontrol PID dapat dilakukan tanpa memerlukan pesawat fisik.

Karena kedua mode menggunakan protokol MAVLink yang sama, seluruh perangkat lunak pada Ground Station dapat digunakan tanpa perubahan konfigurasi yang signifikan. Hal ini memungkinkan proses pengembangan dilakukan menggunakan simulasi terlebih dahulu, kemudian langsung diimplementasikan pada UAV sebenarnya dengan tingkat risiko yang jauh lebih rendah.
