#include <Arduino.h>
#include "AudioGeneratorAAC.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourcePROGMEM.h"

// Incluimos el archivo que acabas de crear con los datos reales
#include "sampleaac.h" 

AudioGeneratorAAC *aac;
AudioFileSourcePROGMEM *source;
AudioOutputI2S *out;

// --- CONFIGURACIÓN DE PINES PARA ESP32-S3 ---
#define I2S_BCLK 1
#define I2S_LRC  2
#define I2S_DOUT 4

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Iniciando Ejercicio 1 (Audio desde Memoria) - ESP32-S3");

  // Cargamos el archivo de sonido apuntando a 'sampleaac' (nombre definido dentro del .h)
  // y calculamos su tamaño real
  source = new AudioFileSourcePROGMEM(sampleaac, sizeof(sampleaac));

  out = new AudioOutputI2S();
  
  // Reasignamos los pines para tu ESP32-S3
  out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  
  // Opcional: Subir un poco el volumen (la ganancia)
  out->SetGain(0.125); 

  aac = new AudioGeneratorAAC();
  
  if (!aac->begin(source, out)) {
    Serial.println("Error al iniciar el generador AAC");
  }
}

void loop() {
  if (aac->isRunning()) {
    if (!aac->loop()) {
      aac->stop();
      Serial.println("Reproducción finalizada.");
    }
  } else {
    delay(1000);
  }
}