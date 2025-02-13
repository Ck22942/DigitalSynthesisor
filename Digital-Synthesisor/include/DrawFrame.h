#pragma once
#include "Polyphonic.h"
#include "Synth.h"

struct StateData{
    int display_w , display_h;
    bool drawMain = true;
    bool drawKeyBoard = false;
    bool drawEditor = false;
    bool appOn = true;
};

struct CurrentVariables{
    int currentPos = -1;
    Note currentNote;
    Scale currentScale = Scale(sSound() , "\\AZSXCFVGBHNMK,L.Q2W3E4RT6Y7UI9O0P-[" , 220.0f , 0.1f);
};

struct GlobalData{
    std::string keys = "\\AZSXCFVGBHNMK,L.Q2W3E4RT6Y7UI9O0P-[";
    std::vector<sSound> sounds = {};
    Synth synth = Synth(SampleRate, 256);
};

static void glfw_error_callback(int error, const char *description);
GLFWwindow* SetupGLFWAndImGui(const char* windowTitle, int width, int height, const char* glsl_version);

std::string getUniqueName(std::vector<sSound>& vec,const std::string& name , int ignoreIndex = -1);

void ShowVolumeBar(float volume);
void ShowFrequencySpectrum(Synth&);
void ShowTemporalPlot(Note&);
void ShowAmplitudePlot(Note&);

void ShowEditorWindow(StateData& sd , CurrentVariables& cev , GlobalData& gd);
void ShowMainWindow(StateData& sd , CurrentVariables& cev , GlobalData& gd);
void ShowKeyBoardWindow(StateData& sd , CurrentVariables& cev , GlobalData& gd);


void startframe(int& display_w , int& display_h, GLFWwindow* window);
void finishframe(int& display_w , int& display_h, GLFWwindow* window);

void cleanup(GLFWwindow* window , CurrentVariables& cev , StateData& sd , GlobalData& gd);

std::vector<sSound> readSounds(const std::string& filename);
void saveSounds(const std::string& filename, const std::vector<sSound>& sounds);