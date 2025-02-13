#pragma once

//macros
#define PI 3.14159265358979323846
#define SampleRate  44100.00
#define TimeIncrement 0.0000226757369615
#define ScaleFactor 1.05946309436
#define NOMINMAX


//APIs 
#include <Windows.h> 
#include <portaudio.h>

//Graphics
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h> 

//multithreading
#include <thread>
#include <atomic>
#include <condition_variable>

//containers
#include <string> 
#include <vector>
#include <array>
#include <unordered_map>
#include <cstring>

//maths
#include <algorithm> 
#include <cmath>
#include <fftw3.h>

//wrappers
#include <memory>
#include <functional>

//in and out
#include <iostream>
#include <stdio.h>
#include <fstream>
