#include "DrawFrame.h"    

        
void ShowEditorWindow(StateData& sd , CurrentVariables& cev , GlobalData& gd){
        
        sSound& _CurrentSound = cev.currentNote.Sound_details;
         
        std::vector<sPartial>& SpectrumTab = _CurrentSound.spec;
        TotEnv& EnvelopeTab = _CurrentSound.env;
        std::string& NameTab = _CurrentSound.name;
        
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(sd.display_w , sd.display_h));       
        ImGui::Begin("Fullscreen Window", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        
        ImGui::BeginChild("TableScroll", ImVec2(200, 300), true);
        //state stuff
        static int selected_row = -1;
        static int edit_row = -1;
        static bool show_popup = false;
        static sPartial edit_partial(0.0f,0.0f,0);

        ImGui::BeginTable("MyTable", 3);  

        // Table headers
        ImGui::TableSetupColumn("osc");
        ImGui::TableSetupColumn("rf");
        ImGui::TableSetupColumn("rA");
        ImGui::TableHeadersRow();
     
        int row = 0;
        for (auto& _partial : SpectrumTab) {
            ImGui::TableNextRow();
        
            std::string plomp;
            switch(_partial.oscillator){
                case 0:
                    plomp = "Sine";
                    break;
                case 1:
                    plomp = "Square";
                    break;

                case 2:
                    plomp = "Triangle";
                    break;

                case 3:
                    plomp = "SawTooth";
                    break;
                
                default:
                    plomp = "Unknown";
                    break;
            }
            ImGui::TableSetColumnIndex(0);
            
            bool is_selected = (selected_row == row);

            ImGui::PushID(row);

            if (ImGui::Selectable(plomp.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)){
                selected_row = row;
            }
            ImGui::PopID();
           
            
            if (ImGui::BeginPopupContextItem()) {

                float tempf = _partial.frequency;
                ImGui::SliderFloat("Edit Frequency", &tempf, 0.0f, 3.0f, "%.2f");
                _partial.frequency = tempf;

                float tempA = _partial.Amplitude;
                ImGui::SliderFloat("Edit Amplitude", &tempA, 0.0f, 1.5f, "%.2f");
                _partial.Amplitude = tempA;

                if(ImGui::RadioButton("Sine", &(_partial.oscillator), 0));  
                if(ImGui::RadioButton("Square", &(_partial.oscillator), 1));  
                if(ImGui::RadioButton("Triangle",&(_partial.oscillator), 2));  
                if(ImGui::RadioButton("SawTooth", &(_partial.oscillator), 3));

                if (ImGui::MenuItem("delete")) {
                    SpectrumTab.erase(SpectrumTab.begin() + row);
                }
                if (ImGui::MenuItem("clone")) {
                    SpectrumTab.emplace_back(_partial);
                }

                ImGui::EndPopup();
            }

            

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2f" , _partial.frequency);
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.2f" , _partial.Amplitude);

            ++row;      
        }

        ImGui::EndTable();

        if (ImGui::Button("New Partial")){
            SpectrumTab.emplace_back(1.0f,1.0f,0);
        }

        ImGui::EndChild();
        
        ImGui::SameLine();
        
        ImGui::BeginChild("Time Plot", ImVec2(485, 300), false);
        cev.currentNote.UpdateTemporalPlot();
        ShowTemporalPlot(cev.currentNote);
        ImGui::EndChild();

        ImGui::NewLine();

        ImGui::BeginChild("EnvChild", ImVec2(200, 300), false);
        if (ImGui::Selectable(("Attack Time(" + std::to_string(EnvelopeTab.AttackTime) + "s)").c_str())) {}
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("PopupSlider1");
        }
        if (ImGui::BeginPopup("PopupSlider1")) {
            float temp = static_cast<float>(EnvelopeTab.AttackTime);
            if (ImGui::SliderFloat("Edit Time", &temp, 0.0f, 0.5f, "%.3f")) EnvelopeTab.AttackTime = temp;
            ImGui::EndPopup();
        }
        // Slider 2
        if (ImGui::Selectable(("Decay Time(" + std::to_string(EnvelopeTab.DecayTime) + "s)").c_str())) {}
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("PopupSlider2");
        }
        if (ImGui::BeginPopup("PopupSlider2")) {
            float temp = static_cast<float>(EnvelopeTab.DecayTime);
            if (ImGui::SliderFloat("Edit Time", &temp, 0.0f, 2.0f, "%.2f")) EnvelopeTab.DecayTime = temp;
            ImGui::EndPopup();
        }

        // Slider 3
        if (ImGui::Selectable(("Release Time(" + std::to_string(EnvelopeTab.ReleaseTime) + "s)").c_str())) {}
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("PopupSlider3");
        }
        if (ImGui::BeginPopup("PopupSlider3")) {
            float temp = static_cast<float>(EnvelopeTab.ReleaseTime);
            if (ImGui::SliderFloat("Edit Time", &temp, 0.0f, 2.0f, "%.2f")) EnvelopeTab.ReleaseTime = temp;
            ImGui::EndPopup();
        }

        // Slider 4
        if (ImGui::Selectable(("Sustain Level(" + std::to_string(EnvelopeTab.SustainLevel) + ")").c_str())) {}
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("PopupSlider4");
        }
        if (ImGui::BeginPopup("PopupSlider4")) {
            float temp = static_cast<float>(EnvelopeTab.SustainLevel);
            if (ImGui::SliderFloat("Edit Level", &temp, 0.0f, 1.5f, "%.2f")) EnvelopeTab.SustainLevel = temp;
            ImGui::EndPopup();
        }

        // Mode 1
        if (ImGui::Selectable(("Attack Shape(" + std::string(EnvelopeTab.AttackShape == 0 ? "Linear" : EnvelopeTab.AttackShape == 1 ? "Exponential" : "Logarithmic") + ")").c_str())) {}
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("PopupMode1");
        }
        if (ImGui::BeginPopup("PopupMode1")) {
            if (ImGui::RadioButton("Linear", EnvelopeTab.AttackShape == 0)) EnvelopeTab.AttackShape = 0;
            if (ImGui::RadioButton("Exponential", EnvelopeTab.AttackShape == 1)) EnvelopeTab.AttackShape = 1;
            if (ImGui::RadioButton("Logarithmic", EnvelopeTab.AttackShape == 2)) EnvelopeTab.AttackShape = 2;
            ImGui::EndPopup();
        }

        // Mode 2
        if (ImGui::Selectable(("Decay Shape(" + std::string(EnvelopeTab.DecayShape == 0 ? "Linear" : EnvelopeTab.DecayShape == 1 ? "Exponential" : "Logarithmic") + ")").c_str())) {}
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("PopupMode2");
        }
        if (ImGui::BeginPopup("PopupMode2")) {
            if (ImGui::RadioButton("Linear", EnvelopeTab.DecayShape == 0)) EnvelopeTab.DecayShape = 0;
            if (ImGui::RadioButton("Exponential", EnvelopeTab.DecayShape == 1)) EnvelopeTab.DecayShape = 1;
            if (ImGui::RadioButton("Logarithmic", EnvelopeTab.DecayShape == 2)) EnvelopeTab.DecayShape = 2;
            ImGui::EndPopup();
        }

        // Mode 3
        if (ImGui::Selectable(("Release Shape(" + std::string(EnvelopeTab.ReleaseShape == 0 ? "Linear" : EnvelopeTab.ReleaseShape == 1 ? "Exponential" : "Logarithmic") + ")").c_str())) {}
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("PopupMode3");
        }
        if (ImGui::BeginPopup("PopupMode3")) {
            if (ImGui::RadioButton("Linear", EnvelopeTab.ReleaseShape == 0)) EnvelopeTab.ReleaseShape = 0;
            if (ImGui::RadioButton("Exponential", EnvelopeTab.ReleaseShape == 1)) EnvelopeTab.ReleaseShape = 1;
            if (ImGui::RadioButton("Logarithmic", EnvelopeTab.ReleaseShape == 2)) EnvelopeTab.ReleaseShape = 2;
            ImGui::EndPopup();
        }
        ImGui::EndChild();
       
        ImGui::SameLine();
        
        ImGui::BeginChild("Amplitude Plot", ImVec2(485, 300), false);
        cev.currentNote.UpdateAmplitudePlot();
        ShowAmplitudePlot(cev.currentNote);
        ImGui::EndChild();
        
        ImGui::SameLine();

        ImGui::BeginChild("Spectrum Stuff", ImVec2(485, 400), false);
        ImGui::Button("Preview");
        if(ImGui::IsItemActivated() && !cev.currentNote.playing()){
            cev.currentNote.toggle();
            cev.currentNote.evolve('A');  
        } else if ((!ImGui::IsItemActive()) && cev.currentNote.playing()){
            cev.currentNote.toggle();
            cev.currentNote.evolve('R');  
        }
        ShowVolumeBar(gd.synth.GetVolume());
        ShowFrequencySpectrum(gd.synth);
        ImGui::EndChild();

        ImGui::BeginChild("Finish", ImVec2(250, 250), false);
        
        char buffer[128]; 
        strncpy(buffer, NameTab.c_str(), sizeof(buffer)); 
        buffer[sizeof(buffer) - 1] = 0; 
        if (ImGui::InputText("Name", buffer, IM_ARRAYSIZE(buffer))) {
            NameTab = buffer; 
        }

        if(cev.currentPos == -1){
            if(ImGui::Button("Add New Sound")){
                sd.drawMain = true;
                sd.drawEditor = false;
                _CurrentSound.name = getUniqueName(gd.sounds , _CurrentSound.name);
                gd.sounds.emplace_back(_CurrentSound);
                _CurrentSound = {};
                cev.currentNote.reset();
            }
        }else{
            if(ImGui::Button("Finish Editing Sound")){
                sd.drawMain = true;
                sd.drawEditor = false;
                _CurrentSound.name = getUniqueName(gd.sounds , _CurrentSound.name , cev.currentPos);
                gd.sounds[cev.currentPos] = _CurrentSound;
                _CurrentSound = {};
                cev.currentNote.reset();
            }
        }
        ImGui::EndChild();

        ImGui::End();
        
}