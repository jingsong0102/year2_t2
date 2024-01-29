#pragma once

#include "program.h"

#include <glm/glm.hpp> // vec2, vec3, mat4, radians
#include <glm/ext.hpp> // pi

#include <sstream>

namespace cg
{

    struct Light
    {
        glm::vec3 position;
        std::vector<glm::vec3> L;

        Light(
                glm::vec3 position = { 0.0f, 5.0f, 0.0f },
                std::initializer_list<glm::vec3> L = 
                {
                    { 0.4f, 0.4f, 0.4f },  // L or La
                    { 1.0f, 1.0f, 1.0f },  // Ld
                    { 1.0f, 1.0f, 1.0f }   // Ls
                }
            )
            : 
            position{ position }, 
            L{ L }
        { }

        void setUniforms(Program* pProgram, int i = 0) const
        {
            pProgram->use();

            std::stringstream s;

            s << "light[" << i << "].position";
            pProgram->setUniform(s.str(), position);

            if (L.size() > 0)
            {
                s.str("");
                s << "light[" << i << "].L";
                pProgram->setUniform(s.str(), L[0]);

                s.str("");
                s << "light[" << i << "].La";
                pProgram->setUniform(s.str(), L[0]);
            }

            if (L.size() > 1)
            {
                s.str("");
                s << "light[" << i << "].Ld";
                pProgram->setUniform(s.str(), L[1]);
            }

            if (L.size() > 2)
            {
                s.str("");
                s << "light[" << i << "].Ls";
                pProgram->setUniform(s.str(), L[2]);
            }

            pProgram->disuse();
        }

        void onCursor(double xoffset, double yoffset, Program* pProgram = nullptr, int i = 0)
        {
            const float PI05 = glm::pi<float>() / 2.0f;
            const float r = glm::sqrt(position.x * position.x +
                position.y * position.y + position.z * position.z);
            float alpha = glm::asin(position.y / r);
            float betta = std::atan2f(position.x, position.z);

            alpha += yoffset > 0.0 ? -0.05f : 0.05f;
            betta += xoffset < 0.0 ? -0.05f : 0.05f;

            if (alpha <= -PI05)
                alpha = -PI05 + 0.01f;
            else if (alpha >= PI05)
                alpha = PI05 - 0.01f;

            position.x = r * glm::cos(alpha) * glm::sin(betta);
            position.y = r * glm::sin(alpha);
            position.z = r * glm::cos(alpha) * glm::cos(betta);

            if (pProgram)
                setUniforms(pProgram, i);
        }

        void onScroll(double xoffset, double yoffset, Program* pProgram = nullptr, int i = 0)
        {
            float r = glm::sqrt(position.x * position.x +
                position.y * position.y +
                position.z * position.z);
            const float alpha = glm::asin(position.y / r);
            const float betta = std::atan2f(position.x, position.z);

            r += yoffset > 0.0 ? -1.0f : 1.0f;
            if (r < 1.0f) r = 1.0f;

            position.x = r * glm::cos(alpha) * glm::sin(betta);
            position.y = r * glm::sin(alpha);
            position.z = r * glm::cos(alpha) * glm::cos(betta);

            if (pProgram)
                setUniforms(pProgram, i);
        }
    };

}