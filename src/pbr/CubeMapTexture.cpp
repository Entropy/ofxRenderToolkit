#include "gli/gli.hpp"
#include "glm/glm.hpp"

#include "CubeMapTexture.h"
#include "util/GLError.h"

namespace ofxRenderToolkit
{
    CubeMapTexture::CubeMapTexture()
    {}

    CubeMapTexture::~CubeMapTexture()
    {}

    /// Filename can be KTX or DDS files
    GLuint CubeMapTexture::createTexture(const std::string& _path)
    {
        CheckGLError();

        gli::texture Texture = gli::load(_path);

        if (Texture.empty())
        {
            return 0;
        }

        gli::gl GL(gli::gl::PROFILE_GL33);
        gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
        ofLogNotice("ofxRender::CubeMapTexture::createTexture") << "Texture format is " << Format.Internal << endl;

        CheckGLError();

        GLenum Target = GL.translate(Texture.target());

        GLuint TextureName = 0;
        glGenTextures(1, &TextureName);
        glBindTexture(Target, TextureName);
        glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
        glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
        glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
        glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
        glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

        GLuint magFilter = Texture.levels() == 0 ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR;
        GLuint minFilter = Texture.levels() == 0 ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR;

        glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, minFilter);

        CheckGLError();

        glm::tvec3<GLsizei> const Extent(Texture.extent());
        GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

        if (Texture.target() == gli::TARGET_CUBE)
        {
            glTexStorage2D(Target, static_cast<GLint>(Texture.levels()), Format.Internal, Extent.x, Extent.y);
        }
        else
        {
            return 0;
        }

        CheckGLError();

        for (std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
        {
            for (std::size_t Face = 0; Face < Texture.faces(); ++Face)
            {
                for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
                {
                    GLsizei const LayerGL = static_cast<GLsizei>(Layer);
                    glm::tvec3<GLsizei> Extent(Texture.extent(Level));
                    Target = static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face);

                    // upload each face / mip
                    glTexSubImage2D(
                        Target, static_cast<GLint>(Level),
                        0, 0,
                        Extent.x,
                        Extent.y,
                        Format.External, Format.Type,
                        Texture.data(Layer, Face, Level));

                    ofLogNotice("ofxRender::CubeMapTexture::createTexture") << "Layer = " << Layer << ", Face = " << Face << ", Level = " << Level << ", Extent = (" <<
                        Extent.x << ", " << Extent.y << "), Format (" << Format.External << ", " << Format.Type << ")";
                }
            }
        }

        CheckGLError();

        return TextureName;
    }

    void CubeMapTexture::loadDDSTexture(const std::string& _path)
    {
        m_texId = createTexture(ofToDataPath(_path));
        ofLogNotice("ofxRender::CubeMapTexture::loadDDSTexture") << "Texture ID = " << m_texId;
    }

    void CubeMapTexture::bindTexture(GLuint _texUnit)
    {
        glActiveTexture(GL_TEXTURE0 + _texUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_texId);
    }
}
