#pragma once
#include "Note.h" 

extern std::unordered_map<char, int> keyMap;


std::vector<int> GetKeyIDs(std::string _KEYS);


class Scale{
    protected:
        std::vector<int> KeyIDs;
        size_t NumKeys;
        void UpdateKeystates();
        bool Tracking;
        std::thread TrackerThread;
        void Polyphony_init();
    public:
        Scale() = default;
        Scale(sSound sound_type , std::string KeyString , float lowest_pitch , float volume);
        Scale(std::vector<std::shared_ptr<Note>> _Notes, std::string KeyString);
        std::function<float()> CurrentSound;
        std::vector<std::shared_ptr<Note>> Notes;

        void ChangeNotes(sSound sound_type);

        float value;
        //Public Methods
        void PrintDetails();
        bool Start();
        bool Stop(); 
};
