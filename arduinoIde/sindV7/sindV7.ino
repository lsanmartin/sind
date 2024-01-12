#include <driver/i2s.h>
#include <SPIFFS.h>
#include <ESP_Mail_Client.h>
#include <SimpleFTPServer.h>

#define I2S_WS 15
#define I2S_SD 13
#define I2S_SCK 2
#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE (16000)
#define I2S_SAMPLE_BITS (8)
#define I2S_READ_LEN (8 * 1024) // Tamaño del buffer de lectura I2S
#define RECORD_TIME (3) // Duración de la grabación en segundos
#define I2S_CHANNEL_NUM (1)
#define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME)


// crea parámetros del servidor web para descargar el archivo de prueba 
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#define DBG_OUTPUT_PORT Serial


// Configuración del cliente SMTP
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "app.indx@gmail.com"
#define AUTHOR_PASSWORD "PAya9276.."


SMTPSession smtp;

FtpServer ftpSrv;  

// const char* ssid = "_lsm";
// const char* password = "7292767292";
const char* ssid = "VTR-1493572";
const char* password = "Xt5qdvkprjgz";

const char* host = "esp32fs";
WebServer server(80);


void setup() {
  Serial.begin(115200);
  SPIFFSInit();
  i2sInit();
  xTaskCreate(i2s_adc, "i2s_adc", 1024 * 4, NULL, 1, NULL); // Aumento del tamaño de la pila


  //WIFI INIT
  DBG_OUTPUT_PORT.printf("Connecting to %s\n", ssid);
  if (String(WiFi.SSID()) != String(ssid)) {
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    
    WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DBG_OUTPUT_PORT.print(".");
  }
  DBG_OUTPUT_PORT.println("");
  DBG_OUTPUT_PORT.print("Connected! IP address: ");
  DBG_OUTPUT_PORT.println(WiFi.localIP());

  MDNS.begin(host);
  DBG_OUTPUT_PORT.print("Open http://");
  DBG_OUTPUT_PORT.print(host);
  DBG_OUTPUT_PORT.println(".local/edit to see the file browser");

  
  // Ruta para listar archivos
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", getHtmlListOfFiles());
     DBG_OUTPUT_PORT.println(getHtmlListOfFiles());
  });
  
     // Ruta para descargar archivos
server.on("/download", HTTP_GET, []() {
    String path = "/download/recording.wav";
    if (SPIFFS.exists(path)) {
        File file = SPIFFS.open(path, "r");
        if (file) {
            server.setContentLength(file.size());
            server.send(200, "application/octet-stream", "");

            uint8_t buf[2048]; // Aumentar el tamaño del buffer si es necesario
            while (file.available()) {
                size_t len = file.read(buf, sizeof(buf));
                server.sendContent_P((const char *)buf, len);
                yield(); // Permite que el background tasks se ejecute
            }
            file.close();
        } else {
            server.send(500, "text/plain", "No se puede abrir el archivo");
        }
    } else {
        server.send(404, "text/plain", "Archivo no encontrado");
    }
});


  
  // Iniciar servidor
  server.begin();


  // Inicializa el sistema de archivos, por ejemplo SPIFFS
  SPIFFS.begin(true);

  if (!SPIFFS.begin(true)) {
    Serial.println("Error al inicializar SPIFFS");
    // Manejo del error
}

  // Configura y arranca el servidor FTP
  // ftpSrv.begin("_lsm","7292767292");

  /////FTP Setup, ensure SPIFFS is started before ftp;  /////////
  #ifdef ESP32       //esp32 we send true to format spiffs if cannot mount
    if (SPIFFS.begin(true)) {
  #elif defined ESP8266
    if (SPIFFS.begin()) {
  #endif
        Serial.println("SPIFFS opened!");
        ftpSrv.begin("_lsm","7292767292");    //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)
    }   

  //  listFilesInDir(SPIFFS);
  listFilesInDir(SPIFFS, "/download", 0);
  
}

void loop() {
   server.handleClient();
  delay(2);//allow the cpu to switch to other tasks

  ftpSrv.handleFTP();   
  
}


File file;
const char filename[] = "/download/recording.wav";
const int headerSize = 44;

void SPIFFSInit(){
  if(!SPIFFS.begin(true)){
        Serial.println("SPIFFS initialisation failed!");
        while(1) yield();
    }

    // Crear un directorio si no existe
    if(!SPIFFS.exists("/download")){
        File dir = SPIFFS.open("/download", FILE_WRITE);
        if(dir){
            dir.close();
        }
    }

    String filePath = String(filename);
    SPIFFS.remove(filePath);
    file = SPIFFS.open(filePath, FILE_WRITE);
    if(!file){
        Serial.println("File is not available!");
    }

  byte header[headerSize];
  wavHeader(header, FLASH_RECORD_SIZE);

  file.write(header, headerSize);
  listSPIFFS();
}

