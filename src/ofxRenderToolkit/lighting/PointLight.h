#pragma once

#include "ofMain.h"

namespace ofxRenderToolkit
{
    namespace lighting
    {
        struct PointLight
        {
        public:
            //--------------------------------------------------------------
            PointLight()
                : position(0.0f, 0.0f, 0.0f, 1.0f)
                , color(1.0f, 1.0f, 1.0f)
                , radius(1.0f)
                , intensity(500.0f)
            {}

            //--------------------------------------------------------------
            PointLight(const ofVec3f & position, const ofVec3f & color, float radius, float intensity)
                : position(position.x, position.y, position.z, 1.0f)
                , color(position)
                , radius(radius)
                , intensity(intensity)
            {}

        public:
            ofVec4f position;
            ofVec3f color;
            float radius;
            float intensity;
            float padding[3];
        };
    }
}

