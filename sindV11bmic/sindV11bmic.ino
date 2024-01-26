#include <driver/i2s.h>

#define I2S_BCK_PIN 32
#define I2S_SD_PIN 33
#define I2S_WS_PIN 25
#define I2S_PORT I2S_NUM_0

const uint8_t dma_count=8;
const uint16_t dma_len=256;

void i2s_install(){
 const i2s_config_t i2s_config={
   .mode=i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
   .sample_rate=44100,
   .bits_per_sample=I2S_BITS_PER_SAMPLE_32BIT,
   .channel_format=I2S_CHANNEL_FMT_ONLY_LEFT,
   .communication_format= i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
   .intr_alloc_flags=ESP_INTR_FLAG_LEVEL1,
   .dma_buf_count=dma_count,
   .dma_buf_len=dma_len,
   .use_apll=false,
 };
 
 const i2s_pin_config_t pin_config={
   .bck_io_num=I2S_BCK_PIN,
   .ws_io_num=I2S_WS_PIN,
   .data_out_num=I2S_PIN_NO_CHANGE,
   .data_in_num=I2S_SD_PIN
 };

 i2s_driver_install(I2S_PORT,&i2s_config,0,NULL);
 i2s_set_pin(I2S_PORT,&pin_config);
}

void setup(){
   i2s_install();
   Serial.begin(115200);
   Serial.printf("Inicio del programa de comunicación con el micrófono MEMS INMP441 mediante protocolo I2S.\nSe utilizará el Serial Plotter para visualizar los datos de audio.\n ");
   delay(2000);
}
void loop(){
   size_t bytes_read=0;
   int16_t samples=dma_len*dma_count;
   int32_t buffer[samples]; 
   uint16_t samples_read=0,range=1000;
   
   esp_err_t i2s_err= i2s_read(I2S_PORT,&buffer,samples*sizeof(int32_t),&bytes_read,portMAX_DELAY);
   samples_read=bytes_read/sizeof(int32_t);
   if(i2s_err==ESP_OK){
        for(uint16_t i=0; i<samples; i++){
         //Serial.print(range);
         //Serial.print(" ");
         //Serial.print(range * (-1));
         //Serial.print(" ");
         Serial.println(buffer[i]);
        }
    }
}
