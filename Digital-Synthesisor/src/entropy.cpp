#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h> 
#include <iostream>
#include <cmath>
#include <algorithm>
#define PI 3.14159265358979323846

const int OCTAVE = 100;
const int LIMIT = 100000;
const int GRID_SIZE = 1080;
std::vector<float> grid(GRID_SIZE * GRID_SIZE, 0.0f);

double log2(double x) {
    return log(x) / log(2);
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void computeTriadicEntropy(int A, int B, int C, double s) {
    if (A * B * C >= LIMIT || gcd(gcd(A, B), C) != 1) return;

    double x = log2((double)B / A) * OCTAVE;
    double y = log2((double)C / B) * OCTAVE;
    int half_g_width = round(s * 4);
    int x_grid = round(x + half_g_width);
    int y_grid = round(y + half_g_width);

    // Check bounds before accessing the grid
    if (x_grid < 0 || x_grid >= GRID_SIZE || y_grid < 0 || y_grid >= GRID_SIZE) return;

    double w = 1.0 / pow(A * B * C, 1.0 / 3.0);
    int index = x_grid + y_grid * GRID_SIZE;
    grid[index] = std::max((float)w, grid[index]);

    std::vector<double> gauss_kernel(2 * half_g_width + 1);
    for (int i = -half_g_width; i <= half_g_width; ++i) {
        gauss_kernel[i + half_g_width] = exp(-0.5 * pow(i / s, 2)) / (s * sqrt(2 * PI));
    }

    for (int i = -half_g_width; i <= half_g_width; ++i) {
        for (int j = -half_g_width; j <= half_g_width; ++j) {
            int x_new = x_grid + i;
            int y_new = y_grid + j;

            // Check bounds before accessing the grid
            if (x_new >= 0 && x_new < GRID_SIZE && y_new >= 0 && y_new < GRID_SIZE) {
                int new_index = x_new + y_new * GRID_SIZE;
                grid[new_index] += (float)(w * gauss_kernel[i + half_g_width] * gauss_kernel[j + half_g_width]);
            }
        }
    }
}

// Map entropy value to a color (blue to red gradient)
ImU32 getColor(float value) {
    // Normalize the value to the range [0, 1]
    value = std::min(std::max(value, 0.0f), 50.0f)/50.0f;

    // Define a custom color gradient
    if (value < 0.25f) {
        // Transition from blue to cyan
        float t = value / 0.25f;
        return IM_COL32(0, (int)(t * 255), 255, 255);
    } else if (value < 0.5f) {
        // Transition from cyan to green
        float t = (value - 0.25f) / 0.25f;
        return IM_COL32(0, 255, (int)((1.0f - t) * 255), 255);
    } else if (value < 0.75f) {
        // Transition from green to yellow
        float t = (value - 0.5f) / 0.25f;
        return IM_COL32((int)(t * 255), 255, 0, 255);
    } else {
        // Transition from yellow to red
        float t = (value - 0.75f) / 0.25f;
        return IM_COL32(255, (int)((1.0f - t) * 255), 0, 255);
    }
}

void renderTriadicHarmonyVisual(ImDrawList* drawList, const ImVec2& origin, float scale) {
    // Draw the heatmap using ImGui's drawing functions
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            float value = grid[x + y * GRID_SIZE];
            if (value > 0.0f) {
                ImU32 color = getColor(value);
                ImVec2 pos = ImVec2(origin.x + x * scale, origin.y + y * scale);
                drawList->AddRectFilled(pos, ImVec2(pos.x + scale, pos.y + scale), color);
            }
        }
    }
}

void updateTriadicHarmony() {
    double s = 0.01; // Standard deviation
    grid.assign(GRID_SIZE * GRID_SIZE, 0.0f);

    for (int A = 1; A < 100; ++A) {
        for (int B = 1; B < 100; ++B) {
            for (int C = 1; C < 100; ++C) {
                computeTriadicEntropy(A, B, C, s);
            }
        }
    }
}

void renderImGui() {
    static bool initialized = false;
    if (!initialized) {
        updateTriadicHarmony();
        initialized = true;
    }
    ImVec2 origin = ImGui::GetCursorScreenPos();
    float scale = 1.0f; // Adjust scale as needed
    renderTriadicHarmonyVisual(ImGui::GetWindowDrawList(), origin, scale);

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


int main(){

    GLFWwindow* window = SetupGLFWAndImGui("Synth App TEST", 1920, 1080, "#version 130");
    int w , h;

    while(!glfwWindowShouldClose(window)){
        startframe(w, h, window);

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(w , h));       
        ImGui::Begin("Fullscreen Window", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        renderImGui();

        ImGui::End();

        finishframe(w, h, window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}