// Serial.println(filename);

void i2sInit(){
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT, // Cambio a 8 bits
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 64,
    .dma_buf_len = 1024,
    .use_apll = 1
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}


void i2s_adc_data_scale(uint8_t * d_buff, uint8_t* s_buff, uint32_t len)
{
    uint32_t j = 0;
    uint32_t dac_value = 0;
    for (int i = 0; i < len; i += 2) {
        dac_value = ((((uint16_t) (s_buff[i + 1] & 0xf) << 8) | ((s_buff[i + 0]))));
        d_buff[j++] = 0;
        d_buff[j++] = dac_value * 256 / 2048;
    }
}

void i2s_adc(void *arg) {
  int i2s_read_len = I2S_READ_LEN;
  int flash_wr_size = 0;
  size_t bytes_read;

  char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
  uint8_t* flash_write_buff = (uint8_t*) calloc(i2s_read_len, sizeof(char));

  Serial.println(" *** Recording Start *** ");
  while (flash_wr_size < FLASH_RECORD_SIZE) {
    i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
    i2s_adc_data_scale(flash_write_buff, (uint8_t*)i2s_read_buff, i2s_read_len);
    file.write((const byte*) flash_write_buff, bytes_read);
    flash_wr_size += bytes_read;
    ets_printf("Sound recording %u%%\n", flash_wr_size * 100 / FLASH_RECORD_SIZE);
  }
  file.close();

  free(i2s_read_buff);
  i2s_read_buff = NULL;
  free(flash_write_buff);
  flash_write_buff = NULL;

  listSPIFFS();
  vTaskDelete(NULL);
}

void example_disp_buf(uint8_t* buf, int length)
{
    printf("======\n");
    for (int i = 0; i < length; i++) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("======\n");
}

void wavHeader(byte* header, int wavSize){
  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  unsigned int fileSize = wavSize + headerSize - 8;
  header[4] = (byte)(fileSize & 0xFF);
  header[5] = (byte)((fileSize >> 8) & 0xFF);
  header[6] = (byte)((fileSize >> 16) & 0xFF);
  header[7] = (byte)((fileSize >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10;
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01;
  header[21] = 0x00;
  header[22] = 0x01;
  header[23] = 0x00;
  header[24] = 0x80;
  header[25] = 0x3E;
  header[26] = 0x00;
  header[27] = 0x00;
  header[28] = 0x00;
  header[29] = 0x7D;
  header[30] = 0x00;
  header[31] = 0x00;
  header[32] = 0x02;
  header[33] = 0x00;
  header[34] = 0x10;
  header[35] = 0x00;
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(wavSize & 0xFF);
  header[41] = (byte)((wavSize >> 8) & 0xFF);
  header[42] = (byte)((wavSize >> 16) & 0xFF);
  header[43] = (byte)((wavSize >> 24) & 0xFF);
  
}


void listSPIFFS(void) {
  Serial.println(F("\r\nListing SPIFFS files:"));
  static const char line[] PROGMEM =  "=================================================";

  Serial.println(FPSTR(line));
  Serial.println(F("  File name                              Size"));
  Serial.println(FPSTR(line));

  fs::File root = SPIFFS.open("/");
  if (!root) {
    Serial.println(F("Failed to open directory"));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F("Not a directory"));
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      // File path can be 31 characters maximum in SPIFFS
      int spaces = 33 - fileName.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 10 - fileSize.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }

  Serial.println(FPSTR(line));
  Serial.println();
  delay(1000);
}




String getHtmlListOfFiles() {
  String html = "<h1>Lista de Archivos</h1><ul>";
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file){
    String fileName = file.name();
    html += "<li><a href=\"" + fileName + "\">" + fileName + "</a></li>";
    file = root.openNextFile();
  }
  html += "</ul>";
  return html;
}


void sendEmail() {
  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  SMTP_Message message;
  message.sender.name = "ESP32";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Archivo de grabación";
  message.addRecipient("lsm", "lsanmartin@gmail.com");

  String contenidoEmail = "Se adjunta el archivo de grabación.";
  message.text.content = contenidoEmail.c_str();
  
  // Adjunta el archivo desde SPIFFS
  ESP_Mail_Attachment attach;
  attach.descr.filename = "/download/recording.wav";
  attach.descr.mime = "audio/wav";
  attach.file.storage_type = esp_mail_file_storage_type_flash;
  message.addAttachment(attach);

  // Iniciar sesión y enviar correo
  if (!smtp.connect(&session)) {
    Serial.println("Error al conectar con el servidor SMTP");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error al enviar correo");
    Serial.println("Error: " + smtp.errorReason());
  }
}


void listFilesInDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File dir = fs.open(dirname);
  if (!dir) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!dir.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = dir.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listFilesInDir(fs, file.name(), levels -1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = dir.openNextFile();
  }
}
