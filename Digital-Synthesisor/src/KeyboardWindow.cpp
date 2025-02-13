#include "DrawFrame.h" 


void ShowKeyBoardWindow(StateData& sd , CurrentVariables& cev , GlobalData& gd){
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(sd.display_w, sd.display_h));       
    ImGui::Begin("Fullscreen Window", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::Text("KEEEEYBOARRRRD");
    ShowVolumeBar(gd.synth.GetVolume());
    ShowFrequencySpectrum(gd.synth);

    if(ImGui::Button("Stop playing")){
        sd.drawMain = true;
        sd.drawKeyBoard = false;
        gd.synth.pause();
        cev.currentScale.Stop();
        gd.synth.play(cev.currentNote.nFunc);
        ImGui::End();
        return;
    }

   
    {
        
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 anchor(700, 500); // Pick any screen coordinates
        ImGui::SetCursorScreenPos(anchor);
        ImVec2 p = ImGui::GetCursorScreenPos();

        //white keys
        const float whiteKeyWidth  = 40.0f;
        const float whiteKeyHeight = 150.0f;
        const float blackKeyWidth  = 20.0f;
        const float blackKeyHeight = 90.0f;  
        float blackOffsets[5] = { 1.0f, 2.0f, 4.0f, 5.0f, 6.0f};
        const float octaveLength = whiteKeyWidth * 7.0f;
        ImU32 Black = IM_COL32(0, 0, 0, 255);
        ImU32 White = IM_COL32(255, 255, 255, 255);
        ImU32 Gold = IM_COL32(255, 215, 0, 255);
        ImU32 DarkGold = IM_COL32(184, 134, 11, 255);
        int index = 0;
        bool draw = false;
        for (int j = 0; j < 3; j++){
        
            for (int i = 0; i < 7; i++){
                
                ImVec2 top_left     = ImVec2(p.x + i * whiteKeyWidth + octaveLength * j, p.y);
                ImVec2 bottom_right = ImVec2(top_left.x + whiteKeyWidth, p.y + whiteKeyHeight);
                ImU32 StateColW = (cev.currentScale.Notes[index]->playing()) ? Gold : White;
                draw_list->AddRectFilled(top_left, bottom_right, StateColW);
                
                draw_list->AddRect(top_left, bottom_right, Black);
                ++index;

                if (draw){
                    float xCenter = p.x + ((float)(i) * whiteKeyWidth) + octaveLength * j;
                    ImVec2 black_top_left = ImVec2(xCenter - blackKeyWidth * 0.5f, p.y);
                    ImVec2 black_bottom_right = ImVec2(xCenter + blackKeyWidth * 0.5f, p.y + blackKeyHeight);
                    ImU32 StateColB = (cev.currentScale.Notes[index-2]->playing()) ? DarkGold : Black;
                    draw_list->AddRectFilled(black_top_left, black_bottom_right, StateColB);
                    draw_list->AddRect(black_top_left, black_bottom_right, Black); 
                    draw = false;
                }
                if(i != 2 && i !=6){
                    draw = true;
                    ++index;
                }   
            }
        }
    }
    ImGui::End();
}