#include <Arduino.h>
#include <driver/i2s.h>

// --- CONFIGURACIÓN DE PINES ---
// Altavoz (MAX98357A)
#define SPEAKER_I2S_NUMBER I2S_NUM_0
#define SPEAKER_BCLK 1
#define SPEAKER_LRC  2
#define SPEAKER_DOUT 4

// Micrófono (INMP441)
#define MIC_I2S_NUMBER     I2S_NUM_1
#define MIC_SCK  5
#define MIC_WS   6
#define MIC_SD   7

// --- PARÁMETROS DE AUDIO ---
#define SAMPLE_RATE     16000
#define RECORD_TIME_SEC 2
#define BUFFER_SIZE     (SAMPLE_RATE * RECORD_TIME_SEC)

// Buffer para guardar el audio (16 bits por muestra)
int16_t *audio_buffer = NULL;

void setup_i2s() {
  // 1. Configurar I2S para el Altavoz (Salida)
  i2s_config_t speaker_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };
  i2s_pin_config_t speaker_pins = {
    .bck_io_num = SPEAKER_BCLK,
    .ws_io_num = SPEAKER_LRC,
    .data_out_num = SPEAKER_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  i2s_driver_install(SPEAKER_I2S_NUMBER, &speaker_config, 0, NULL);
  i2s_set_pin(SPEAKER_I2S_NUMBER, &speaker_pins);

  // 2. Configurar I2S para el Micrófono (Entrada)
  i2s_config_t mic_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };
  i2s_pin_config_t mic_pins = {
    .bck_io_num = MIC_SCK,
    .ws_io_num = MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = MIC_SD
  };
  i2s_driver_install(MIC_I2S_NUMBER, &mic_config, 0, NULL);
  i2s_set_pin(MIC_I2S_NUMBER, &mic_pins);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Reservar memoria en la RAM para los 2 segundos
  audio_buffer = (int16_t *)malloc(BUFFER_SIZE * sizeof(int16_t));
  if (audio_buffer == NULL) {
    Serial.println("Error: No hay memoria suficiente para el buffer");
    while(1);
  }

  setup_i2s();

  // --- PROCESO DE GRABACIÓN ---
  Serial.println(">>> Empieza a grabar (2 segundos)...");
  
  size_t bytes_read;
  // i2s_read llena el buffer con los datos del micro
  i2s_read(MIC_I2S_NUMBER, audio_buffer, BUFFER_SIZE * sizeof(int16_t), &bytes_read, portMAX_DELAY);
  
  Serial.println(">>> Acaba de grabar.");
  
  // Apagamos el micro para ahorrar recursos mientras reproducimos
  i2s_stop(MIC_I2S_NUMBER); 
}

void loop() {
  size_t bytes_written;
  
  // Reproducir el contenido del buffer en el altavoz
  i2s_write(SPEAKER_I2S_NUMBER, audio_buffer, BUFFER_SIZE * sizeof(int16_t), &bytes_written, portMAX_DELAY);
  
  // Pequeña pausa antes de repetir el bucle
  delay(500); 
}