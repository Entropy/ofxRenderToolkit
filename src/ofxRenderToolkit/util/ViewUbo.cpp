#include "ViewUbo.h"

namespace ofxRenderToolkit
{
    namespace util
    {
        //--------------------------------------------------------------
        ViewUbo::ViewUbo()
            : binding(0)
        {}

        //--------------------------------------------------------------
        bool ViewUbo::setup(GLuint binding)
        {
            this->binding = binding;
            this->ubo.allocate(sizeof(Data), GL_DYNAMIC_DRAW);
            return this->ubo.isAllocated();
        }

        //--------------------------------------------------------------
        void ViewUbo::configureShader(const ofShader & shader)
        {
            shader.begin();
            {
                shader.bindUniformBlock(this->binding, "uViewBlock");
            }
            shader.end();
        }

        //--------------------------------------------------------------
        void ViewUbo::bind()
        {
            this->ubo.bindBase(GL_UNIFORM_BUFFER, this->binding);
        }

        //--------------------------------------------------------------
        void ViewUbo::unbind()
        {
            this->ubo.unbindBase(GL_UNIFORM_BUFFER, this->binding);
        }

        //--------------------------------------------------------------
        void ViewUbo::update(const ofCamera & camera)
        {
            const auto bounds = ofGetCurrentViewport();
            this->data.viewportSize = glm::vec2(bounds.width, bounds.height);
            this->data.rcpViewportSize = 1.0f / this->data.viewportSize;
            this->data.nearClip = camera.getNearClip();
            this->data.farClip = camera.getFarClip();
			this->data.viewMatrix = camera.getModelViewMatrix();
            this->data.inverseViewMatrix = glm::inverse(this->data.viewMatrix);

            this->ubo.updateData(sizeof(Data), &this->data);
        }
    }
}