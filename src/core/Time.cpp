#include "core/Time.h"
#include "glfw/glfw3.h"

// Time.cpp
float Time::delta_time = 0.0f;
double Time::last_time = 0.0f;

void Time::Update() {
    double current = glfwGetTime();
    delta_time = (float)(current - last_time);
    last_time = current;
}