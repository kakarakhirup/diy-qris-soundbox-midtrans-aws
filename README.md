# DIY QRIS SoundBox (ESP32 + Midtrans + AWS IoT Core)
Proyek SoundBox QRIS menggunakan ESP32. Alat ini akan mengeluarkan output suara nominal setiap kali ada pembayaran QRIS yang sukses via Midtrans.

## Fitur
- Pembacaan nominal Rupiah hingga jutaan.
- Koneksi menggunakan AWS IoT Core.
- Tampilan LCD 16x2 untuk indikator status dan nominal.
- Kompatibel dengan Webhook Midtrans.

## Komponen Perangkat Keras
- **ESP32 Development Board** (ESP32-CH-340)
- **DFPlayer Mini MP3 Player** + MicroSD Card
- **Speaker 8 Ohm / 3 Watt**
- **LCD 16x2 dengan Modul I2C Backpack**

## Schematic
<img width="1373" height="895" alt="schematic" src="https://github.com/user-attachments/assets/d44bc4ce-185f-4138-9c31-7d152da81b95" />

- LCD 16x2 VCC -> Pin 5V ESP32
- LCD 16x2 SDA -> GPIO21
- LCD 16x2 SCL -> GPIO22
- DFPlayer VCC -> Pin 5V ESP32
- DFPlayer RX -> GPIO17
- DFPlayer TX -> GPIO16

## Struktur File MicroSD (DFPlayer)
Di MicroSD, pastikan isi file `MP3` langsung tanpa ada folder dan file audio diisi dengan urutan berikut:
- `0001.mp3` : "satu"
- `0002.mp3` : "dua"
- `0003.mp3` : "tiga"
- `0004.mp3` : "empat"
- `0005.mp3` : "lima"
- `0006.mp3` : "enam"
- `0007.mp3` : "tujuh"
- `0008.mp3` : "delapan"
- `0009.mp3` : "sembilan"
- `0010.mp3` : "sepuluh"
- `0011.mp3` : "sebelas"
- `0012.mp3` : "belas"
- `0013.mp3` : "puluh"
- `0014.mp3` : "seratus"
- `0015.mp3` : "ratus"
- `0016.mp3` : "seribu"
- `0017.mp3` : "ribu"
- `0018.mp3` : "juta"
- `0019.mp3` : "rupiah"
- `0020.mp3` : "pembayaran diterima" atau "sfx: cha-ching"

[![Download Audio](https://img.shields.io/badge/Download-Audio_Voicebank-blue?style=for-the-badge&logo=github)](https://github.com/kakarakhirup/diy-qris-soundbox-midtrans-aws/raw/main/voicebank.rar)

*audio wajib diawali dengan `0001` (4 digit) dengan format `.mp3`*

## Library Arduino yang Dibutuhkan
- `PubSubClient` by Nick O'Leary
- `ArduinoJson` by Benoit Blanchon
- `hd44780` by Bill Perry
- `DFRobotDFPlayerMini` by DFRobot
