#include "DrawFrame.h"

std::vector<sSound> readSounds(const std::string& filename){
    std::vector<sSound> sounds;
    std::ifstream inFile(filename);

    if (!inFile) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return sounds;
    }

    size_t soundCount; //how many sounds are there
    inFile >> soundCount;

    for (size_t i = 0; i < soundCount; ++i) {
        sSound sound(1);


        //read envelope
        inFile >> sound.env.AttackTime >> sound.env.DecayTime >> sound.env.SustainLevel >> sound.env.ReleaseTime
               >> sound.env.AttackShape >> sound.env.DecayShape >> sound.env.ReleaseShape;

        //read spectrum
        size_t partialCount;
        inFile >> partialCount;
        for(size_t j = 0; j < partialCount ; ++j){
            sPartial partial;
            inFile >> partial.frequency >> partial.Amplitude >> partial.oscillator;
            sound.spec.push_back(partial);
        }
        inFile.ignore();
        std::getline(inFile, sound.name);

        sounds.push_back(sound);
    }


    return sounds;

}

void saveSounds(const std::string& filename, const std::vector<sSound>& sounds){
    std::ofstream outFile(filename);

    if (!outFile) {
        std::cerr << "Failed to open file for writing: " << filename << "\n";
        return;
    }
    // Write the number of sounds
    outFile << sounds.size() << "\n\n";

    for (const auto& sound : sounds) {
        // Write env
        outFile << sound.env.AttackTime << " " << sound.env.DecayTime << " " << sound.env.SustainLevel << " "
                << sound.env.ReleaseTime << " " << sound.env.AttackShape << " " << sound.env.DecayShape << " "
                << sound.env.ReleaseShape << "\n";

        // Write partials
        outFile << sound.spec.size() << "\n";
        for (const auto& partial : sound.spec) {
            outFile << partial.frequency << " " << partial.Amplitude << " " << partial.oscillator << "\n";
        }

        outFile << sound.name << "\n\n";
    }

    outFile.close();
}


static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* SetupGLFWAndImGui(const char* windowTitle, int width, int height, const char* glsl_version) {
    if (!glfwInit()) {
        std::cout << "not initialised";
        std::cin.get();
        return nullptr; 
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create GLFW window
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
    if (window == nullptr) {
        return window; 
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 
    

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io; 
    ImGui::StyleColorsDark();
   

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    return window; 
}


void startframe(int& display_w , int& display_h, GLFWwindow* window){
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void finishframe(int& display_w , int& display_h, GLFWwindow* window){
    ImGui::Render();
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void cleanup(GLFWwindow* window , CurrentVariables& cev , StateData& sd , GlobalData& gd){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    gd.synth.pause();
    glfwDestroyWindow(window);
    glfwTerminate();
    if(sd.drawKeyBoard){
        cev.currentScale.Stop();
    }

}

std::string getUniqueName(std::vector<sSound>& vec, const std::string& name, int ignoreIndex) {
    auto isMatch = [&](const sSound& obj, int index) {
        return obj.name == name && index != ignoreIndex;
    };

    if (std::find_if(vec.begin(), vec.end(), [&](const sSound& obj) {
        return isMatch(obj, &obj - vec.data());
    }) == vec.end()) {
        return name;
    }

    int suffix = 1;
    while (true) {
        std::string newName = name + "(" + std::to_string(suffix) + ")";
        if (std::find_if(vec.begin(), vec.end(), [&](const sSound& obj) {
            return isMatch(obj, &obj - vec.data()) && obj.name == newName;
        }) == vec.end()) {
            return newName;
        }
        ++suffix;
    }
}

//Dork

void ShowVolumeBar(float volume) {   
    volume = (volume > 1.0f )? 1.0f : volume;
    ImGui::Text("Volume");
    ImGui::ProgressBar(volume, ImVec2(0.0f, 0.0f)); 
}

void ShowFrequencySpectrum(Synth& _synth){ 
    ImGui::PlotLines("##Frequency Spectrum", _synth.GetFrequencies().data(), 128, 0, nullptr, 0.0f, 0.15f, ImVec2(485, 300));
}

void ShowTemporalPlot(Note& _note){
    ImGui::PlotLines("##Carrier Wave", _note.TemporalVec.data(), 4096, 0, nullptr, -4.0f, 4.0f, ImVec2(485, 300));

}

void ShowAmplitudePlot(Note& _note){
    ImGui::PlotLines("##Carrier Wave", _note.AmplitudeVec.data(), 500, 0, nullptr, 0.0f , 1.5f, ImVec2(485, 300));

}
