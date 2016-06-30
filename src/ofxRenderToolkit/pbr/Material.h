#pragma once

#include "ofMain.h"
#include "CubeMapTexture.h"

namespace ofxRenderToolkit
{
    namespace pbr
    {
        class Material
        {
        public:
            Material();

            void clearUpdateFlags();

            void setName(const std::string & name);

            void setBaseColor(const ofFloatColor & color);

            void setMetallic(float metallic);
            void setRoughness(float roughness);

            void setEmissiveColor(const ofFloatColor & color);
            void setEmissiveIntensity(float intensity);

            void setAlbedoMap(shared_ptr<ofTexture> tex);
            void setNormalMap(shared_ptr<ofTexture> tex);
            void setMetalnessMap(shared_ptr<ofTexture> tex);
            void setRoughnessMap(shared_ptr<ofTexture> tex);

            void setUniforms(const ofShader & shader);

        public:
            static const uint8_t UPDATE_BASE_COLOR = 1;
            static const uint8_t UPDATE_METALLIC = 1 << 1;
            static const uint8_t UPDATE_ROUGHNESS = 1 << 2;
            static const uint8_t UPDATE_ALBEDO_MAP = 1 << 3;
            static const uint8_t UPDATE_NORMAL_MAP = 1 << 4;
            static const uint8_t UPDATE_METALNESS_MAP = 1 << 5;
            static const uint8_t UPDATE_ROUGHNESS_MAP = 1 << 6;

        public:
            uint32_t updateFlags;

            std::string name;

            ofFloatColor baseColor;
            float metallic;
            float roughness;
            ofFloatColor emissiveColor;
            float emissiveIntensity;

            // TODO: Texture not implemented yet.
            ofPtr<ofTexture> texAlbedo;
            ofPtr<ofTexture> texNormal;
            ofPtr<ofTexture> texMetalness;
            ofPtr<ofTexture> texRoughness;
        };
    }
}
