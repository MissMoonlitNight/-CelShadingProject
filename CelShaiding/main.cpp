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
int w = 1024;
int h = 768;

float out_thick = 0.01f; // толщена обводки
Camera cam(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f);

bool rot_enable = false; // кручение объекта
float cur_angle = 0.0f; 

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
    ofn.lpstrFilter = "Obj Files\0*.obj\0All\0*.*\0"; // только obj
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    }
    return "";
}

//масштабирование экрана
void resize_cb(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    w = width;
    h = height;
}

//мышка
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

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        cam.ProcessMouseMovement(xoff, yoff);
    }
}

//колёсико мыши
void scroll_cb(GLFWwindow* window, double xoff, double yoff) {

    //изменение толщены обводки при зажатом ALT
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
    {
        out_thick += static_cast<float>(yoff) * 0.002f;
        if (out_thick < 0.0f) out_thick = 0.0f;
        // std::cout << "Thickness: " << out_thick << std::endl;
    }
    else
    {
        //обычный зум, если альт не нажат
        cam.ProcessMouseScroll(static_cast<float>(yoff));
    }
}

//загрузка нового файла
void UpdateModel(Model*& current_model_ptr) {
    std::string p = GetFile();

    if (p == "") return;

    // фиксим путь (виндовс слеши)
    for (int i = 0; i < p.length(); ++i) {
        if (p[i] == '\\') p[i] = '/';
    }
    //  удаление старой
    if (current_model_ptr != NULL) {
        delete current_model_ptr;
    }

    // загрузка новой 
    std::cout << "Loading: " << p << std::endl;
    current_model_ptr = new Model(p);
}


int main() {
    std::cout << "Select file..." << std::endl;
    std::string path = GetFile();

    if (path == "") {
        std::cout << "No file!" << std::endl;
        return 0;
    }

    for (int i = 0; i < path.length(); ++i) {
        if (path[i] == '\\') path[i] = '/';
    }
    std::cout << "File: " << path << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(w, h, "Project 1 Outline", NULL, NULL);
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

    // Основной шейдер
    Shader myShader("model.vert", "model.frag");
    // Шейдер для обводки
    Shader outShader("outline.vert", "outline.frag");

    Model* mdl = new Model(path);

    glm::vec3 lghtPos(2.0f, 5.0f, 5.0f);

    while (!glfwWindowShouldClose(win)) {
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, true);

        //Открытие новго файла при нажатии на O
        static bool o_pressed = false;
        if (glfwGetKey(win, GLFW_KEY_O) == GLFW_PRESS) {
            if (!o_pressed) {
                o_pressed = true;
                UpdateModel(mdl);
            }
        }
        else {
            o_pressed = false;
        }
        //Вкл.Выкл. кручения на R
        static bool r_pressed = false;
        if (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS) {
            if (!r_pressed) {
                rot_enable = !rot_enable; 
                r_pressed = true;
            }
        }
        else {
            r_pressed = false;
        }
        if (rot_enable) {
            cur_angle += 0.001f; // скорость вращения
        }

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.0f);
        glm::mat4 view = cam.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, cur_angle, glm::vec3(0.0f, 0.1f, 0.0f)); // вращение самой модели
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    
        myShader.use();
        myShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        myShader.setVec3("objColor", glm::vec3(0.6f, 0.6f, 0.6f));
        myShader.setVec3("lightPos", lghtPos);
        myShader.setVec3("viewPos", cam.Position);

        myShader.setMat4("projection", proj);
        myShader.setMat4("view", view);
        myShader.setMat4("model", model);

        mdl->Draw(myShader);

        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        //обводка
        outShader.use();
        outShader.setMat4("projection", proj);
        outShader.setMat4("view", view);
        outShader.setMat4("model", model);
        outShader.setFloat("outline_width", out_thick);

        mdl->Draw(outShader);

        glCullFace(GL_BACK); 
        glDisable(GL_CULL_FACE);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}