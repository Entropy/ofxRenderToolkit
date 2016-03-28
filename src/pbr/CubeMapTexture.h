#pragma once

#include "ofMain.h"

namespace ofxRenderToolkit
{
    class CubeMapTexture
    {
    public:
        CubeMapTexture();
        ~CubeMapTexture();

        void loadDDSTexture(const std::string& _path);
        void bindTexture(GLuint _texUnit);

    private:
        GLuint createTexture(const std::string& _path);

        GLuint m_texId;
    };
}
