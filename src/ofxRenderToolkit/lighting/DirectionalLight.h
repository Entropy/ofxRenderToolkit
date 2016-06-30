#pragma once

#include "ofMain.h"

namespace ofxRenderToolkit
{
    namespace lighting
    {
        struct DirectionalLight
        {
        public:
            //--------------------------------------------------------------
            DirectionalLight()
                : direction(1.0f, 0.0f, 0.0f)
                , padding(0.0f)
                , color(1.0f, 1.0f, 1.0f)
                , intensity(1000.0f)
            {};

            //--------------------------------------------------------------
            DirectionalLight(const ofVec3f & direction, const ofVec3f & color, float intensity)
                : direction(direction)
                , padding(0.0f)
                , color(color)
                , intensity(intensity)
            {}

        public:
            ofVec3f direction;

        private:
            float padding; // .w = 0 for point lights

        public:
            ofVec3f color;
            float  intensity;
        };
    }
}

