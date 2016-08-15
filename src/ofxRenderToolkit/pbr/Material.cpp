#include "Material.h"

namespace ofxRenderToolkit
{
	namespace pbr
	{
		//--------------------------------------------------------------
		Material::Material()
			: updateFlags(0)
			, name("")
			, baseColor("Base Color", ofFloatColor::black)
			, metallic("Metallic", 0.0f, 0.0f, 1.0f)
			, roughness("Roughness", 0.0f, 0.0f, 1.0f)
			, emissiveColor("Emissive Color", ofFloatColor::black)
			, emissiveIntensity("Emissive Intensity", 0.0f, 0.0f, 1.0f)
			, texAlbedo(nullptr)
			, texNormal(nullptr)
			, texMetalness(nullptr)
			, texRoughness(nullptr)
		{
			this->parameters.setName("Material");
			this->parameters.add(baseColor, metallic, roughness, emissiveColor, emissiveIntensity);

			this->eventListeners.push_back(this->baseColor.newListener([this](ofFloatColor &)
			{
				this->updateFlags |= Material::UPDATE_BASE_COLOR;
			}));
			this->eventListeners.push_back(this->metallic.newListener([this](float &)
			{
				this->updateFlags |= Material::UPDATE_METALLIC;
			}));
			this->eventListeners.push_back(this->roughness.newListener([this](float &)
			{
				this->updateFlags |= Material::UPDATE_ROUGHNESS;
			}));
			this->eventListeners.push_back(this->emissiveColor.newListener([this](ofFloatColor &)
			{
				this->updateFlags |= Material::UPDATE_BASE_COLOR;
			}));
			this->eventListeners.push_back(this->emissiveIntensity.newListener([this](float &)
			{
				this->updateFlags |= Material::UPDATE_METALLIC;
			}));
		}

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