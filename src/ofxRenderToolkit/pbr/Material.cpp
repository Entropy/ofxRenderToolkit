#include "Material.h"

namespace ofxRenderToolkit
{
    namespace pbr
    {
        //--------------------------------------------------------------
        Material::Material()
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
        {}

        //--------------------------------------------------------------
        void Material::clearUpdateFlags()
        {
            this->updateFlags = 0;
        }

        //--------------------------------------------------------------
        void Material::setName(const std::string & name)
        {
            this->name = name;
        }

        //--------------------------------------------------------------
        void Material::setBaseColor(const ofFloatColor & color)
        {
            this->baseColor = color;
            this->updateFlags |= Material::UPDATE_BASE_COLOR;
        }

        //--------------------------------------------------------------
        void Material::setMetallic(float metallic)
        {
            this->metallic = std::min(std::max(metallic, 0.001f), 1.0f);
            this->updateFlags |= Material::UPDATE_METALLIC;
        }

        //--------------------------------------------------------------
        void Material::setRoughness(float roughness)
        {
            this->roughness = std::min(std::max(roughness, 0.001f), 1.0f);
            this->updateFlags |= Material::UPDATE_ROUGHNESS;
        }

        //--------------------------------------------------------------
        void Material::setEmissiveColor(const ofFloatColor & color)
        {
            this->emissiveColor = color;
            this->updateFlags |= Material::UPDATE_BASE_COLOR;
        }

        //--------------------------------------------------------------
        void Material::setEmissiveIntensity(float intensity)
        {
            this->emissiveIntensity = std::min(std::max(intensity, 0.0f), 1.0f);
            this->updateFlags |= Material::UPDATE_METALLIC;
        }

        //--------------------------------------------------------------
        void Material::setAlbedoMap(shared_ptr<ofTexture> tex)
        {
            this->texAlbedo = tex;
            this->updateFlags |= Material::UPDATE_ALBEDO_MAP;
        }

        //--------------------------------------------------------------
        void Material::setNormalMap(shared_ptr<ofTexture> tex)
        {
            this->texNormal = tex;
            this->updateFlags |= Material::UPDATE_NORMAL_MAP;
        }

        //--------------------------------------------------------------
        void Material::setMetalnessMap(shared_ptr<ofTexture> tex)
        {
            this->texMetalness = tex;
            this->updateFlags |= Material::UPDATE_METALNESS_MAP;
        }

        //--------------------------------------------------------------
        void Material::setRoughnessMap(shared_ptr<ofTexture> tex)
        {
            this->texRoughness = tex;
            this->updateFlags |= Material::UPDATE_ROUGHNESS_MAP;
        }

        //--------------------------------------------------------------
        void Material::setUniforms(const ofShader & shader)
        {
            shader.setUniform4f("uBaseColor", this->baseColor);
            shader.setUniform1f("uMetallic", this->metallic);
            shader.setUniform1f("uRoughness", this->roughness);
            shader.setUniform4f("uEmissiveColor", this->emissiveColor);
            shader.setUniform1f("uEmissiveIntensity", this->emissiveIntensity);
        }
    }
}