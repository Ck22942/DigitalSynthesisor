#pragma once
#include "common.h"

enum EPhases{Attack, Decay, Sustain, Release};
enum EFuncs{Linear , Exponential};
enum WaveFunctions{Sine, Square, Triangle, SawTooth};

struct sPartial{
    float frequency;
    float Amplitude;
    int oscillator;

    sPartial(float _f , float _A , int _osc) : frequency(_f) , Amplitude(_A) , oscillator(_osc){;};
    sPartial() : sPartial(1.0f,1.0f,0){;};
};

struct TotEnv{

    float AttackTime;
    float DecayTime;
    float SustainLevel; 
    float ReleaseTime;
    
    int AttackShape;
    int DecayShape;
    int ReleaseShape;

    TotEnv() : AttackTime(0.1f) , DecayTime(0.1f) , SustainLevel(0.8f) , ReleaseTime(0.1f) , AttackShape(0) , DecayShape(0) , ReleaseShape(0){;};
};


struct sSound{
    std::string name;
    TotEnv env;
    std::vector<sPartial> spec;

    sSound() : env(TotEnv()) , name("New Sound") , spec({sPartial()}){;}; 
    sSound(int nom) : env(TotEnv()) , name("") , spec({}){;}; 
};
