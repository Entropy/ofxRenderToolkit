#include "LightSystem.h"
#include "glm/glm.hpp"

namespace ofxRenderToolkit
{
    LightSystem::LightSystem()
        : m_pointLightUboBinding(skDefaultPointLightUboBinding)
        , m_lightIndexTexUnit(skDefaultLightIndexTexUnit)
        , m_lightPointerTexUnit(skDefaultLightPointerTexUnit)
    {

    }

    LightSystem::~LightSystem()
    {
    }

    void LightSystem::Init(const ofCamera& _camera)
    {
        ProjInfo projInfo;
        projInfo.fov = ofDegToRad(_camera.getFov());
        projInfo.aspectRatio = _camera.getAspectRatio();
        projInfo.nearZ = _camera.getNearClip();
        projInfo.farZ = _camera.getFarClip();

        m_directionalLights.reserve(skMaxDirectionalLights);
        m_pointLights.reserve(skMaxPointLights);

        m_clusterGrid.Init(projInfo);
        m_clusterGridDebug.CreateClusterMesh(m_clusterGrid, projInfo);

        // Create point light uniform buffer
        size_t numBytes = sizeof(LightUbo);
        ofLogNotice() << numBytes << endl;
        m_pointLightUbo.allocate(numBytes, nullptr, GL_DYNAMIC_DRAW);
        assert(true == m_pointLightUbo.isAllocated());

        m_debugSphere = ofSpherePrimitive(1.0f, 8);

        m_pointLights.clear();
    }

    void LightSystem::ConfigureShader(const ofShader& _shader) const
    {
        _shader.begin();
        _shader.bindUniformBlock(m_pointLightUboBinding, "LightBlock");
        _shader.setUniform1i("uLightPointerTex", m_lightPointerTexUnit);
        _shader.setUniform1i("uLightIndexTex", m_lightIndexTexUnit);
        _shader.end();
    }

    void LightSystem::SetPointLightUboBinding(uint8_t _binding)
    {
        m_pointLightUboBinding = _binding;
    }

    void LightSystem::SetLightIndexTexUnit(uint8_t _texUnit)
    {
        m_lightIndexTexUnit = _texUnit;
    }

    void LightSystem::SetLightPointerTexUnit(uint8_t _texUnit)
    {
        m_lightPointerTexUnit = _texUnit;
    }

    void LightSystem::SetAmbientIntensity(float _intensity)
    {
        m_ambientIntensity = _intensity;
    }

    void LightSystem::AddPointLight(const PointLight& _light)
    {
        assert(m_pointLights.size() < skMaxPointLights);

        if (m_pointLights.size() < skMaxPointLights)
        {
            m_pointLights.push_back(_light);
        }
    }

    void LightSystem::AddDirectionalLight(const DirectionalLight& _light)
    {
        assert(m_directionalLights.size() < skMaxDirectionalLights);

        if (m_directionalLights.size() < skMaxDirectionalLights)
        {
            m_directionalLights.push_back(_light);
        }
    }


    void LightSystem::Update(const ofCamera& _camera)
    {
        m_clusterGrid.CullPointLights(_camera.getModelViewMatrix(), m_pointLights);
        m_clusterGrid.SortLightIndexList();
        m_clusterGrid.UpdateLightIndexTextures();

        UpdateUbo();
    }

    void LightSystem::UpdateUbo()
    {
        // updateData() will use direct state access (DSA) on GL 4.5, faster than map?
        m_pointLightUbo.unbind(GL_UNIFORM_BUFFER);

        const int numDirectionalLights = m_directionalLights.size();;

        uint8_t * ptr = m_pointLightUbo.map<uint8_t>(GL_WRITE_ONLY);
        memcpy(ptr + offsetof(LightUbo, pointLights), m_pointLights.data(), sizeof(LightUbo::pointLights));
        memcpy(ptr + offsetof(LightUbo, directionalLights), m_directionalLights.data(), sizeof(LightUbo::directionalLights));
        memcpy(ptr + offsetof(LightUbo, directionalLightCount), &numDirectionalLights, sizeof(numDirectionalLights));
        memcpy(ptr + offsetof(LightUbo, ambientIntensity), &m_ambientIntensity, sizeof(m_ambientIntensity));

        m_pointLightUbo.unmap();
    }

    void LightSystem::Bind()
    {
        m_clusterGrid.BindLightIndexTextures(m_lightIndexTexUnit, m_lightPointerTexUnit);
        m_pointLightUbo.bindBase(GL_UNIFORM_BUFFER, m_pointLightUboBinding);
    }

    void LightSystem::DebugDrawClusteredPointLights()
    {
        const uint32_t numLightIndices = m_clusterGrid.GetNumPointLightIndices();
        const uint16_t * lightIndices = m_clusterGrid.GetPointLightIndices();

        for (int idx = 0; idx < numLightIndices; ++idx)
        {
            ofPushMatrix();
            {
                PointLight& light = m_pointLights[lightIndices[idx]];
                ofSetColor(ofFloatColor(light.color.x, light.color.y, light.color.z, 1.0f));

                ofTranslate(light.position.x, light.position.y, light.position.z);
                ofScale(light.radius);
                m_debugSphere.drawWireframe();
            }
            ofPopMatrix();
        }
    }

    void LightSystem::DebugDrawCulledPointLights()
    {
        const uint32_t numLightIndices = m_clusterGrid.GetNumCulledPointLights();
        const uint16_t * lightIndices = m_clusterGrid.GetCulledPointLightIndices();

        for (int idx = 0; idx < numLightIndices; ++idx)
        {
            ofPushMatrix();
            {
                PointLight& light = m_pointLights[lightIndices[idx]];
                ofSetColor(ofFloatColor(0.2f, 0.2f, 0.2f, 1.0f));

                ofTranslate(light.position.x, light.position.y, light.position.z);
                ofScale(light.radius);
                m_debugSphere.drawWireframe();
            }
            ofPopMatrix();
        }
    }

    void LightSystem::DebugDrawFrustum(const ofCamera& _camera)
    {
        m_clusterGridDebug.DrawFrustum(_camera);
    }

    void LightSystem::DebugDrawOccupiedClusters(const ofCamera& _camera)
    {
        m_clusterGridDebug.DrawOccupiedClusters(_camera, m_clusterGrid);
    }
}
