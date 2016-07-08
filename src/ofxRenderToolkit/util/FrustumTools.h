#pragma once

#include "ofMain.h"

#include "Plane.h"

namespace ofxRenderToolkit
{
    namespace util
    {
        //--------------------------------------------------------------
        inline bool SphereInFrustum(const Plane * planes, const glm::vec3 & center, float radius)
        {
            bool bOutside = false;
            bOutside |= planes[0].signedDistance(center) < -radius;
            bOutside |= planes[1].signedDistance(center) >  radius; // TODO(James): Fix this
            bOutside |= planes[2].signedDistance(center) < -radius;
            bOutside |= planes[3].signedDistance(center) < -radius;
            bOutside |= planes[4].signedDistance(center) < -radius;
            bOutside |= planes[5].signedDistance(center) < -radius;

            return !bOutside;
        }

        //--------------------------------------------------------------
        inline bool BoxInFrustum(const Plane * planes, const glm::vec3 & aabbMin, const glm::vec3 & aabbMax)
        {
            // Check all corners of AABB against frustum planes.
            for (int i = 0; i < 6; ++i)
            {
                int out = 0;
                const auto & normal = planes[i].getNormal();
                out += (glm::dot(normal, glm::vec3(aabbMin.x, aabbMin.y, aabbMin.z)) < 0.0f) ? 1 : 0;
                out += (glm::dot(normal, glm::vec3(aabbMax.x, aabbMin.y, aabbMin.z)) < 0.0f) ? 1 : 0;
                out += (glm::dot(normal, glm::vec3(aabbMin.x, aabbMax.y, aabbMin.z)) < 0.0f) ? 1 : 0;
                out += (glm::dot(normal, glm::vec3(aabbMax.x, aabbMax.y, aabbMin.z)) < 0.0f) ? 1 : 0;
                out += (glm::dot(normal, glm::vec3(aabbMin.x, aabbMin.y, aabbMax.z)) < 0.0f) ? 1 : 0;
                out += (glm::dot(normal, glm::vec3(aabbMax.x, aabbMin.y, aabbMax.z)) < 0.0f) ? 1 : 0;
                out += (glm::dot(normal, glm::vec3(aabbMin.x, aabbMax.y, aabbMax.z)) < 0.0f) ? 1 : 0;
                out += (glm::dot(normal, glm::vec3(aabbMax.x, aabbMax.y, aabbMax.z)) < 0.0f) ? 1 : 0;

                if (out == 8)
                {
                    return false;
                }
            }

            return true;
        }
    }
}