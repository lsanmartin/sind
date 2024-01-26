#include <Arduino.h>
#include <BluetoothA2DPSource.h>



BluetoothA2DPSource a2dp_source;
// SoundData *data = new OneChannelSoundData((int16_t*)piano16bit_raw, piano16bit_raw_len / 2);

void setup() {
  // Inicialización de Bluetooth
  a2dp_source.start();

  // Inicia la búsqueda de dispositivos
  //a2dp_source.startDiscovery();

  // Espera a que los dispositivos sean descubiertos
  //while (!a2dp_source.isDeviceAvailable()) {
   // delay(100);
  //}

  const char* deviceAddress = "00:16:94:2F:84:EA";
  
  // Convierte la dirección MAC en un array de bytes
  uint8_t remoteAddress[6];
  sscanf(deviceAddress, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
         &remoteAddress[0], &remoteAddress[1], &remoteAddress[2], 
         &remoteAddress[3], &remoteAddress[4], &remoteAddress[5]);
  
  // Conecta al dispositivo seleccionado
  a2dp_source.connect(remoteAddress);
}

void loop() {
  // Manejar eventos de conexión
  if (a2dp_source.isConnected()) {
    // Conexión exitosa, proceder con la reproducción de audio
    // Aquí deberías agregar el código para reproducir el audio
  } else {
    // Manejar fallo de conexión
    // Agregar código para reintentar la conexión o manejar el error
  }
}
