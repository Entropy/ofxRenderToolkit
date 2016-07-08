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
            DirectionalLight(const glm::vec3 & direction, const glm::vec3 & color, float intensity)
                : direction(direction)
                , padding(0.0f)
                , color(color)
                , intensity(intensity)
            {}

        public:
            glm::vec3 direction;

        private:
            float padding; // .w = 0 for point lights

        public:
			glm::vec3 color;
            float  intensity;
        };
    }
}

