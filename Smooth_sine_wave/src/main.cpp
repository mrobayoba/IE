/*
  SineWave

  Generates a pre-generated sawtooth-waveform.

  See the full documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/dac
*/

#include "analogWaveMod.h" // Include the library for analog waveform generation

analogWave wave(DAC);   // Create an instance of the analogWave class, using the DAC pin

int carrierfreq = 1000;  //  carrier wave  freq in Hz
int modFreq = 1; // Modulated signal freq in Hz

const int sampling_rate = 200; // same as analogWaveMod

float t_delta = 1/(modFreq*sampling_rate); // Time step to get samples

float A_c = 1.0; // Amplitude of the carrier
float A_m = 0.9; // Amplitude of the modulated

float samples[sampling_rate] = {0}; 


void generateSamples(float* array);

void setup() {
  generateSamples(samples);
  wave.sine(carrierfreq);  // Generate a sine wave with the initial frequency
}

void loop() {
   for(int i = 0; i < sampling_rate-1; i++){
    wave.amplitude(samples[i]);
    delayMicroseconds((unsigned int) t_delta*1000000);
   }
}

void generateSamples(float* array){
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
    array[i] = s;  // Stores samples into the buffer
    t = t + t_delta;
  }
}