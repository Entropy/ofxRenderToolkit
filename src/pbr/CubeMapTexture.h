#pragma once

#include "ofMain.h"

namespace ofxRenderToolkit
{
    namespace pbr
    {
        class CubeMapTexture
        {
        public:
            CubeMapTexture();
            ~CubeMapTexture();

            void loadDDSTexture(const std::string & path);

            void bind(GLuint texUnit);
            void unbind(GLuint texUnit);

        private:
            GLuint createTexture(const std::string & path);

            GLuint texId;
        };
    }
}
