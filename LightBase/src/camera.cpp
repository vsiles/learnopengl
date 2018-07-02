#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include "camera.hpp"

using namespace vinz;

void Camera::update()
{
    glm::vec3 tfront;
    tfront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    tfront.y = sin(glm::radians(pitch));
    tfront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(tfront);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::processKeys(Camera::Movement direction, float delta)
{
    float velocity = speed * delta;
    switch(direction) {
        case Camera::Movement::FORWARD:
            position += velocity * front;
            break;
        case Camera::Movement::BACKWARD:
            position -= velocity * front;
            break;
        case Camera::Movement::LEFT:
            position -= velocity * right;
            break;
        case Camera::Movement::RIGHT:
            position += velocity * right;
            break;
        case Camera::Movement::UP:
            position += velocity * up;
            break;
        case Camera::Movement::DOWN:
            position -= velocity * up;
            break;
    }
}

void Camera::processMotion(float xoffset, float yoffset, bool constraint)
{
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constraint)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    update();
}
