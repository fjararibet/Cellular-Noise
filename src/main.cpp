#include <glad/glad.h>

#include "shader.hpp"
#include "shader_src.hpp"
#include "style.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int UI_WIDTH = 300;
const unsigned int GAME_WIDTH = 800;
const unsigned int SCR_WIDTH = GAME_WIDTH + UI_WIDTH;
const unsigned int SCR_HEIGHT = 800;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cellular Noise", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    enemyMouseStyle();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float vertices[] = {
        1.0f,  1.0f,  0.0f, // top right
        1.0f,  -1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, // bottom left
        -1.0f, 1.0f,  0.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    Shader shader(VERTEX_SHADER_SRC, SMOOTH_VORONOIT_FRAG_SRC);
    glViewport(0, 0, GAME_WIDTH, SCR_HEIGHT);
    float fadeStrength = 0.5f;
    float smoothness = 0.3;
    int fadeIn = 1;
    int scale = 10;
    float speed = 10.f;
    float colorFrequency = 2.5f;
    float colorPhase = 5.0f;
    float offsetR = 6.2831f;
    float offsetG = 4.1f;
    float offsetB = 3.7;
    float phaseSpread = 6.2831f;
    bool showIsoLines = false;
    float ColorEdit4 = 0.f;
    float isoLineThickness = 0.7f;
    float isoLineIntensity = 0.7f;
    float isoLineSpacing = 70.f;
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        int content_width = std::max(1, display_w - (int)UI_WIDTH);
        glViewport(0, 0, content_width, display_h);

        // Set UI on the right side
        ImGui::SetNextWindowPos(ImVec2(content_width, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(UI_WIDTH, display_h), ImGuiCond_Always);

        ImGui::Begin("Controls", nullptr,
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        shader.set2Float("u_resolution", (float)width, (float)height);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseY = height - mouseY;
        shader.set2Float("u_mouse", (float)mouseX, (float)mouseY);
        shader.setFloat("u_time", glfwGetTime());

        ImGui::SeparatorText("Visuals");
        ImGui::SliderFloat("Smoothness", &smoothness, 0.f, 1.f);
        shader.setFloat("smoothness", smoothness);
        ImGui::SliderInt("Scale", &scale, 0, 100);
        shader.setFloat("scale", (float)scale);

        ImGui::SeparatorText("Iso Lines");
        ImGui::Checkbox("Show", &showIsoLines);
        shader.setFloat("showIsoLines", showIsoLines);
        ImGui::SliderFloat("Thickness", &isoLineThickness, 0.0f, 1.0f);
        shader.setFloat("isoLineThickness", 1.0 - isoLineThickness);
        ImGui::SliderFloat("Intensity", &isoLineIntensity, 0.0f, 1.f);
        shader.setFloat("isoLineIntensity", isoLineIntensity);
        ImGui::SliderFloat("Spacing", &isoLineSpacing, 0.0f, 100.f);
        shader.setFloat("isoLineSpacing", 100.f - isoLineSpacing);

        ImGui::SeparatorText("Fade");
        ImGui::RadioButton("Fade In", &fadeIn, 0);
        ImGui::RadioButton("Fade out", &fadeIn, 1);
        shader.setBool("fadeIn", (bool)fadeIn);

        ImGui::SliderFloat("Strength", &fadeStrength, 0.f, 1.f);
        shader.setFloat("fadeStrength", fadeStrength);
        ImGui::Separator();

        ImGui::SeparatorText("Behavior");
        ImGui::SliderFloat("Speed", &speed, 0.f, 100.f);
        shader.setFloat("speed", speed / 5);
        ImGui::SliderFloat("Phase Spread", &phaseSpread, 0.f, 6.2831f);
        shader.setFloat("phaseSpread", phaseSpread);

        ImGui::SeparatorText("Color");
        ImGui::SliderFloat("Frequency", &colorFrequency, 0.f, 10.f);
        shader.setFloat("colorFrequency", colorFrequency);
        ImGui::SliderFloat("Phase", &colorPhase, 0.f, 6.2831f);
        shader.setFloat("colorPhase", colorPhase);
        ImGui::SliderFloat("Red Offset", &offsetR, 0.f, 6.2831f);
        shader.setFloat("offsetR", offsetR);
        ImGui::SliderFloat("Blue Offset", &offsetG, 0.f, 6.2831f);
        shader.setFloat("offsetG", offsetG);
        ImGui::SliderFloat("Green Offset", &offsetB, 0.f, 6.2831f);
        shader.setFloat("offsetB", offsetB);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    int viewport_width = width > UI_WIDTH ? (width - UI_WIDTH) : 1;
    glViewport(0, 0, viewport_width, height);
}
