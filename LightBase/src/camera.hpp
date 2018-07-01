#pragma once

#include <memory>

#include "glm/vec3.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace vinz {

class Camera
{
    private:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 worldUp;

        float speed;
        float yaw;
        float pitch;
        float sensitivity;
        
        void update();

    public:
        enum class Movement
        {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT
        };

        Camera()
        {
            position = glm::vec3(0.0f);
            worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

            speed = 0.15f;
            sensitivity = 0.05f;

            yaw = -90.0f;
            pitch = 0.0f;

            update();
        }

        Camera(const glm::vec3 &pos, const glm::vec3 &up, float yaw,
               float pitch, float speed = 0.15f, float sensitivity = 0.05f)
        {
            this->position = pos;
            this->worldUp = up;
            this->yaw = yaw;
            this->pitch = pitch;
            this->speed = speed;
            this->sensitivity = sensitivity;

            update();
        }

        Camera(const Camera &) = default;
        Camera(Camera &&) = default;
        ~Camera() {}
        Camera &operator=(Camera const &cam) = default;
        Camera &operator=(Camera &&cam) = default;

        void setPosition(const glm::vec3 &pos)
        {
            position = pos;
        }

        glm::mat4 view() const
        {
            return glm::lookAt(position, position + front, up);
        }

        void processKeys(Movement direction, float delta);
        void processMotion(float xoffset, float yoffset,
                           bool constraint = true);
};
};