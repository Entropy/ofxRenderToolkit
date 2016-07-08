#pragma once

#include "ofMain.h"

namespace ofxRenderToolkit
{
    namespace util
    {
        class Plane
        {
        public:
            Plane();
            Plane(const glm::vec3 & normalizedNormal, float distance);
            Plane(const glm::vec3 & normalizedNormal, const glm::vec3 & point);
            Plane(const glm::vec3 & point, const glm::vec3 & v1, const glm::vec3 & v2);
            Plane(const Plane & plane);

            Plane & operator=(const Plane & plane);

            glm::vec3 getNormal() const;

            void normalize();

            float signedDistance(const glm::vec3 & point) const;
            float signedDistanceXPlane(const glm::vec3 & point) const;
            float unsignedDistance(const glm::vec3 & point) const;
            float unsignedDistanceX(const glm::vec3 & point) const;

            glm::vec3 closestPoint(const glm::vec3 & point) const;
            
            glm::vec3 projectPoint(const glm::vec3 & point) const;
            glm::vec3 projectPointX(const glm::vec3 & point) const;
            glm::vec3 projectPointY(const glm::vec3 & point) const;

            bool intersectsSphere(const glm::vec3 & center, float radius, glm::vec3 * circleMidPoint, float * circleRadius) const;
            // Optimized form for a Y plane
            bool intersectsSphereY(const glm::vec3 & center, float radius, glm::vec3 * circleMidPoint, float * circleRadius) const;
            // Optimized form for a Z plane
            bool intersectsSphereZ(const glm::vec3 & center, float radius, glm::vec3 * circleMidPoint, float * circleRadius) const;

            void flip();
            void flipNormal();

        public:
            float a;
            float b;
            float c;
            float d;
        };
    }
}
