
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  // Intenta conectarse a los auriculares Bluetooth
  if(SerialBT.connect("00:16:94:2f:84:ea")) {
    Serial.println("Connected Successfully!");
  } else {
    Serial.println("Failed to Connect");
  }
}

void loop() {
  // Aquí puedes añadir código para manejar la conexión
  delay(100);
}
