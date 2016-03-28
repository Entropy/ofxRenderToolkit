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

        void    setup(const ofCamera& _camera);
        void    configureShader(const ofShader& _shader) const;

        void    update(const ofCamera& _camera);
        void    updateUbo();

        void    setPointLightUboBinding(uint8_t _binding);
        void    setLightIndexTexUnit(uint8_t _texUnit);
        void    setLightPointerTexUnit(uint8_t _texUnit);

        void    setAmbientIntensity(float _intensity);
        void    addPointLight(const PointLight& _light);
        void    addDirectionalLight(const DirectionalLight& _light);

        void    begin();
        void    end();

        void    debugDrawCulledPointLights();
        void    debugDrawClusteredPointLights();
        void    debugDrawFrustum(const ofCamera& _camera);
        void    debugDrawOccupiedClusters(const ofCamera& _camera);

        inline uint8_t getPointLightUboBinding() const { return m_pointLightUboBinding; }
        inline uint8_t getLightIndexTexUnit() const { return m_lightIndexTexUnit; }
        inline uint8_t getLightPointerTexUnit() const { return m_lightPointerTexUnit; }

        inline uint16_t getNumVisibleLights() const { return m_clusterGrid.GetNumVisibleLights(); }
        inline uint32_t getNumPointLightIndices() const { return m_clusterGrid.GetNumPointLightIndices(); }
        inline uint32_t getNumCulledPointLights() const { return m_clusterGrid.GetNumCulledPointLights(); }
        inline uint32_t getNumAffectedClusters() const { return m_clusterGrid.GetNumAffectedClusters(); }

        inline std::vector<PointLight>& getPointLights() { return m_pointLights; };
        inline std::vector<DirectionalLight>& getDirectionalLights() { return m_directionalLights; };

        inline void clearPointLights() { m_pointLights.clear(); }
        inline void clearDirectionalLights() { m_directionalLights.clear(); }

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
