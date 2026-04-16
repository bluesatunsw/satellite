// IMU Data Logger — NUCLEO STM32F042K6
//
// Wiring:
//   SD card: CS=D10, MOSI=D11, MISO=D12, SCK=D13
//   IMU:     SDA=A4, SCL=A5
//
// Libraries needed (install via Arduino Library Manager):
//   - "SD"

#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN   10
#define IMU_ADDRESS 0x68

File dataFile;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting...");

  // Start I2C
  Wire.begin();

  // Wake up the IMU (it starts in sleep mode apparently)
  Wire.beginTransmission(IMU_ADDRESS);
  Wire.write(0x6B); // power management register
  Wire.write(0x00); // write 0 to wake it up
  Wire.endTransmission();
  delay(100);
  Serial.println("IMU OK");

  // Start SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card failed. Wiring wrong.");
    while (true);
  }
  Serial.println("SD OK");

  // Open file
  dataFile = SD.open("data.csv", FILE_WRITE);
  if (!dataFile) {
    Serial.println("data.csv could not be opened");
    while (true);
  }

  dataFile.println("time_ms,ax,ay,az,gx,gy,gz");
  dataFile.flush();
  Serial.println("Starting data.csv logging");
}

void loop() {
  // Ask the IMU to send data starting from register 0x3B
  Wire.beginTransmission(IMU_ADDRESS);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(IMU_ADDRESS, 14); // 14 bytes = accel(6) + temp(2) + gyro(6)

  int16_t ax = Wire.read() << 8 | Wire.read();
  int16_t ay = Wire.read() << 8 | Wire.read();
  int16_t az = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read(); // skip temperature
  int16_t gx = Wire.read() << 8 | Wire.read();
  int16_t gy = Wire.read() << 8 | Wire.read();
  int16_t gz = Wire.read() << 8 | Wire.read();

  dataFile.print(millis());   dataFile.print(",");
  dataFile.print(ax);         dataFile.print(",");
  dataFile.print(ay);         dataFile.print(",");
  dataFile.print(az);         dataFile.print(",");
  dataFile.print(gx);         dataFile.print(",");
  dataFile.print(gy);         dataFile.print(",");
  dataFile.println(gz);
  dataFile.flush();

  Serial.print(millis());     Serial.print("  ax=");
  Serial.print(ax);           Serial.print("  ay=");
  Serial.print(ay);           Serial.print("  az=");
  Serial.println(az);
}
