#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

enum class CAMERA_MOVEMENT {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV_zoom = 45.0f;

class Camera {
public:
    // camera attributes
    glm::vec3 position;
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;  // up vector in world space

    // Euler angles
    float yaw = YAW;
    float pitch = PITCH;

    // camera options
    float movementSpeed = SPEED;
    float mouseSensitivity = SENSITIVITY;
    float fov_zoom = FOV_zoom;

    // constructor with vectors
    Camera(glm::vec3 = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 = glm::vec3(0.0f, 1.0f, 0.0f), float = YAW, float = PITCH);
    // constructor with scalar values
    Camera(float, float, float, float, float, float, float, float);

    // return view matrix calculated using Euler angles and LookAt matrix
    glm::mat4 getViewMatrix();

    // process input received from any keyboard-like input system
    void processKeyboard(CAMERA_MOVEMENT, float);

    // process input received from a mouse input system
    void processMouseMovement(float, float, GLboolean = true);

    // process input received from a mouse scroll-wheel event
    void processMouseScroll(float);

    // process input to change camera position
    void processCameraPosition(float, float);
private:
    void updateCameraVectors();
};

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch) {
    this->position = position;
    this->worldUp = worldUp;
    this->yaw = yaw;
    this->pitch = pitch;
    this->updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) {
    this->position = glm::vec3(posX, posY, posZ);
    this->worldUp = glm::vec3(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;
    this->updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    front = glm::normalize(newFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(CAMERA_MOVEMENT POSITION, float deltaFrame) {
    float velocity = movementSpeed * deltaFrame;
    switch (POSITION) {
        case CAMERA_MOVEMENT::FORWARD:
            position += front * velocity;
            break;
        case CAMERA_MOVEMENT::BACKWARD:
            position -= front * velocity;
            break;
        case CAMERA_MOVEMENT::LEFT:
            position -= right * velocity;
            break;
        case CAMERA_MOVEMENT::RIGHT:
            position += right * velocity;
            break;
    }   
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    pitch += yoffset;
    yaw += xoffset;

    if (constrainPitch) {
        pitch = std::max(-89.0f, pitch);
        pitch = std::min( 89.0f, pitch);
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
    fov_zoom -= yoffset;
    fov_zoom = std::max(1.0f, fov_zoom);
    fov_zoom = std::min(60.0f, fov_zoom);
}

void Camera::processCameraPosition(float xoffset, float yoffset) {
    position.z -= xoffset * mouseSensitivity / 10;
    position.y += yoffset * mouseSensitivity / 10; // need smaller sensitivity
}

#endif // CAMERA_H