#include "Synth.h"
 
bool Synth::setupAudio() {
        if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD_PTR)waveOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
            return false;

        for (int i = 0; i < NUM_BUFFERS; ++i) {
            waveHeaders[i].lpData = reinterpret_cast<LPSTR>(&buffers[i * BUFFER_SIZE]);
            waveHeaders[i].dwBufferLength = BUFFER_SIZE * sizeof(short);
            waveHeaders[i].dwFlags = 0;
            waveHeaders[i].dwLoops = 0;
            waveOutPrepareHeader(hWaveOut, &waveHeaders[i], sizeof(WAVEHDR));
        }

        bufferReady = NUM_BUFFERS;
        return true;
    }


 
Synth::Synth(int sampleRate, int amplitude, int numBuffers, int bufferSize)
        : SAMPLE_RATE(sampleRate), AMPLITUDE(amplitude), NUM_BUFFERS(numBuffers), BUFFER_SIZE(bufferSize),
          waveHeaders(numBuffers),running(false){

        this->wfx.wFormatTag = WAVE_FORMAT_PCM; 
        this->wfx.nSamplesPerSec = sampleRate;
        this->wfx.wBitsPerSample = 16;
        this->wfx.nChannels = 1;
        this->wfx.nBlockAlign = 2;
        this->wfx.nAvgBytesPerSec = 32;
        this->wfx.cbSize = 0;


        int temp = numBuffers* bufferSize;
        this->buffers.resize(temp);
        this->setupAudio();

        this->bufferReady = NUM_BUFFERS;
    }

Synth::~Synth() {
    this->Stop();}

bool Synth::play(std::function<double()>& waveFunc) {
        if (running) { return false;};
        
        waveform = &waveFunc;
        running = true;
        audioThread = std::thread(&Synth::audioLoop, this);

        return true;
        }

bool Synth::pause(){
    if (!running) return false;
            
        running = false;  
        if (audioThread.joinable()) audioThread.join();
        
        waveOutReset(hWaveOut);
        return false;
} 
    

void Synth::Stop() {
        this->pause();  
        for (auto& header : waveHeaders) {
            waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
        }
        waveOutClose(hWaveOut);
    }

void Synth::audioLoop() { 
        int currentBuffer = 0;
        double phaseIncrement = 1.0 / SAMPLE_RATE;

        while (this->running) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return (bufferReady  || !running) ; });

            if (!(this->running)) break;
            
            generateSignal(&(this->buffers)[currentBuffer * (this->BUFFER_SIZE)], this->BUFFER_SIZE);
            
            waveOutWrite(hWaveOut, &(waveHeaders[currentBuffer]), sizeof(WAVEHDR));
            
            this->bufferReady--;
            currentBuffer = (currentBuffer + 1) % this->NUM_BUFFERS;
        }
    }

void Synth::generateSignal(short* buffer, int numSamples) {
        
        for (int i = 0; i < numSamples; ++i) {        
            buffer[i] = static_cast<short>(AMPLITUDE * (*waveform)());
        }
        
    }