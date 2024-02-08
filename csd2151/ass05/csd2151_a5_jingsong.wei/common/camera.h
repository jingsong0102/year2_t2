#pragma once

#include "program.h"

#include <glm/glm.hpp> // vec2, vec3, mat4, radians
#include <glm/gtc/matrix_transform.hpp>

namespace cg
{

    struct Camera
    {
        glm::vec3 position;
        float fieldOfView;
        float near;
        float far;

        double oldx;
        double oldy;

        Camera(
            glm::vec3 position = { 0.0f, 3.0f, 3.0f }, 
            float fieldOfView = 45.0f, 
            float near = 0.5f, 
            float far = 100.0f
            ): 
            position{ position }, 
            fieldOfView{ fieldOfView }, 
            near{ near }, 
            far{ far }, 
            oldx{ -1.0 }, 
            oldy{ }
        { }

        virtual ~Camera()
        { }

        virtual void setUniforms(Program* pProgram) const
        {
            pProgram->use();
            pProgram->setUniform("camera.position", position);
            pProgram->disuse();
        }

        void onCursor(double xoffset, double yoffset, Program* pProgram = nullptr)
        {
            const float PI05 = glm::pi<float>() / 2.0f;

            const float r = glm::sqrt(position.x * position.x +
                position.y * position.y + position.z * position.z);
            float alpha = glm::asin(position.y / r);
            float betta = std::atan2f(position.x, position.z);

            alpha += yoffset < 0.0 ? -0.05f : 0.05f;
            betta += xoffset > 0.0 ? -0.05f : 0.05f;

            if (alpha <= -PI05)
                alpha = -PI05 + 0.01f;
            else if (alpha >= PI05)
                alpha = PI05 - 0.01f;

            position.x = r * glm::cos(alpha) * glm::sin(betta);
            position.y = r * glm::sin(alpha);
            position.z = r * glm::cos(alpha) * glm::cos(betta);

            setUniforms(pProgram);
        }

        void onScroll(double xoffset, double yoffset, Program* pProgram = nullptr)
        {
            float r = glm::sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
            const float alpha = glm::asin(position.y / r);
            const float betta = std::atan2f(position.x, position.z);

            r += yoffset > 0.0f ? -1.0f : 1.0f;
            if (r < 1.0f) r = 1.0f;

            position.x = r * glm::cos(alpha) * glm::sin(betta);
            position.y = r * glm::sin(alpha);
            position.z = r * glm::cos(alpha) * glm::cos(betta);

            setUniforms(pProgram);
        }
    };
}