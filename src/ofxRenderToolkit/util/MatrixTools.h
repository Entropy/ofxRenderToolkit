#pragma once

#include "glm/glm.hpp"
#include "ofMain.h"

namespace ofxRenderToolkit
{
    namespace util
    {
        //--------------------------------------------------------------
        inline void GetNearFarFromProjectionMatrix(const glm::mat4 & matrix, float * nearClip, float * farClip)
        {
            //   fNear = -mProj.r3.z / mProj.r2.z;
            //   fFar = mProj.r2.z / (mProj.r2.z-1) * fNear;

            float m31 = matrix[0][2];
            float m32 = matrix[1][2];
            float m33 = matrix[2][2];
            float m34 = matrix[3][2];

            float m41 = matrix[0][3];
            float m42 = matrix[1][3];
            float m43 = matrix[2][3];
            float m44 = matrix[3][3];

            glm::vec4 nearPlane;
            glm::vec4 farPlane;

            nearPlane.x = m41 + m31;
            nearPlane.y = m42 + m32;
            nearPlane.z = m43 + m33;
            nearPlane.w = m44 + m34;
            nearPlane = glm::normalize(nearPlane);

            farPlane.x = m41 - m31;
            farPlane.y = m42 - m32;
            farPlane.z = m43 - m33;
            farPlane.w = m44 - m34;
            farPlane = glm::normalize(farPlane);

            *nearClip = nearPlane.w / nearPlane.z;
            *farClip = farPlane.w / farPlane.z;
        }

        //--------------------------------------------------------------
        inline float GetFovFromProjMatrix(const glm::mat4 & mat4)
        {
            return 2.0f * atanf(1.0f / mat4[1][1]);
        }

        //--------------------------------------------------------------
        inline float GetHorizontalFov(float vertFovRad, float aspectRatio)
        {
            // Convert vertical FOV to clip, correct with aspect ratio and then back to angle.
            return 2.0f * atanf(tanf(vertFovRad * 0.5f) * aspectRatio);
        }

        //--------------------------------------------------------------
        inline float GetFocalLength(float fovRad)
        {
            return  1.0f / tanf(fovRad * 0.5f);
        }

        //--------------------------------------------------------------
        inline glm::mat3 Mat4ToMat3(const glm::mat4 & mat4)
        {
            return glm::mat3(
                mat4[0][0], mat4[0][1], mat4[0][2],
                mat4[1][0], mat4[1][1], mat4[1][2],
                mat4[2][0], mat4[2][1], mat4[2][2]);
        }
    }
}
