#pragma once

#include "SoundConfig.h"


class Note{
    protected:
        //ID
        

        //Settings
        std::unordered_map<char,std::function<float(float)>> EnvelopeFunctions;
        
        //Pitch and Volume
        float volume , pitch; 
          
        //State
        std::atomic<char> EnvState;
        float t , t0;
        float A , A0;
        std::atomic<bool> keystate;
        float WavePos , CurrentValue;
        

        //plot wave
        
        //details
        std::vector<sPartial>& SPEC = Sound_details.spec;
        TotEnv& AmplitudeEvolution = Sound_details.env;
        std::string& name = Sound_details.name;
        float& AT = AmplitudeEvolution.AttackTime;
        float& DT = AmplitudeEvolution.DecayTime;
        float& SL = AmplitudeEvolution.SustainLevel;
        float& RT = AmplitudeEvolution.ReleaseTime;
        
        //Initialising...
        
        void FormCarrier();
        void FormADSRfunctions();
        
               
        void ChangeState(bool on, float A, float A0, float t, float t0 , char envstate);
        void MoveState();
               
        //Main functions to call
        std::function<float(float)> CarrierSignal , ModulatingSignal;
        
    public:
        std::function<float()> nFunc;
        void UpdateTemporalPlot();
        void UpdateAmplitudePlot();

        std::array<float,4096> TemporalVec;
        std::array<float,512> AmplitudeVec;

        sSound Sound_details;
        //constructors
        Note(sSound sound_details , float pitch, float volume);
        Note(const Note& other);
        Note();


        //Super Duper important
        virtual float MoveWave();

        //setters
        void toggle();
        void evolve(char state);
        void reset();

        //getters
        std::string getInfo();
        char getEnvState();
        float getTime();
        float getAmplitude();
        float getValue();
        bool playing();



       //STATIC METHODS
    public:
        class EnvelopePath{
            public: 
                static float LinUp(float t , float t0, float L0,float L1, float dt);
                
                static float ExpUp(float t , float t0, float L0, float L1, float dt);

                static float LinDown(float t, float t0, float L0, float L1, float dt) ;    
                
                static float ExpDown(float t, float t0, float L0, float L1, float dt) ;
                  
                static float Flat(float);
                                
        };
        class Osc{
            public:
                static float Sine(float time, float phase, float frequency) ;
            
                static float Square(float time ,float phase, float frequency) ;
                
                static float Sawtooth(float time , float phase, float frequency) ;
                
                static float Triangle(float time , float phase, float frequency) ;
                 
                static float Compute(float time , float frequency);
        };
             
};
