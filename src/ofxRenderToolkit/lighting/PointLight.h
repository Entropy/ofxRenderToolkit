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
            PointLight(const glm::vec3 & position, const glm::vec3 & color, float radius, float intensity)
                : position(position, 1.0f)
                , color(color)
                , radius(radius)
                , intensity(intensity)
            {}

        public:
            glm::vec4 position;
            glm::vec3 color;
            float radius;
            float intensity;
            float padding[3];
        };
    }
}

