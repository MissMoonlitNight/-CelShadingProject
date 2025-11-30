#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include <iostream>

// для диалога
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#pragma comment(lib, "comdlg32.lib") 

// размеры экрана
const unsigned int w = 1024;
const unsigned int h = 768;

Camera cam(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f);

float lastX = w / 2.0f;
float lastY = h / 2.0f;
bool first_mouse = true;

// функция открытия файла
std::string GetFile() {
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Obj Files\0*.obj\0All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    }
    return "";
}

void resize_cb(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_cb(GLFWwindow* window, double xposIn, double yposIn) {
    float x = static_cast<float>(xposIn);
    float y = static_cast<float>(yposIn);

    if (first_mouse) {
        lastX = x;
        lastY = y;
        first_mouse = false;
    }

    float xoff = x - lastX;
    float yoff = y - lastY;

    lastX = x;
    lastY = y;

    // проверка нажатия колесика
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        cam.ProcessMouseMovement(xoff, yoff);
    }
}

void scroll_cb(GLFWwindow* window, double xoff, double yoff) {
    cam.ProcessMouseScroll(static_cast<float>(yoff));
}

int main() {
    std::cout << "Select file..." << std::endl;
    std::string path = GetFile();

    if (path == "") {
        std::cout << "No file!" << std::endl;
        return 0;
    }

    // фиксим путь
    for (int i = 0; i < path.length(); ++i) {
        if (path[i] == '\\') path[i] = '/';
    }
    std::cout << "File: " << path << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(w, h, "Project 1", NULL, NULL);
    if (win == NULL) {
        std::cout << "Error window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win);
    glfwSetFramebufferSizeCallback(win, resize_cb);
    glfwSetCursorPosCallback(win, mouse_cb);
    glfwSetScrollCallback(win, scroll_cb);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    glEnable(GL_DEPTH_TEST);

    Shader myShader("model.vert", "model.frag");
    Model mdl(path);

    glm::vec3 lghtPos(2.0f, 5.0f, 5.0f);

    while (!glfwWindowShouldClose(win)) {
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, true);

        // цвет фона
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myShader.use();

        // настройки света
        myShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        myShader.setVec3("objColor", glm::vec3(0.6f, 0.6f, 0.6f)); // серый
        myShader.setVec3("lightPos", lghtPos);
        myShader.setVec3("viewPos", cam.Position);

        // матрицы
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.0f);
        glm::mat4 view = cam.GetViewMatrix();
        myShader.setMat4("projection", proj);
        myShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f)); // вращение самой модели 
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        myShader.setMat4("model", model);

        mdl.Draw(myShader);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}