#pragma once

#include "common.h"

class Synth{
public:
   
    Synth(int sampleRate, int bufferSize);
    ~Synth();
 
    bool play(std::function<float()>& waveFunc); 
    bool pause();                                
    void stop();
    float GetVolume();  
    std::array<float , 256> GetFrequencies();                    

private:
    
    int SAMPLE_RATE;       
    int BUFFER_SIZE;
       
    float volume;
    
    fftwf_plan plan;
    float* FFT_in;
    fftwf_complex* FFT_out;
    std::array<float , 256> frequencies;

    PaStream* stream;  

    std::function<float()>* waveform; 
    std::atomic<bool> running;         

    static int audioCallback(const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags statusFlags, void* userData); 
    void generateSignal(float* buffer, int numSamples); 
    void SetupFFT();
};
