#pragma once

#include "ofMain.h"

#include "ofxRenderToolkit/pbr/CubeMapTexture.h"
#include "ofxRenderToolkit/util/ViewUbo.h"

namespace ofxRenderToolkit
{
	namespace pbr
	{
		class Skybox
		{
		public:
			Skybox();

			bool setup(util::ViewUbo & viewUbo);

			void bind(GLuint texUnit);
			void unbind(GLuint texUnit);

			void draw(float exposure = 1.0f, float gamma = 2.2f);

			pbr::CubeMapTexture & getTexture();

		protected:
			pbr::CubeMapTexture texture;

			ofShader shader;
			GLuint defaultVao;
		};
	}
}