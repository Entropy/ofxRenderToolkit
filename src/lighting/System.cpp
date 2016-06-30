#include "System.h"
#include "glm/glm.hpp"

namespace ofxRenderToolkit
{
    namespace lighting
    {
        //--------------------------------------------------------------
        System::System()
            : binding(DEFAULT_LIGHT_UBO_BINDING)
            , lightIndexTexUnit(DEFAULT_LIGHT_INDEX_TEX_UNIT)
            , lightPointerTexUnit(DEFAULT_LIGHT_POINTER_TEX_UNIT)
        {}

        //--------------------------------------------------------------
        System::~System()
        {}

        //--------------------------------------------------------------
        bool System::setup(const ofCamera & camera)
        {
            ProjInfo projInfo;
            projInfo.fov = ofDegToRad(camera.getFov());
            projInfo.aspectRatio = camera.getAspectRatio();
            projInfo.nearZ = camera.getNearClip();
            projInfo.farZ = camera.getFarClip();

            // Clear lights.
            this->directionalLights.reserve(MAX_DIRECTIONAL_LIGHTS);
            this->pointLights.reserve(MAX_POINT_LIGHTS);

            this->directionalLights.clear();
            this->pointLights.clear();
            
            // Setup cluster grid.
            this->clusterGrid.setup(projInfo);
            this->clusterGridDebug.createClusterMesh(this->clusterGrid, projInfo);

            // Create lights ubo.
            this->ubo.allocate(sizeof(Data), GL_DYNAMIC_DRAW);
            if (!this->ubo.isAllocated())
            {
                return false;
            }

            // Build the primitive for debugging.
            this->debugSphere = ofSpherePrimitive(1.0f, 8);

            return true;
        }

        //--------------------------------------------------------------
        void System::configureShader(const ofShader & shader) const
        {
            shader.begin();
            {
                shader.bindUniformBlock(this->binding, "uLightBlock");
                shader.setUniform1i("uLightPointerTex", lightPointerTexUnit);
                shader.setUniform1i("uLightIndexTex", lightIndexTexUnit);
            }
            shader.end();
        }

        //--------------------------------------------------------------
        void System::setPointLightUboBinding(uint8_t binding)
        {
            this->binding = binding;
        }

        //--------------------------------------------------------------
        void System::setLightIndexTexUnit(uint8_t texUnit)
        {
            this->lightIndexTexUnit = texUnit;
        }

        //--------------------------------------------------------------
        void System::setLightPointerTexUnit(uint8_t texUnit)
        {
            this->lightPointerTexUnit = texUnit;
        }

        //--------------------------------------------------------------
        void System::setAmbientIntensity(float intensity)
        {
            this->ambientIntensity = intensity;
        }

        //--------------------------------------------------------------
        void System::addPointLight(const PointLight & light)
        {
            assert(this->pointLights.size() < MAX_POINT_LIGHTS);

            if (this->pointLights.size() < MAX_POINT_LIGHTS)
            {
                this->pointLights.push_back(light);
            }
        }

        //--------------------------------------------------------------
        void System::addDirectionalLight(const DirectionalLight & light)
        {
            assert(this->directionalLights.size() < MAX_DIRECTIONAL_LIGHTS);

            if (this->directionalLights.size() < MAX_DIRECTIONAL_LIGHTS)
            {
                this->directionalLights.push_back(light);
            }
        }

        //--------------------------------------------------------------
        void System::clearPointLights()
        {
            this->pointLights.clear();
        }

        //--------------------------------------------------------------
        void System::clearDirectionalLights()
        {
            this->directionalLights.clear();
        }

        //--------------------------------------------------------------
        uint8_t System::getLightUboBinding() const
        { 
            return this->binding;
        }
        
        //--------------------------------------------------------------
        uint8_t System::getLightIndexTexUnit() const
        {
            return this->lightIndexTexUnit; 
        }
        
        //--------------------------------------------------------------
        uint8_t System::getLightPointerTexUnit() const 
        {
            return this->lightPointerTexUnit; }

        //--------------------------------------------------------------
        uint16_t System::getNumVisibleLights() const 
        { 
            return this->clusterGrid.getNumVisibleLights(); 
        }
        
