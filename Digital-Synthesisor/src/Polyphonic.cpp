#include "Polyphonic.h"

std::vector<int> GetKeyIDs(std::string _KEYS){
    std::vector<int> result;
    for(char _key : _KEYS){
        result.push_back(keyMap[_key]);
    }
    return result;
}

std::unordered_map<char, int> keyMap = {
        {'\\', 0xDC},         {'Z', 0x5A},        {'X', 0x58},        {'C', 0x43},       {'V', 0x56},
        {'B', 0x42},          {'N', 0x4E},        {'M', 0x4D},        {',', 0xBC},       {'.', 0xBE},
        {'/', 0xBF},          {'A', 0x41},        {'S', 0x53},        {'D', 0x44},       {'F', 0x46},
        {'G', 0x47},          {'H', 0x48},        {'J', 0x4A},        {'K', 0x4B},       {'L', 0x4C},
        {';', 0xBA},          {'\'', 0xDE},       {'Q', 0x51},        {'W', 0x57},       {'E', 0x45},
        {'R', 0x52},          {'T', 0x54},        {'Y', 0x59},        {'U', 0x55},       {'I', 0x49},
        {'O', 0x4F},          {'P', 0x50},        {'[', 0xDB},        {']', 0xDD},       {'1', 0x31},
        {'2', 0x32},          {'3', 0x33},        {'4', 0x34},        {'5', 0x35},       {'6', 0x36},
        {'7', 0x37},          {'8', 0x38},        {'9', 0x39},        {'0', 0x30},       {'-', 0xBD}, 
        {'=', 0xBB},             
    };



void Scale::UpdateKeystates(){
    while(Tracking){
        for (size_t i = 0; i < NumKeys ; ++i) {
                    int key = KeyIDs[i];
                    bool ISPLAY = GetAsyncKeyState(key) & 0x8000;
                    if(ISPLAY){     
                            if (!(Notes[i]->playing())) { 
                                Notes[i]->toggle();
                                Notes[i]->evolve('A');
                                }}
                    else {
                            if (Notes[i]->playing()) {
                                Notes[i]->toggle();
                                Notes[i]->evolve('R');
                                }}
                    }

    }

};

//Public Methods
bool Scale::Start(){
    if(Tracking){return false;};
    Tracking = true;
    TrackerThread = std::thread(&Scale::UpdateKeystates, this);
    return true; 
}

bool Scale::Stop(){
    if(!Tracking){return false;};
    Tracking = false;
    if(TrackerThread.joinable()) TrackerThread.join();
    return true;
}


//initialising..
Scale::Scale(std::vector<std::shared_ptr<Note>> _Notes, std::string KeyString) : Notes(_Notes) , KeyIDs(GetKeyIDs(KeyString)) , Tracking(false) , NumKeys(KeyString.length()){
    Polyphony_init();
};

Scale::Scale(sSound sound_type , std::string KeyString ,float lowest_pitch , float volume) : KeyIDs(GetKeyIDs(KeyString)) , Tracking(false) , NumKeys(KeyString.length()){
    Notes = {};
    float temp = lowest_pitch;
    for(int i = 0 ; i < NumKeys ; ++i){
        std::shared_ptr<Note> TempPtr = std::make_shared<Note>(sound_type,temp ,volume);
        Notes.push_back(TempPtr);
        temp *= ScaleFactor;
    }  
    Polyphony_init(); 
};

void Scale::ChangeNotes(sSound sound_type){
    for(auto fog : Notes){
        (*fog).Sound_details = sound_type;
    }
}


void Scale::Polyphony_init(){
    CurrentSound = [this](){
            float result = 0.0f;
            for(int i = 0 ; i < Notes.size() ; ++i){
                result += Notes[i]->MoveWave();
            }
            value = result;
            return result;};
    }