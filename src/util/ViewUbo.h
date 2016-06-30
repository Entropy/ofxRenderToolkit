#pragma once

#include "ofMain.h"

namespace ofxRenderToolkit
{
    namespace util
    {
        class ViewUbo
        {
        public:
            ViewUbo();

            bool setup(GLuint binding);

            void configureShader(const ofShader & shader);

            void bind();
            void unbind();

            void update(const ofCamera & camera);

        private:
            struct Data
            {
            public:
                ofMatrix4x4 inverseViewMatrix;
                ofVec2f viewportSize;
                ofVec2f rcpViewportSize;
                float nearClip;
                float farClip;

            private:
                float padding[2];
            } data;

            ofBufferObject ubo;
            GLuint binding;
        };
    }
}