        //--------------------------------------------------------------
        uint32_t System::getNumPointLightIndices() const 
        { 
            return this->clusterGrid.getNumPointLightIndices(); 
        }
        
        //--------------------------------------------------------------
        uint32_t System::getNumCulledPointLights() const 
        { 
            return this->clusterGrid.getNumCulledPointLights(); 
        }
        
        //--------------------------------------------------------------
        uint32_t System::getNumAffectedClusters() const 
        { 
            return this->clusterGrid.getNumAffectedClusters(); 
        }

        //--------------------------------------------------------------
        std::vector<PointLight> & System::getPointLights() 
        { 
            return this->pointLights; 
        };
        
        //--------------------------------------------------------------
        std::vector<DirectionalLight> & System::getDirectionalLights() 
        { 
            return this->directionalLights; 
        };

        //--------------------------------------------------------------
        void System::update(const ofCamera & camera)
        {
            this->clusterGrid.cullPointLights(camera.getModelViewMatrix(), this->pointLights);
            this->clusterGrid.sortLightIndexList();
            this->clusterGrid.updateLightIndexTextures();

            // updateData() will use direct state access (DSA) on GL 4.5, faster than map?
            this->ubo.unbind(GL_UNIFORM_BUFFER);

            const auto numDirectionalLights = this->directionalLights.size();;

            auto ptr = this->ubo.map<uint8_t>(GL_WRITE_ONLY);
            memcpy(ptr + offsetof(Data, pointLights), this->pointLights.data(), sizeof(Data::pointLights));
            memcpy(ptr + offsetof(Data, directionalLights), this->directionalLights.data(), sizeof(Data::directionalLights));
            memcpy(ptr + offsetof(Data, directionalLightCount), &numDirectionalLights, sizeof(numDirectionalLights));
            memcpy(ptr + offsetof(Data, ambientIntensity), &ambientIntensity, sizeof(ambientIntensity));
            this->ubo.unmap();
        }

        //--------------------------------------------------------------
        void System::begin()
        {
            this->clusterGrid.bindLightIndexTextures(this->lightIndexTexUnit, this->lightPointerTexUnit);
            this->ubo.bindBase(GL_UNIFORM_BUFFER, this->binding);
        }

        //--------------------------------------------------------------
        void System::end()
        {
            this->clusterGrid.unbindLightIndexTextures(this->lightIndexTexUnit, this->lightPointerTexUnit);
            this->ubo.unbindBase(GL_UNIFORM_BUFFER, this->binding);
        }

        //--------------------------------------------------------------
        void System::debugDrawClusteredPointLights()
        {
            const auto numLightIndices = this->clusterGrid.getNumPointLightIndices();
            const auto * lightIndices = this->clusterGrid.getPointLightIndices();

            for (int idx = 0; idx < numLightIndices; ++idx)
            {
                ofPushMatrix();
                {
                    auto & light = this->pointLights[lightIndices[idx]];
                    ofSetColor(ofFloatColor(light.color.x, light.color.y, light.color.z, 1.0f));

                    ofTranslate(light.position);
                    ofScale(light.radius);
                    this->debugSphere.drawWireframe();
                }
                ofPopMatrix();
            }
        }

        //--------------------------------------------------------------
        void System::debugDrawCulledPointLights()
        {
            const auto numLightIndices = this->clusterGrid.getNumPointLightIndices();
            const auto * lightIndices = this->clusterGrid.getPointLightIndices();

            for (int idx = 0; idx < numLightIndices; ++idx)
            {
                ofPushMatrix();
                {
                    auto & light = this->pointLights[lightIndices[idx]];
                    ofSetColor(ofFloatColor(0.2f, 0.2f, 0.2f, 1.0f));

                    ofTranslate(light.position);
                    ofScale(light.radius);
                    this->debugSphere.drawWireframe();
                }
                ofPopMatrix();
            }
        }

        //--------------------------------------------------------------
        void System::debugDrawFrustum(const ofCamera & camera)
        {
            this->clusterGridDebug.drawFrustum(camera);
        }

        //--------------------------------------------------------------
        void System::debugDrawOccupiedClusters(const ofCamera & camera)
        {
            this->clusterGridDebug.drawOccupiedClusters(camera, this->clusterGrid);
        }
    }
}
