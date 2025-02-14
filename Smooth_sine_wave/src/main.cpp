/*
  SineWave

  Generates a pre-generated sawtooth-waveform.

  See the full documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/dac
*/

#include "analogWaveMod.h" // Include the library for analog waveform generation

   // Create an instance of the analogWave class, using the DAC pin

float carrierfreq = 1000;  //  carrier wave  freq in Hz
float modFreq = 100; // Modulated signal freq in Hz

const int sampling_rate = 200; // same as analogWaveMod

float t_delta = 1/(modFreq*sampling_rate); // Time step to get samples

float A_c = 1.0; // Amplitude of the carrier
float A_m = 0.9; // Amplitude of the modulated

uint16_t samples[sampling_rate-1] = {0}; 


void generateSamples(uint16_t* array);

void setup() {
  Serial.begin(115200);
  generateSamples(samples);
  
  // wave.sine(carrierfreq);  // Generate a sine wave with the initial frequency
}

void loop() {
  static analogWave wave(DAC,samples,sampling_rate-1,0);
  wave.begin(carrierfreq); 

  while (1);
  
   for(int i = 0; i < sampling_rate-1; i++){
    // wave.amplitude(samples[i]);
    // Serial.println(String(samples[i]));
    // delayMicroseconds((unsigned int) t_delta*1000000);
    // delay(100);
   }
}

void generateSamples(uint16_t* array){
  float t = 0;
  float c = 0; // To hold carrier values
  float m = 0; // To hold modulated values
  float s = 0; // To hold AM signal values
  float A_max = A_c + A_m;

  for(int i = 0; i < sampling_rate-1; i++){
    c = A_c*cos(2*PI*carrierfreq*t);
    m = A_m*cos(2*PI*modFreq*t);

    s = (1+m)*c+A_max;// AM signal sample
    s = s/(2*A_max); // Normalized AM signal sample
    s = (s*1000 - 0) * (43253 - 0) / (1000 - 0) + 0;
    array[i] = (uint16_t) s;  // Stores samples into the buffer
    // Serial.println( array[i]);
    // delay(100);
    t = t + t_delta;
  }
}