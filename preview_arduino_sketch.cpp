#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include "secrets.h"


hd44780_I2Cexp lcd;
HardwareSerial mp3Serial(2);
DFRobotDFPlayerMini player;
WiFiClientSecure net;
PubSubClient mqtt(net);

const char* MQTT_TOPIC = "qris/payment";

bool paymentActive = false;
unsigned long paymentDisplayUntil = 0;

void playTrack(int track)
{
  player.play(track); 
  delay(300); 
  delay(900);
}

int buildQueue(long value, int queue[], int maxLen)
{
  int count = 0;
  if (value <= 0 || count >= maxLen) return 0;
  long jutaPart = value / 1000000;
  if (jutaPart > 0)
  {
    count += buildQueue(jutaPart, queue + count, maxLen - count);
    queue[count++] = 18;
    value %= 1000000;
  }

  long ribuPart = value / 1000;
  if (ribuPart > 0)
  {
    if (ribuPart == 1)
    {
      queue[count++] = 16;
    }
    else
    {
      count += buildQueue(ribuPart, queue + count, maxLen - count);
      queue[count++] = 17;
    }
    value %= 1000;
  }
 
  long ratusPart = value / 100;
  if (ratusPart > 0)
  {
    if (ratusPart == 1)
    {
      queue[count++] = 14;
    }
    else
    {
      queue[count++] = ratusPart;
      queue[count++] = 15;       
    }
    value %= 100;
  }
 
  if (value > 0)
  {
    if (value <= 9)
    {
      queue[count++] = value;
    }
    else if (value == 10)
    {
      queue[count++] = 10;
    }
    else if (value == 11)
    {
      queue[count++] = 11;
    }
    else if (value >= 12 && value <= 19)
    {
      queue[count++] = value - 10;
      queue[count++] = 12;         
    }
    else
    {
      long puluhan = value / 10;
      long satuan  = value % 10;
      queue[count++] = puluhan;
      queue[count++] = 13;     
      if (satuan > 0)
      {
        queue[count++] = satuan;
      }
    }
  }

  return count;
}

void playAmount(long amount)
{
  playTrack(20);
  const int MAX_TRACKS = 30;
  int queue[MAX_TRACKS];
  int total = buildQueue(amount, queue, MAX_TRACKS);
 
  if (total < MAX_TRACKS)
  {
    queue[total++] = 19;
  }

  for (int i = 0; i < total; i++)
  {
    playTrack(queue[i]);
  }
}

String formatRupiah(long value)
{
  String s = String(value);
  String result = "";
  int counter = 0;

  for (int i = s.length() - 1; i >= 0; i--)
  {
    result = s[i] + result;
    counter++;
    if (counter % 3 == 0 && i != 0)
    {
      result = "." + result;
    }
  }

  return "Rp " + result;
}

void showStandby()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menunggu");
  lcd.setCursor(0, 1);
  lcd.print("Pembayaran...");
}

void callback(char* topic, byte* payload, unsigned int length)
{
  String msg = "";
  for (unsigned int i = 0; i < length; i++)
  {
    msg += (char)payload[i];
  }

  Serial.println("==========");
  Serial.println("MQTT DATA");
  Serial.println(msg);

  StaticJsonDocument<128> doc;
  DeserializationError err = deserializeJson(doc, msg);

  if (err)
  {
    Serial.println("JSON ERROR");
    return;
  }

  long amount = doc["amount"];

  Serial.print("Amount = ");
  Serial.println(amount);
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Diterima");
  lcd.setCursor(0, 1);
  lcd.print(formatRupiah(amount));

  paymentActive = true;
  paymentDisplayUntil = millis() + 10000;
 
  playAmount(amount);
}

void connectWiFi()
{
  Serial.println("Connecting WiFi");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connect WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");

  delay(1000);
}

void connectAWS()
{
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  mqtt.setServer(AWS_IOT_ENDPOINT, 8883);
  mqtt.setCallback(callback);

  while (!mqtt.connected())
  {
    Serial.println("Connecting AWS...");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connect AWS");

    if (mqtt.connect("SoundBoxESP32"))
    {
      Serial.println("AWS Connected");
      mqtt.subscribe(MQTT_TOPIC);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("AWS Connected");

      delay(1000);
    }
    else
    {
      Serial.print("MQTT Error: ");
      Serial.println(mqtt.state());

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("AWS Failed");

      delay(3000);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  Wire.begin();
  int status = lcd.begin(16, 2);

  if (status)
  {
    Serial.print("LCD Error: ");
    Serial.println(status);
    while (true);
  }

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("QRIS SoundBox");
  lcd.setCursor(0, 1);
  lcd.print("Booting...");

  mp3Serial.begin(9600, SERIAL_8N1, 16, 17);
  delay(1000);

  if (player.begin(mp3Serial))
  {
    Serial.println("DFPlayer OK");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DFPlayer OK");
    player.volume(30);
    delay(1000);
  }
  else
  {
    Serial.println("DFPlayer FAIL");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DFPlayer FAIL");
    while (true);
  }

  connectWiFi();
  connectAWS();
  showStandby();
}

void loop()
{
  if (!mqtt.connected())
  {
    connectAWS();
  }

  mqtt.loop();

  if (paymentActive && millis() > paymentDisplayUntil)
  {
    paymentActive = false;
    showStandby();
  }
}
