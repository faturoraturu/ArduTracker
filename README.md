ArduPilot Antenna Tracker

Dokumen ini menjelaskan arsitektur perangkat keras, alur komunikasi data, dan logika perangkat lunak dari sistem Antenna Tracker berbasis mikrokontroler Arduino dan Pixhawk. Sistem ini dirancang untuk melacak posisi wahana udara tanpa awak (UAV) secara otomatis dan presisi guna menjaga kualitas tautan komunikasi jarak jauh.

1. Alur Komunikasi Data (Telemetry & Control)
Sistem pelacakan ini bergantung pada aliran data koordinat secara real-time dari UAV ke stasiun bumi (ground station). Berikut adalah rute data dari pesawat hingga menjadi pergerakan fisik antena:

Pembangkitan Data (UAV): Flight controller pada pesawat (misalnya SpeedyBee F405) secara konstan membaca posisi GPS dan orientasi AHRS, lalu memaketkannya menjadi protokol MAVLink.

Transmisi Nirkabel (SIYI HM30): Data MAVLink dikirim oleh HM30 Air Unit melalui tautan radio nirkabel ke HM30 Ground Unit di darat.

Pemrosesan Targeting (Pixhawk): HM30 Ground Unit meneruskan data MAVLink ke Pixhawk 2.4.8 yang di- flash dengan firmware ArduTracker. ArduTracker mengkalkulasi posisi pesawat terhadap koordinat stasiun bumi dan menghasilkan perhitungan sudut PAN (kiri-kanan) dan TILT (naik-turun).

Output Sinyal (PWM): Pixhawk mengonversi target sudut tersebut menjadi sinyal PWM (Pulse Width Modulation) dengan rentang 1000 us hingga 2000 us, lalu mengirimkannya ke Arduino.

Eksekusi (Arduino & BTS7960): Arduino membaca lebar pulsa PWM tersebut secara terus-menerus. Dengan menggunakan algoritma PID, Arduino memerintahkan driver motor BTS7960 untuk memutar motor GM25-370-CE hingga antena tepat mengarah ke posisi target.

2. Batas Rentang Mekanik (Fisik Antena)
Desain antena ini menggunakan kabel yang terhubung langsung tanpa perantara slip-ring (konektor putar 360° tanpa batas). Untuk mencegah kabel terlilit atau putus, pergerakan mekanik dibatasi dengan hardstop fisik secara software:

Sumbu PAN (Kiri - Kanan): Dibatasi secara linear pada rentang -180° hingga +180°.

PWM 1500 us = 0° (Posisi antena menghadap lurus ke depan).

PWM 1000 us = -180° (Batas putaran maksimal ke kiri).

PWM 2000 us = +180° (Batas putaran maksimal ke kanan).

Sumbu TILT (Naik - Turun): Dibatasi pada rentang 0° hingga 90°.

PWM 1000 us = 0° (Antena mendatar).

PWM 2000 us = 90° (Antena menghadap tegak lurus ke langit).

Jika target UAV bergeser dari +181°, sistem tidak akan meneruskan putaran melingkar, melainkan akan memutar balik melewati titik 0° menuju sisi -180° (shortest-path yang legal terhadap batas fisik).

3. Logika Closed-Loop PID dan Umpan Balik Encoder
Berbeda dengan sistem open-loop konvensional, tracker ini menggunakan sistem Closed-Loop Position Control. Motor GM25-370-CE dilengkapi dengan incremental encoder yang mengirimkan ribuan pulsa elektronik per putaran ke Arduino.

Arduino secara real-time membaca pulsa ini untuk mengetahui "Posisi Aktual" (Current Angle) antena dalam derajat dan membandingkannya dengan "Posisi Target" dari Pixhawk. Selisih dari kedua nilai ini disebut sebagai Error.

Algoritma PID akan mengkalkulasi kecepatan motor berdasarkan error tersebut:

Proportional (P): Memberikan tenaga dorongan utama. Semakin besar jarak (error) antara antena dan pesawat, semakin kencang motor berputar.

Integral (I): Menyapu sisa error kecil secara bertahap. Jika P tidak cukup kuat mendorong gesekan fisik gearbox saat hampir sampai di target, I akan perlahan mengumpulkan sisa error tersebut dan menambah tenaga motor hingga target tercapai akurat.

Derivative (D): Bertindak sebagai rem prediktif. D membaca seberapa cepat antena bergerak mendekati target dan menahan laju motor untuk mencegah antena kebablasan (overshoot).

Deadband & Hysteresis: Untuk mencegah motor bergetar halus (chattering) saat antena sudah membidik target, motor akan dimatikan otomatis jika error berada di bawah 0.5°. Motor baru akan menyala kembali jika target bergeser cukup jauh keluar dari batas toleransi ini.

4. Algoritma Anti-Windup (Conditional Integration)
Salah satu keunggulan utama dari kode kontrol ini adalah penggunaan Conditional Integration.

Dalam sistem PID standar, jika antena tertinggal sangat jauh dari pergerakan pesawat yang cepat, nilai Integral akan terus terakumulasi dan membengkak (disebut Windup). Akibatnya, saat antena akhirnya menyusul posisi pesawat, nilai Integral yang kelewat besar ini akan memaksa motor terus berputar dan menghasilkan overshoot yang parah.

Conditional Integration memecahkan masalah ini dengan logika batasan kecepatan fisik:
Arduino mengetahui bahwa batas maksimal kecepatan driver BTS7960 adalah PWM 255. Jika sistem meminta kecepatan lebih dari 255, motor tidak mungkin berputar lebih cepat lagi. Pada titik saturasi ini, program akan berhenti menambahkan nilai Integral. Penambahan Integral baru akan dilanjutkan ketika kecepatan motor sudah turun di bawah batas maksimal, sehingga pergerakan tetap responsif dan presisi tanpa overshoot.

5. Kalibrasi Permanen (EEPROM)
Setiap perangkat mekanik memiliki toleransi perakitan yang berbeda. Oleh karena itu, rasio antara jumlah pulsa encoder dengan derajat fisik perputaran (Counts per Degree) harus disesuaikan.

Sistem ini memiliki modul penyimpanan konfigurasi ke memori internal (EEPROM) mikrokontroler.

Real-time Tuning: Kalibrasi titik 0° mekanik dan penyetelan nilai PID (Kp, Ki, Kd) dapat dilakukan langsung di lapangan menggunakan perintah Serial Monitor (seperti Z, P90.0, atau KP=2.0,0.05,0.3).

Non-Volatile: Begitu perintah dikirim, pengaturan akan langsung ditulis ke cip EEPROM. Saat tracker dimatikan dan dihidupkan kembali keesokan harinya, Arduino akan otomatis memuat ulang kalibrasi terakhir tanpa perlu di-flash ulang menggunakan komputer.
