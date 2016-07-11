#include "Skybox.h"

namespace ofxRenderToolkit
{
	namespace pbr
	{
		//--------------------------------------------------------------
		Skybox::Skybox()
		{

		}

		//--------------------------------------------------------------
		bool Skybox::setup(util::ViewUbo & viewUbo)
		{
			glGenVertexArrays(1, &this->defaultVao);

			if (!this->shader.load("shaders/skybox"))
			{
				return false;
			}

			viewUbo.configureShader(this->shader);

			this->texture.loadDDSTexture("textures/skybox.dds");

			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

			return true;
		}

		//--------------------------------------------------------------
		void Skybox::bind(GLuint texUnit)
		{
			this->texture.bind(texUnit);
		}

		//--------------------------------------------------------------
		void Skybox::unbind(GLuint texUnit)
		{
			this->texture.unbind(texUnit);
		}

		//--------------------------------------------------------------
		void Skybox::draw(float exposure, float gamma)
		{
			auto wasCullFace = glIsEnabled(GL_CULL_FACE);
			auto wasDepthTest = glIsEnabled(GL_DEPTH_TEST);

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);

			this->shader.begin();
			this->shader.setUniform1f("uExposure", exposure);
			this->shader.setUniform1f("uGamma", gamma);
			this->shader.setUniform1i("uCubeMap", 3);
			{
				// Draw full-screen quad.
				glBindVertexArray(this->defaultVao);
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}
			this->shader.end();

			if (wasCullFace) glEnable(GL_CULL_FACE);
			if (wasDepthTest) glEnable(GL_DEPTH_TEST);
		}

		//--------------------------------------------------------------
		pbr::CubeMapTexture & Skybox::getTexture()
		{
			return this->texture;
		}
	}
}