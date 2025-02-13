#include "Note.h"


void Note::FormCarrier(){
    this->CarrierSignal = [this](float t){
        float result = 0.0f;
        for(auto _partial : SPEC){
            switch(_partial.oscillator){
                case Sine:
                    result += _partial.Amplitude*Osc::Sine(t , 0.0f , pitch*_partial.frequency);
                    break;
                case Square:
                    result += _partial.Amplitude*Osc::Square(t , 0.0f , pitch*_partial.frequency);
                    break;
                case Triangle:
                    result += _partial.Amplitude*Osc::Triangle(t , 0.0f , pitch*_partial.frequency);
                    break;
                case SawTooth:
                    result += _partial.Amplitude*Osc::Sawtooth(t , 0.0f , pitch*_partial.frequency);
                    break;    
            }
        }   
        return result;
    };
}

void Note::FormADSRfunctions(){

    EnvelopeFunctions['Z'] = [](float x){return 0.0f;};
    EnvelopeFunctions['S'] = [this](float x){return EnvelopePath::Flat(A0);};
   
    EnvelopeFunctions['A'] = [this](float x){
        if(AmplitudeEvolution.AttackShape == Linear){
                    return EnvelopePath::LinUp(x , t0 , A0 , 1.0f ,AT);  //ATTACK
            }else if(AmplitudeEvolution.AttackShape == Exponential){
                    return EnvelopePath::ExpUp(x, t0, A0 ,1.0f, AT);
                }else{ return 0.0f;};
        };
    EnvelopeFunctions['D'] = [this](float x){
        if(AmplitudeEvolution.DecayShape == Linear){
                    return EnvelopePath::LinDown(x , t0 , A0 , SL, DT);  //ATTACK
            }else if(AmplitudeEvolution.DecayShape == Exponential){
                    return EnvelopePath::ExpDown(x, t0, A0 ,SL , DT);
                }else{return 0.0f;};
        };
    EnvelopeFunctions['R'] = [this](float x){
        if(AmplitudeEvolution.ReleaseShape == Linear){
                    return EnvelopePath::LinDown(x , t0 , A0 ,0.0f, RT);  //ATTACK
            }else if(AmplitudeEvolution.ReleaseShape == Exponential){
                    return EnvelopePath::ExpDown(x, t0, A0 ,0.0f, RT);
                }else{return 0.0f;};
        };

}

 

Note::Note(sSound _sound_details , float _pitch, float _volume):volume(_volume) ,pitch(_pitch), Sound_details(_sound_details){
    
    reset();
    FormADSRfunctions();
    FormCarrier();
    nFunc = [this](){return MoveWave();};
    
};

Note::Note(const Note& other) : Note(other.Sound_details , other.pitch , other.volume) {;};

Note::Note() :volume(0.1f) ,pitch(440.0f), Sound_details(sSound{}){
    
    reset();
    FormADSRfunctions();
    FormCarrier();
    nFunc = [this](){return MoveWave();};

};

void Note::UpdateTemporalPlot(){

    for(int i = 0; i < TemporalVec.size() ; ++i){
        TemporalVec[i] = CarrierSignal((float)i * TimeIncrement);
    }
}
//Update the Note..

void Note::toggle(){this->keystate = !keystate ;};


void Note::ChangeState(bool on, float A, float A0, float t, float t0 , char envstate){
    this->keystate = on;
    this->A = A;
    this->t = t;
    this->t0 = t0;
    this->A0 = A0;
    this->EnvState = envstate;
}

void Note::evolve(char state){
    this->EnvState = state;
    this->t0 = t;
    this->A0 = A;
    }


void Note::MoveState(){   
    if(EnvState == 'A'){
        if(t - t0 > AT){
            evolve('D');
        }
        return;
    }
}

float Note::MoveWave(){
    MoveState();
    WavePos = CarrierSignal(t);
    A = EnvelopeFunctions[EnvState](t);
    CurrentValue = A*WavePos;
    t += TimeIncrement;
    
    return volume*CurrentValue;
};

