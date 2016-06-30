#pragma once

#include "ofMain.h"

#include "Plane.h"

namespace ofxRenderToolkit
{
    namespace util
    {
        //--------------------------------------------------------------
        bool SphereInFrustum(const Plane * planes, const ofVec3f & center, float radius)
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
        bool BoxInFrustum(const Plane * planes, const ofVec3f & aabbMin, const ofVec3f & aabbMax)
        {
            // Check all corners of AABB against frustum planes.
            for (int i = 0; i < 6; ++i)
            {
                int out = 0;
                const auto & normal = planes[i].getNormal();
                out += (normal.dot(ofVec3f(aabbMin.x, aabbMin.y, aabbMin.z)) < 0.0f) ? 1 : 0;
                out += (normal.dot(ofVec3f(aabbMax.x, aabbMin.y, aabbMin.z)) < 0.0f) ? 1 : 0;
                out += (normal.dot(ofVec3f(aabbMin.x, aabbMax.y, aabbMin.z)) < 0.0f) ? 1 : 0;
                out += (normal.dot(ofVec3f(aabbMax.x, aabbMax.y, aabbMin.z)) < 0.0f) ? 1 : 0;
                out += (normal.dot(ofVec3f(aabbMin.x, aabbMin.y, aabbMax.z)) < 0.0f) ? 1 : 0;
                out += (normal.dot(ofVec3f(aabbMax.x, aabbMin.y, aabbMax.z)) < 0.0f) ? 1 : 0;
                out += (normal.dot(ofVec3f(aabbMin.x, aabbMax.y, aabbMax.z)) < 0.0f) ? 1 : 0;
                out += (normal.dot(ofVec3f(aabbMax.x, aabbMax.y, aabbMax.z)) < 0.0f) ? 1 : 0;

                if (out == 8)
                {
                    return false;
                }
            }

            return true;
        }
    }
}