#include "DrawFrame.h"




void ShowMainWindow(StateData& sd , CurrentVariables& cev , GlobalData& gd){
            static int selectedItem = -1;
            static int hoveredItem = -1;
            sSound& _currentSound = cev.currentNote.Sound_details;

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(sd.display_w, sd.display_h));       
            ImGui::Begin("Fullscreen Window", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            
            {
                ImGui::BeginGroup();
                
                if(ImGui::Button("Create New Sound")){
                    selectedItem = -1;
                    cev.currentPos = -1;
                    sd.drawMain = false;
                    sd.drawEditor = true;
                    //gd.synth.pause();
                    _currentSound = sSound();
                    cev.currentNote.reset();
                } 
                
                if (ImGui::BeginListBox("##hidden", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
                    for (int idx = 0; idx < gd.sounds.size(); ++idx) {
                        const bool isSelected = (selectedItem == idx);
                        if (ImGui::Selectable((gd.sounds[idx].name).c_str() , isSelected)) {
                            selectedItem = (isSelected ? -1 : idx);
                            _currentSound = ((selectedItem == -1) ? sSound() : gd.sounds[selectedItem]);
                            cev.currentNote.reset();
                        }
                        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                            ImGui::OpenPopup(("ContextMenu" + std::to_string(idx)).c_str()); 
                            hoveredItem = idx; 
                            }
                        if (ImGui::BeginPopup(("ContextMenu" + std::to_string(idx)).c_str())) {
                            if (ImGui::Button("delete")) {
                                gd.sounds.erase(gd.sounds.begin() + hoveredItem);
                            }
                            if (ImGui::Button("clone")) {
                                sSound temp = gd.sounds[hoveredItem];
                                temp.name  = getUniqueName(gd.sounds, temp.name);
                                gd.sounds.emplace_back(temp);
                                }

                            if(ImGui::Button("edit")){
                                selectedItem = -1;
                                cev.currentPos = hoveredItem;
                                sd.drawMain = false;
                                sd.drawEditor = true;
                                //gd.synth.pause();
                                _currentSound = gd.sounds[hoveredItem];
                                cev.currentNote.reset();
                            }

                            ImGui::EndPopup();
                        
                        }
                        
                        
   
                        }
                    
                    ImGui::EndListBox();
                }
            
                
                ImGui::EndGroup();
            }
           
            {
                               
                ImGui::BeginGroup();
                if(selectedItem != -1){ 
                    
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
                    
                    ImGui::Text("%s", cev.currentNote.getInfo().c_str());

                    if(ImGui::Button("KeyBoard")){
                        gd.synth.pause();
                        sd.drawMain = false;
                        sd.drawKeyBoard = true;
                        cev.currentScale.ChangeNotes(_currentSound);
                        cev.currentScale.Start();
                        gd.synth.play(cev.currentScale.CurrentSound);
                    }
                }

                ImGui::EndGroup();


            }

            if(ImGui::Button("Save & Exit")){
                ImGui::Text("Saving... Please Wait");
                saveSounds("data/sounds.txt", gd.sounds);
                sd.appOn = false;
            }

            
            
            ImGui::End();
    }