void Note::reset(){
    this->ChangeState(false, 0.0f,0.0f,0.0f,0.0f,'Z');
    this->WavePos = 0.0f;
    this->CurrentValue = 0.0f;
    } 


//GETTERS
char Note::getEnvState(){return this->EnvState;};
float Note::getTime(){return this->t;}
float Note::getAmplitude(){return this->A;}
float Note::getValue(){return this->CurrentValue;}
bool Note::playing(){return this->keystate;}


std::string Note::getInfo(){
    char buffer[256]; // Make sure this is large enough
    snprintf(buffer, sizeof(buffer),
             "*********\n%s: \nPitch: %.2f Hz\nVolume: %.2f\n********",
             name.c_str(), pitch, volume);
    return std::string(buffer);
};


void Note::UpdateAmplitudePlot(){
    float ST = 5.0f - AT - DT - RT;
    size_t i = 0;
    
    for(float time = 0.0f; time < AT ; time += 0.01){
        if(AmplitudeEvolution.AttackShape == Linear){
            AmplitudeVec[i] =  EnvelopePath::LinUp(time , 0.0f , 0.0f , 1.0f ,AT);  
        }
        if(AmplitudeEvolution.AttackShape == Exponential){
            AmplitudeVec[i] =  EnvelopePath::ExpUp(time, 0.0f, 0.0f ,1.0f, AT);
        }
        ++i;
    }
    for(float time = AT; time < DT + AT + ST; time += 0.01){
        if(AmplitudeEvolution.DecayShape == Linear){
            AmplitudeVec[i] =  EnvelopePath::LinDown(time , AT, 1.0f , SL ,DT);  
        }
        if(AmplitudeEvolution.DecayShape == Exponential){
            AmplitudeVec[i] =  EnvelopePath::ExpDown(time, AT , 1.0f , SL , DT);
        }
        ++i;
    }
    for(float time = AT + DT + ST; time < AT + DT + ST + RT; time += 0.01){
        if(AmplitudeEvolution.ReleaseShape == Linear){
            AmplitudeVec[i] =  EnvelopePath::LinDown(time , AT + DT + ST , SL , 0.0f ,RT);  
        }
        if(AmplitudeEvolution.ReleaseShape == Exponential){
            AmplitudeVec[i] =  EnvelopePath::ExpDown(time, AT + DT + ST , SL , 0.0f , RT);
        }
        ++i;
    }


}


//WAVEFORMS

float Note::Osc::Sine(float time, float phase, float frequency) {
     return sin(time * 2.0f * PI * frequency - phase);
};

float Note::Osc::Square(float time ,float phase, float frequency) {
    return 4.0f*floor(frequency*time) - 2.0f*floor(2.0f*frequency*time) + 1.0f;
};

float Note::Osc::Triangle(float time , float phase, float frequency) {
    float period = 1.0f/ frequency;
    float fractionalPart = fmod(time, period) / period;
    return 4.0f * fabs(fractionalPart - 0.5f) - 1.0f;
};

float Note::Osc::Sawtooth(float time , float phase, float frequency) {
    float period = 1.0f/frequency;
    return 2.0f * (fmod(time, period) / period) - 1.0f;
};



//ENVPATHS

float Note::EnvelopePath::LinUp(float t , float t0, float L0, float L1, float dt){
    return std::min(L0 + (t - t0)/dt , L1);
}

float Note::EnvelopePath::ExpUp(float t , float t0, float L0, float L1, float dt){
    float dL = L1 - L0;
    return L0 + dL * ( 1.0f - exp(-3.5f*(t-t0)/ dt));
}

float Note::EnvelopePath::LinDown(float t, float t0, float L0, float L1, float dt) {    
    return  std::max(L0 - (t - t0)/dt , L1);

}

float Note::EnvelopePath::ExpDown(float t, float t0, float L0, float L1, float dt) {
    float dL = L0 - L1;
    return L1 + dL * exp(-3.5f*(t - t0) / dt);
}

float Note::EnvelopePath::Flat(float L0){
    return L0;
}
