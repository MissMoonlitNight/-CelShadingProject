#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Camera {
public:
    // Что тут 
    glm::vec3 Target; // Точка, куда смотрим (центр)
    float Radius;     // Расстояние до объекта
    float Yaw;        // Угол поворота вокруг Y
    float Pitch;      // Угол наклона сверху/снизу

    glm::vec3 WorldUp;
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;

    // Настройки чувствительности
    float MouseSensitivity;
    float ZoomSensitivity;

    Camera(glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), float radius = 5.0f)
        : Target(target), Radius(radius), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
        Yaw(-90.0f), Pitch(0.0f), MouseSensitivity(0.2f), ZoomSensitivity(0.3f)
    {
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Target, Up);
    }

    // движения мыши
    void ProcessMouseMovement(float xoffset, float yoffset) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        // прям как блендере
        Yaw += xoffset;
        Pitch += yoffset;

        // перворот камеры через голову убрала)
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;

        updateCameraVectors();
    }

    // Обработка колесика мыши (Зум)
    void ProcessMouseScroll(float yoffset) {
        Radius -= yoffset * ZoomSensitivity;
        
        //std::cout << "Radius before" << Radius << std::endl;
        if (Radius <= 0.1f) Radius = 0.1f;
        //std::cout << "Radius after" << Radius <<std::endl;

        updateCameraVectors();
    }

private:
    // новые позиции камеры 
    void updateCameraVectors() {
        float camX = Target.x + Radius * cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
        float camY = Target.y + Radius * sin(glm::radians(Pitch));
        float camZ = Target.z + Radius * cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));

        Position = glm::vec3(camX, camY, camZ);

        Front = glm::normalize(Target - Position);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};