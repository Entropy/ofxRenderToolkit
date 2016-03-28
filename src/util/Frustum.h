#pragma once
#include "glm/glm.hpp"
#include "Plane.h"

namespace ofxRenderToolkit
{
    bool SphereInFrustum(const Plane* _planes, const glm::vec3& _center, float _radius)
    {
        bool bOutside = false;
        bOutside |= _planes[0].signedDistance(_center) < -_radius;
        bOutside |= _planes[1].signedDistance(_center) > _radius; // TODO(James): Fix this
        bOutside |= _planes[2].signedDistance(_center) < -_radius;
        bOutside |= _planes[3].signedDistance(_center) < -_radius;
        bOutside |= _planes[4].signedDistance(_center) < -_radius;
        bOutside |= _planes[5].signedDistance(_center) < -_radius;

        return !bOutside;
    }

    bool BoxInFrustum(const Plane* _planes, const glm::vec3& _aabbMin, const glm::vec3& _aabbMax)
    {
        // check all corners of AABB against frustum planes
        for (int i = 0; i < 6; ++i)
        {
            int out = 0;
            const glm::vec3& normal = _planes[i].getNormal();
            out += (glm::dot(normal, glm::vec3(_aabbMin.x, _aabbMin.y, _aabbMin.z)) < 0.0f) ? 1 : 0;
            //console() << "i: " << i << " out: " << out << endl;
            out += (glm::dot(normal, glm::vec3(_aabbMax.x, _aabbMin.y, _aabbMin.z)) < 0.0f) ? 1 : 0;
            //console() << "i: " << i << " out: " << out << endl;
            out += (glm::dot(normal, glm::vec3(_aabbMin.x, _aabbMax.y, _aabbMin.z)) < 0.0f) ? 1 : 0;
            //console() << "i: " << i << " out: " << out << endl;
            out += (glm::dot(normal, glm::vec3(_aabbMax.x, _aabbMax.y, _aabbMin.z)) < 0.0f) ? 1 : 0;
            //console() << "i: " << i << " out: " << out << endl;
            out += (glm::dot(normal, glm::vec3(_aabbMin.x, _aabbMin.y, _aabbMax.z)) < 0.0f) ? 1 : 0;
            //console() << "i: " << i << " out: " << out << endl;
            out += (glm::dot(normal, glm::vec3(_aabbMax.x, _aabbMin.y, _aabbMax.z)) < 0.0f) ? 1 : 0;
            //console() << "i: " << i << " out: " << out << endl;
            out += (glm::dot(normal, glm::vec3(_aabbMin.x, _aabbMax.y, _aabbMax.z)) < 0.0f) ? 1 : 0;
            //console() << "i: " << i << " out: " << out << endl;
            out += (glm::dot(normal, glm::vec3(_aabbMax.x, _aabbMax.y, _aabbMax.z)) < 0.0f) ? 1 : 0;
            //console() << "i: " << i << " out: " << out << endl;

            if (out == 8)
            {
                return false;
            }
        }

        return true;
    }
}