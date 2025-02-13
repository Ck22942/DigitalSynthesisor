#pragma once
#include "common.h"


class Synth{
    public:
        Synth(int sampleRate, int amplitude, int numBuffers, int bufferSize);
      
        ~Synth();

        bool play(std::function<double()>& waveFunc);
        bool setupAudio();
        bool pause();
        void Stop();

        std::atomic<int> bufferReady;
        std::condition_variable cv;
    
    private:
       //constants
       int SAMPLE_RATE;
       int AMPLITUDE;
       int NUM_BUFFERS;
       int BUFFER_SIZE;

       //audio variables
       HWAVEOUT hWaveOut;
       WAVEFORMATEX wfx;
       std::vector<short> buffers;
       std::vector<WAVEHDR> waveHeaders;
       std::function<double()>* waveform;

      //control variables
        std::atomic<bool> running;
        std::mutex mtx;
        std::thread audioThread;
        
       //private methods
       static void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR, DWORD_PTR) {
        if (uMsg == WOM_DONE) {
            Synth* synth = reinterpret_cast<Synth*>(dwInstance);
            synth->bufferReady++;
            synth->cv.notify_one();
        }
    }


       void audioLoop();

       void generateSignal(short* buffer, int numSamples);

};
