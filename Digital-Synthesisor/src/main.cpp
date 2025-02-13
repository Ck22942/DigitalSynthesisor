#include "DrawFrame.h"
#include "Synth.h"

int main(int, char **){
    

    // Our data
    static GlobalData gd;
    static StateData sd;
    static CurrentVariables cev;
    gd.sounds = readSounds("data/sounds.txt");

 
    gd.synth.play(cev.currentNote.nFunc);

    //Set up GUI
    GLFWwindow* window = SetupGLFWAndImGui("Synth App TEST", 1920, 1080, "#version 130");
    
    while (!glfwWindowShouldClose(window) && sd.appOn){
        startframe(sd.display_w, sd.display_h, window);
        
        // 
        if (sd.drawMain){
            ShowMainWindow(sd , cev , gd);
        }
        else if (sd.drawEditor){
            ShowEditorWindow(sd, cev, gd);
        }
        else if(sd.drawKeyBoard){
            ShowKeyBoardWindow(sd, cev, gd);
        }            
        //

        finishframe(sd.display_w, sd.display_h, window);
    }

    // cleanup
    cleanup(window , cev , sd , gd);

    return 0;
}
