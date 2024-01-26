// Comentar la línea que incluye esp32dumbdisplay
//#include "esp32dumbdisplay.h"

// Incluir biblioteca Bluetooth A2DP Sink
#include "BluetoothA2DPSink.h"


BluetoothA2DPSink a2dp_sink;



// I2S driver
#include <driver/i2s.h>

// INMP441 I2S pin assignment
#define I2S_WS               5
#define I2S_SD               18
#define I2S_SCK              17
// #define I2S_WS               15
// #define I2S_SD               14
// #define I2S_SCK              2
#define I2S_SAMPLE_BIT_COUNT 16
#define SOUND_SAMPLE_RATE    16000
#define SOUND_CHANNEL_COUNT  1


#define I2S_PORT  I2S_NUM_0



// PlotterDDLayer* plotterLayer;
// LcdDDLayer* micTabLayer;
// LcdDDLayer* recTabLayer;
// LcdDDLayer* playTabLayer;
// LcdDDLayer* startBtnLayer;
// LcdDDLayer* stopBtnLayer;
// LcdDDLayer* amplifyLblLayer;
// LedGridDDLayer* amplifyMeterLayer;



// name of recorded WAV file; since only a single name; hence new one will always overwrite old one
const char* SoundName = "recorded_sound";


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

// sound sample (16 bits) amplification
const int MaxAmplifyFactor = 20;
const int DefAmplifyFactor = 10;


void i2s_install();
void i2s_setpin();




void setup() {

  Serial.begin(115200);

  // Inicializar Bluetooth
  Serial.println("Configurando Bluetooth...");
  a2dp_sink.start("ESP32_A2DP_SINK");

  Serial.println("SETUP MIC ...");

  // set up I2S
  i2s_install();
  i2s_setpin();
  i2s_zero_dma_buffer(I2S_PORT);
  i2s_start(I2S_PORT);
  i2s_get_clk(I2S_PORT);
  Serial.println(I2S_PORT);

  Serial.println("... DONE SETUP MIC");



}


void loop() {

  // Leer audio del micrófono
  size_t bytesRead = 0;
  i2s_read(I2S_PORT, StreamBuffer, StreamBufferNumBytes, &bytesRead, portMAX_DELAY);


}



void i2s_install() {
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SOUND_SAMPLE_RATE,
    .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BIT_COUNT),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,
    .dma_buf_count = I2S_DMA_BUF_COUNT/*8*/,
    .dma_buf_len = I2S_DMA_BUF_LEN/*1024*/,
    .use_apll = false
  };
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };
  i2s_set_pin(I2S_PORT, &pin_config);
}
