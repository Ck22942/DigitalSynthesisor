#include "Synth.h"

void Synth::generateSignal(float* buffer, int numSamples) {
        
    for (int i = 0; i < numSamples; ++i) {        
        buffer[i] = (*waveform)();     
    }
    
    
 }

Synth::Synth(int sampleRate, int bufferSize)
    : SAMPLE_RATE(sampleRate), BUFFER_SIZE(256), waveform(nullptr), running(false), stream(nullptr){
    Pa_Initialize();
    Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, SAMPLE_RATE, BUFFER_SIZE, audioCallback, this);
    SetupFFT();
}

Synth::~Synth() {
    stop();
    Pa_Terminate();
}



bool Synth::play(std::function<float()>& waveFunc) {
    if (running) return false;
    waveform = &waveFunc;
    running = true;

    Pa_StartStream(stream);
    return true;
}


bool Synth::pause() {
    if (!running) return false;
    running = false;
    if (stream) Pa_StopStream(stream);
    return true;
}

void Synth::stop() {
    pause();
    if (stream) {
        Pa_CloseStream(stream);
        stream = nullptr;
    }
    fftwf_destroy_plan(plan);
    fftwf_free(FFT_in);
    fftwf_free(FFT_out);
}


int Synth::audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    Synth* synth = static_cast<Synth*>(userData);
    float* out = static_cast<float*>(outputBuffer);

    if (!synth->running) return paComplete;
    
    synth->generateSignal(out, framesPerBuffer);
    memcpy(synth->FFT_in, out, framesPerBuffer * sizeof(float));
    
    //FFT
    fftwf_execute(synth->plan);
    for(unsigned long i = 0; i < framesPerBuffer ; ++i){
        float real = synth->FFT_out[i][0]/(float)framesPerBuffer;
        float imag = synth->FFT_out[i][1]/(float)framesPerBuffer;
        synth->frequencies[i] = sqrtf(real * real + imag * imag);
    }
    
    //volume
    float vol = 0.0f;
    for(unsigned long i = 0; i < framesPerBuffer ; ++i){
        vol = std::max(vol , out[i]);
    }
    synth->volume = vol;

    return paContinue;
}

float Synth::GetVolume(){
    return this->volume;
}

std::array<float , 256> Synth::GetFrequencies(){
    return this->frequencies;
}

void Synth::SetupFFT(){
    this->FFT_in = fftwf_alloc_real(BUFFER_SIZE);
    this->FFT_out = fftwf_alloc_complex(BUFFER_SIZE / 2 + 1);
    this->plan = fftwf_plan_dft_r2c_1d(BUFFER_SIZE, FFT_in, FFT_out, FFTW_ESTIMATE);
}