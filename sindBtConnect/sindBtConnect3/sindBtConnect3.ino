#include "AudioTools.h"
#include "AudioLibs/AudioA2DP.h"
#include <driver/i2s.h>

#define I2S_WS               5
#define I2S_SD               18
#define I2S_SCK              17
#define I2S_SAMPLE_BIT_COUNT 16
#define SOUND_SAMPLE_RATE    16000
#define SOUND_CHANNEL_COUNT  1
#define I2S_PORT             I2S_NUM_0

using namespace audio_tools;
BluetoothA2DPSource a2dp_source;


const int I2S_DMA_BUF_COUNT = 8;
const int I2S_DMA_BUF_LEN = 1024;


#if I2S_SAMPLE_BIT_COUNT == 32
  const int StreamBufferNumBytes = 512;
  const int StreamBufferLen = StreamBufferNumBytes / 4;
  int32_t StreamBuffer[StreamBufferLen];
#else
  #if SOUND_SAMPLE_RATE == 16000
    // for 16 bits ... 16000 sample per second (32000 bytes per second; since 16 bits per sample) ==> 512 bytes = 16 ms per read
    const int StreamBufferNumBytes = 512;
  #else
    // for 16 bits ... 8000 sample per second (16000 bytes per second; since 16 bits per sample) ==> 256 bytes = 16 ms per read
    const int StreamBufferNumBytes = 256;
  #endif  
  const int StreamBufferLen = StreamBufferNumBytes / 2;
  int16_t StreamBuffer[StreamBufferLen];
#endif



// Función adaptada para obtener datos de sonido
int32_t get_sound_data(Frame* data, int32_t len) {
    size_t bytesRead = 0;
    esp_err_t result = i2s_read(I2S_PORT, &StreamBuffer, StreamBufferNumBytes, &bytesRead, portMAX_DELAY);

    if (result == ESP_OK) {
        // Copia los datos leídos a la estructura Frame
        memcpy(data->left, StreamBuffer, bytesRead);
        // Devuelve el número de muestras leídas
        return bytesRead / (I2S_SAMPLE_BIT_COUNT / 8);
    }

    return 0; // En caso de error, devuelve 0
}

void setup() {
    Serial.begin(115200);

    // Inicializar I2S
    i2s_install();
    i2s_setpin();

    // Iniciar Bluetooth A2DP
    a2dp_source.start("MyESP32Audio", get_sound_data);
}

void loop() {
    // Aquí puedes agregar cualquier lógica adicional si es necesario
    delay(1000);
}
