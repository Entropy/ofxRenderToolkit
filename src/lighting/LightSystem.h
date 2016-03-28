#pragma once

#include "PointLight.h"
#include "DirectionalLight.h"

#include "ClusterGrid.h"
#include "ClusterGridDebug.h"

namespace ofxRenderToolkit
{
    class LightSystem
    {
    private:
        static const int     skMaxPointLights = 1024;
        static const int     skMaxDirectionalLights = 4;

        static const uint8_t skDefaultPointLightUboBinding = 5;
        static const uint8_t skDefaultLightIndexTexUnit = 10;
        static const uint8_t skDefaultLightPointerTexUnit = 11;

        struct LightUbo
        {
            PointLight         pointLights[skMaxPointLights];
            DirectionalLight   directionalLights[skMaxDirectionalLights];
            float                  directionalLightCount;
            float                  ambientIntensity;

        private:
            float                  padding[2];
        };

    public:
        LightSystem();
        ~LightSystem();

        void    Init(const ofCamera& _camera);
        void    ConfigureShader(const ofShader& _shader) const;

        void    Update(const ofCamera& _camera);
        void    UpdateUbo();

        void    SetPointLightUboBinding(uint8_t _binding);
        void    SetLightIndexTexUnit(uint8_t _texUnit);
        void    SetLightPointerTexUnit(uint8_t _texUnit);

        void    SetAmbientIntensity(float _intensity);
        void    AddPointLight(const PointLight& _light);
        void    AddDirectionalLight(const DirectionalLight& _light);

        void    Bind();

        void    DebugDrawCulledPointLights();
        void    DebugDrawClusteredPointLights();
        void    DebugDrawFrustum(const ofCamera& _camera);
        void    DebugDrawOccupiedClusters(const ofCamera& _camera);

        inline uint8_t GetPointLightUboBinding() const { return m_pointLightUboBinding; }
        inline uint8_t GetLightIndexTexUnit() const { return m_lightIndexTexUnit; }
        inline uint8_t GetLightPointerTexUnit() const { return m_lightPointerTexUnit; }

        inline uint16_t GetNumVisibleLights() const { return m_clusterGrid.GetNumVisibleLights(); }
        inline uint32_t GetNumPointLightIndices() const { return m_clusterGrid.GetNumPointLightIndices(); }
        inline uint32_t GetNumCulledPointLights() const { return m_clusterGrid.GetNumCulledPointLights(); }
        inline uint32_t GetNumAffectedClusters() const { return m_clusterGrid.GetNumAffectedClusters(); }

        inline std::vector<PointLight>& GetPointLights() { return m_pointLights; };
        inline std::vector<DirectionalLight>& GetDirectionalLights() { return m_directionalLights; };

        float                  m_ambientIntensity;

    private:
        std::vector<PointLight>       m_pointLights;
        std::vector<DirectionalLight> m_directionalLights;

        ClusterGrid             m_clusterGrid;
        ClusterGridDebug        m_clusterGridDebug;

        ofBufferObject              m_pointLightUbo;
        uint8_t                     m_pointLightUboBinding;
        uint8_t                     m_lightIndexTexUnit;
        uint8_t                     m_lightPointerTexUnit;

        ofSpherePrimitive           m_debugSphere;
    };
}
