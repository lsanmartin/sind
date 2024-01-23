

#include <driver/i2s.h>
#include <BluetoothA2DPSink.h>

// I2S pin assignment for INMP441
#define I2S_WS               25
#define I2S_SD               32
#define I2S_SCK              33
#define I2S_SAMPLE_BIT_COUNT 16
#define SOUND_SAMPLE_RATE    16000
#define SOUND_CHANNEL_COUNT  1

// Configuración de I2S
i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SOUND_SAMPLE_RATE,
    .bits_per_sample = (i2s_bits_per_sample_t)I2S_SAMPLE_BIT_COUNT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
};

i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
};

BluetoothA2DPSink a2dp_sink;

void setup() {
    Serial.begin(115200);

    // Inicialización de I2S
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);

    // Inicialización de Bluetooth A2DP Sink
    a2dp_sink.start("ESP32_A2DP_SINK");

    // Más configuración si es necesario...
}

void loop() {
    // Buffer para datos de audio
    uint8_t audio_data[512];
    size_t bytes_read;

    // Leer datos de audio de I2S
    i2s_read(I2S_NUM_0, &audio_data, sizeof(audio_data), &bytes_read, portMAX_DELAY);

    // Enviar datos de audio a través de Bluetooth
    if (bytes_read > 0) {
        a2dp_sink.write_data(audio_data, bytes_read);
    }

    // Más código si es necesario...
}
