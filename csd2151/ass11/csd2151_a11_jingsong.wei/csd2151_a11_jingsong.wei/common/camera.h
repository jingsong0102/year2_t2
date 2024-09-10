/*!*****************************************************************************
\file camera.h
\author Vadim Surov (vsurov\@digipen.edu)
\par Course: CSD2151/CSD2150/CS250
\par Assignment: all
\date 02/18/2024 (MM/DD/YYYY)
\brief This file has definitions of the camera used in the framework 
       for scene definitions.
*******************************************************************************/
#pragma once

#include "program.h"

#include <glm/glm.hpp> // vec2, vec3, mat4, radians
#include <glm/gtc/matrix_transform.hpp>

namespace cg
{

    enum CameraType { ORBITING, WALKING };

    class Camera
    {
        CameraType cameraType;
        glm::vec3 position;
        glm::vec3 target;
        float fieldOfView;
        float near;
        float far;

        double oldx;
        double oldy;

    public:
        Camera(
            CameraType cameraType,
            glm::vec3 position = { 0.0f, 3.0f, 3.0f },
            glm::vec3 target = { 0.0f, 0.0f, 0.0f },
            float fieldOfView = 45.0f,
            float near = 0.5f,
            float far = 200.0f
        ) :
            cameraType{ cameraType },
            position{ position },
            target{ target },
            fieldOfView{ fieldOfView },
            near{ near },
            far{ far },
            oldx{ -1.0 },
            oldy{ -1.0 }
        { }

        Camera(
            glm::vec3 position = { 0.0f, 3.0f, 3.0f },
            float fieldOfView = 45.0f
        ) :
            cameraType{ CameraType::ORBITING },
            position{ position },
            target{ 0.0f, 0.0f, 0.0f },
            fieldOfView{ fieldOfView },
            near{ 0.5f },
            far{ 200.0f },
            oldx{ -1.0 },
            oldy{ -1.0 }
        { }

        void setType(CameraType cameraType)
        {
            this->cameraType = cameraType;
            if (cameraType == CameraType::ORBITING)
                target = { 0.0f, 0.0f, 0.0f };
        }

        void setUniforms(Program* pProgram) const
        {
            pProgram->use();
            pProgram->setUniform("camera.position", position);
            pProgram->disuse();
        }

        glm::mat4 getLookAt(glm::vec3 center = { 0.0f, 0.0f, 0.0f }, glm::vec3 up = { 0.0f, 1.0f, 0.0f }) const
        {
            return glm::lookAt(position, target, up);
        }

        glm::mat4 getPerspective(float aspect = 1) const
        {
            return glm::perspective(glm::radians(fieldOfView), aspect, near, far);
        }

        void onCursor(double xoffset, double yoffset, Program* pProgram = nullptr)
        {
            const float PI05 = glm::pi<float>() / 2.0f;
            
            if (cameraType == CameraType::ORBITING)
            {
                const float r = glm::sqrt(position.x * position.x +
                    position.y * position.y + position.z * position.z);
                float alpha = glm::asin(position.y / r);
                float betta = std::atan2f(position.x, position.z);

                if (yoffset < 0.0)
                    alpha += -0.02f;
                else if (yoffset > 0.0)
                    alpha += 0.02f;

                if (xoffset < 0.0)
                    betta += 0.05f;
                else if (xoffset > 0.0)
                    betta += -0.05f;

                if (alpha <= -PI05)
                    alpha = -PI05 + 0.01f;
                else if (alpha >= PI05)
                    alpha = PI05 - 0.01f;

                position.x = r * glm::cos(alpha) * glm::sin(betta);
                position.y = r * glm::sin(alpha);
                position.z = r * glm::cos(alpha) * glm::cos(betta);
            }
            else if (cameraType == CameraType::WALKING)
            {
                const float r = glm::sqrt(
                    (target.x - position.x) * (target.x - position.x) +
                    (target.y - position.y) * (target.y - position.y) + 
                    (target.z - position.z) * (target.z - position.z));
                float alpha = glm::asin((target.y - position.y) / r);
                float betta = std::atan2f((target.x - position.x), (target.z - position.z));

                
                if (yoffset < 0.0)
                    alpha += -0.02f;
                else if (yoffset > 0.0)
                    alpha += 0.02f;

                if (xoffset < 0.0)
                    betta += -0.05f;
                else if (xoffset > 0.0)
                    betta += 0.05f;

                if (alpha <= -PI05)
                    alpha = -PI05 + 0.01f;
                else if (alpha >= PI05)
                    alpha = PI05 - 0.01f;

                target.x = position.x + r * glm::cos(alpha) * glm::sin(betta);
                target.y = position.y + r * glm::sin(alpha);
                target.z = position.z + r * glm::cos(alpha) * glm::cos(betta);
            }

            setUniforms(pProgram);
        }

        void onScroll(double xoffset, double yoffset, Program* pProgram = nullptr)
        {
            if (cameraType == CameraType::ORBITING)
            {
                float r = glm::sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
                const float alpha = glm::asin(position.y / r);
                const float betta = std::atan2f(position.x, position.z);

                r += yoffset > 0.0f ? -1.0f : 1.0f;
                if (r < 1.0f) r = 1.0f;

                position.x = r * glm::cos(alpha) * glm::sin(betta);
                position.y = r * glm::sin(alpha);
                position.z = r * glm::cos(alpha) * glm::cos(betta);
            }
            else if (cameraType == CameraType::WALKING)
            {
                const glm::vec3 velocity = glm::normalize(yoffset > 0.0f ? target - position : position - target);
                position += velocity * glm::vec3(1.0f,0.0f,1.0f);
                target += velocity * glm::vec3(1.0f, 0.0f, 1.0f);
            }

            setUniforms(pProgram);
        }
    };

}