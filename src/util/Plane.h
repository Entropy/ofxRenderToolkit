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
            Plane(const ofVec3f & normalizedNormal, float distance);
            Plane(const ofVec3f & normalizedNormal, const ofVec3f & point);
            Plane(const ofVec3f & point, const ofVec3f & v1, const ofVec3f & v2);
            Plane(const Plane & plane);

            Plane & operator=(const Plane & plane);

            ofVec3f getNormal() const;

            void normalize();

            float signedDistance(const ofVec3f & point) const;
            float signedDistanceXPlane(const ofVec3f & point) const;
            float unsignedDistance(const ofVec3f & point) const;
            float unsignedDistanceX(const ofVec3f & point) const;

            ofVec3f closestPoint(const ofVec3f & point) const;
            
            ofVec3f projectPoint(const ofVec3f & point) const;
            ofVec3f projectPointX(const ofVec3f & point) const;
            ofVec3f projectPointY(const ofVec3f & point) const;

            bool intersectsSphere(const ofVec3f & center, float radius, ofVec3f * circleMidPoint, float * circleRadius) const;
            // Optimized form for a Y plane
            bool intersectsSphereY(const ofVec3f & center, float radius, ofVec3f * circleMidPoint, float * circleRadius) const;
            // Optimized form for a Z plane
            bool intersectsSphereZ(const ofVec3f & center, float radius, ofVec3f * circleMidPoint, float * circleRadius) const;

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
