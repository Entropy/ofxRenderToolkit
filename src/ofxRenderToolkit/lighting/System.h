#pragma once

#include "PointLight.h"
#include "DirectionalLight.h"

#include "ClusterGrid.h"
#include "ClusterGridDebug.h"

namespace ofxRenderToolkit
{
    namespace lighting
    {
        class System
        {
        private:
            static const size_t MAX_POINT_LIGHTS = 1024;
            static const size_t MAX_DIRECTIONAL_LIGHTS = 4;

            static const uint8_t DEFAULT_LIGHT_UBO_BINDING = 5;
            static const uint8_t DEFAULT_LIGHT_INDEX_TEX_UNIT = 10;
            static const uint8_t DEFAULT_LIGHT_POINTER_TEX_UNIT = 11;

            struct Data
            {
            public:
                PointLight pointLights[MAX_POINT_LIGHTS];
                DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
                float directionalLightCount;
                float ambientIntensity;

            private:
                float padding[2];
            };

        public:
            System();
            ~System();

            bool setup(const ofCamera & camera);
            void configureShader(const ofShader & shader) const;

            void setPointLightUboBinding(uint8_t binding);
            void setLightIndexTexUnit(uint8_t texUnit);
            void setLightPointerTexUnit(uint8_t texUnit);

            void setAmbientIntensity(float intensity);

            void addPointLight(const PointLight & light);
            void addDirectionalLight(const DirectionalLight & light);

            void clearPointLights();
            void clearDirectionalLights();

            uint8_t getLightUboBinding() const;
            uint8_t getLightIndexTexUnit() const;
            uint8_t getLightPointerTexUnit() const;

            uint16_t getNumVisibleLights() const;
            uint32_t getNumPointLightIndices() const;
            uint32_t getNumCulledPointLights() const;
            uint32_t getNumAffectedClusters() const;

            std::vector<PointLight> & getPointLights();
            std::vector<DirectionalLight> & getDirectionalLights();

            void update(const ofCamera & camera);

            void begin();
            void end();

            void debugDrawCulledPointLights();
            void debugDrawClusteredPointLights();
            void debugDrawFrustum(const ofCamera& _camera);
            void debugDrawOccupiedClusters(const ofCamera& _camera);

        public:
            ofParameter<float> ambientIntensity;

			ofParameterGroup parameters;

        private:
            std::vector<PointLight> pointLights;
            std::vector<DirectionalLight> directionalLights;

            ClusterGrid clusterGrid;
            ClusterGridDebug clusterGridDebug;

            ofBufferObject ubo;
            uint8_t binding;
            uint8_t lightIndexTexUnit;
            uint8_t lightPointerTexUnit;

            ofSpherePrimitive debugSphere;
        };
    }
}
