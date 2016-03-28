#pragma once

#include "ofMain.h"
#include "CubeMapTexture.h"

namespace ofxRenderToolkit
{
    class Material
    {
    public:
        static const uint8_t    UPDATE_BASE_COLOR = 1;
        static const uint8_t    UPDATE_METALLIC = 1 << 1;
        static const uint8_t    UPDATE_ROUGHNESS = 1 << 2;
        static const uint8_t    UPDATE_ALBEDO_MAP = 1 << 3;
        static const uint8_t    UPDATE_NORMAL_MAP = 1 << 4;
        static const uint8_t    UPDATE_METALNESS_MAP = 1 << 5;
        static const uint8_t    UPDATE_ROUGHNESS_MAP = 1 << 6;

        uint32_t          updateFlags;

        std::string       name;

        ofFloatColor	  baseColor;
        float       	  metallic;
        float       	  roughness;
        ofFloatColor	  emissiveColor;
        float        	  emissiveIntensity;

        // TODO: Texture not implemented yet.
        ofPtr<ofTexture> texAlbedo;
        ofPtr<ofTexture> texNormal;
        ofPtr<ofTexture> texMetalness;
        ofPtr<ofTexture> texRoughness;

        Material()
            : updateFlags(0)
            , name("")
            , baseColor(0.0f, 0.0f, 0.0f, 1.0f)
            , metallic(0.0f)
            , roughness(0.0f)
            , emissiveColor(0.0f, 0.0f, 0.0f, 1.0f)
            , emissiveIntensity(0.0f)
            , texAlbedo(nullptr)
            , texNormal(nullptr)
            , texMetalness(nullptr)
            , texRoughness(nullptr)
        {
        }

        void clearUpdateFlags()
        {
            updateFlags = 0;
        }

        void setName(const std::string& _name)
        {
            name = _name;
        }

        void setBaseColor(const ofFloatColor& _color)
        {
            baseColor = _color;
            updateFlags |= Material::UPDATE_BASE_COLOR;
        }

        void setMetallic(float _metallic)
        {
            metallic = std::min(std::max(_metallic, 0.001f), 1.0f);
            updateFlags |= Material::UPDATE_METALLIC;
        }

        void setRoughness(float _roughness)
        {
            roughness = std::min(std::max(_roughness, 0.001f), 1.0f);
            updateFlags |= Material::UPDATE_ROUGHNESS;
        }

        void setEmissiveColor(const ofFloatColor& _color)
        {
            emissiveColor = _color;
            updateFlags |= Material::UPDATE_BASE_COLOR;
        }

        void setEmissiveIntensity(float _intensity)
        {
            emissiveIntensity = std::min(std::max(_intensity, 0.0f), 1.0f);
            updateFlags |= Material::UPDATE_METALLIC;
        }

        void setAlbedoMap(ofPtr<ofTexture> _tex)
        {
            texAlbedo = _tex;
            updateFlags |= Material::UPDATE_ALBEDO_MAP;
        }

        void setNormalMap(ofPtr<ofTexture> _tex)
        {
            texNormal = _tex;
            updateFlags |= Material::UPDATE_NORMAL_MAP;
        }

        void setMetalnessMap(ofPtr<ofTexture> _tex)
        {
            texMetalness = _tex;
            updateFlags |= Material::UPDATE_METALNESS_MAP;
        }

        void setRoughnessMap(ofPtr<ofTexture> _tex)
        {
            texRoughness = _tex;
            updateFlags |= Material::UPDATE_ROUGHNESS_MAP;
        }

        void setUniforms(const ofShader& _shader)
        {
            _shader.setUniform4f("uBaseColor", baseColor);
            _shader.setUniform1f("uMetallic", metallic);
            _shader.setUniform1f("uRoughness", roughness);
            _shader.setUniform4f("uEmissiveColor", emissiveColor);
            _shader.setUniform1f("uEmissiveIntensity", emissiveIntensity);
        }
    };
}
