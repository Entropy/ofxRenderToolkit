#include "gli/gli.hpp"

#include "CubeMapTexture.h"
#include "ofxRenderToolkit/util/GLError.h"

namespace ofxRenderToolkit
{
    namespace pbr
    {
        //--------------------------------------------------------------
        CubeMapTexture::CubeMapTexture()
        {}

        //--------------------------------------------------------------
        CubeMapTexture::~CubeMapTexture()
        {}

        //--------------------------------------------------------------
        // Filename can be KTX or DDS files
        GLuint CubeMapTexture::createTexture(const std::string & path)
        {
            CheckGLError();

            gli::texture texture = gli::load(path);

            if (texture.empty())
            {
                return 0;
            }

            auto & translator = gli::gl(gli::gl::PROFILE_GL33);
            const auto format = translator.translate(texture.format(), texture.swizzles());
            ofLogNotice("CubeMapTexture::createTexture") << "Texture format is " << format.Internal;

            CheckGLError();

            GLenum target = translator.translate(texture.target());

            GLuint textureName = 0;
            glGenTextures(1, &textureName);
            glBindTexture(target, textureName);
            glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(texture.levels() - 1));
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, format.Swizzles[0]);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, format.Swizzles[1]);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, format.Swizzles[2]);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, format.Swizzles[3]);

            GLuint magFilter = texture.levels() == 0 ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR;
            GLuint minFilter = texture.levels() == 0 ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR;

            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);

            CheckGLError();

            const glm::tvec3<GLsizei> extent(texture.extent());

            if (texture.target() == gli::TARGET_CUBE)
            {
                glTexStorage2D(target, static_cast<GLint>(texture.levels()), format.Internal, extent.x, extent.y);
            }
            else
            {
                return 0;
            }

            CheckGLError();

            for (std::size_t layer = 0; layer < texture.layers(); ++layer)
            {
                for (std::size_t face = 0; face < texture.faces(); ++face)
                {
                    for (std::size_t level = 0; level < texture.levels(); ++level)
                    {
                        glm::tvec3<GLsizei> levelExtent(texture.extent(level));
                        target = static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face);

                        // Upload each face / mip.
                        glTexSubImage2D(
                            target, static_cast<GLint>(level),
                            0, 0,
                            levelExtent.x,
                            levelExtent.y,
                            format.External, format.Type,
                            texture.data(layer, face, level));

                        ofLogNotice("CubeMapTexture::createTexture") << "Layer = " << layer << ", Face = " << face << ", Level = " << level << ", Extent = (" <<
                            levelExtent.x << ", " << levelExtent.y << "), Format (" << format.External << ", " << format.Type << ")";
                    }
                }
            }

            CheckGLError();

            return textureName;
        }

        //--------------------------------------------------------------
        void CubeMapTexture::loadDDSTexture(const std::string & path)
        {
            this->texId = this->createTexture(ofToDataPath(path));
            ofLogNotice("CubeMapTexture::loadDDSTexture") << "Loaded texture with ID = " << this->texId;
        }

        //--------------------------------------------------------------
        void CubeMapTexture::bind(GLuint texUnit)
        {
            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->texId);
        }

        //--------------------------------------------------------------
        void CubeMapTexture::unbind(GLuint texUnit)
        {
            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
    }
}
