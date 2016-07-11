#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    this->gui.setup();

    // Set up cameras.
    this->camera.setupPerspective( false, 60.0f, 0.1f, 2000.0f );
    this->debugCamera.setupPerspective( false, 60.0f, 1.0f, 10000.0f );

    ofLogNotice("ofApp::setup") << "Window size: " << ofGetWindowWidth() << "x" << ofGetWindowHeight();

    this->camera.setAspectRatio( ofGetWindowWidth() / (float)ofGetWindowHeight() );
    this->debugCamera.setAspectRatio( ofGetWindowWidth() / (float)ofGetWindowHeight() );

    this->camera.setPosition( 0.0, 0.0, 600.0 );
    
    this->camera.setAutoDistance( false );
    this->camera.setDistance( 500 );

    ofLogNotice("ofApp::setup") << "Camera clip: " << this->camera.getNearClip() << ", " << this->camera.getFarClip();

    // Load shaders.
    this->shader.load("shaders/main.vert", "shaders/main.frag");
    this->shader.printActiveUniforms();
    this->shader.printActiveUniformBlocks();

    this->skyboxShader.load("shaders/sky_box.vert", "shaders/sky_box.frag");
    glGenVertexArrays( 1, &this->defaultVao );

    // Set up view UBO.
    const int viewUboBinding = 1;
    this->viewUbo.setup(viewUboBinding);
    this->viewUbo.configureShader(this->shader);
    this->viewUbo.configureShader(this->skyboxShader);

    // Set up lighting.
    this->lightingSystem.setup(this->camera);
    this->lightingSystem.configureShader(this->shader);
    this->lightingSystem.setAmbientIntensity(0.5f);

    // Set up PBR.
    this->material.setBaseColor(ofFloatColor(1.0f, 1.0f, 1.0f, 1.0f));
    this->material.setMetallic(0.0f);
    this->material.setRoughness(0.0f);
    this->material.setEmissiveColor(ofFloatColor(1.0f, 0.4f, 0.0f, 1.0f));
    this->material.setEmissiveIntensity(0.0f);

    float aperture = 0.5f;
    float shutterSpeed = 1.0f / 60.0f;

    this->exposure = ofxRTK::util::CalcEVFromCameraSettings(aperture, shutterSpeed);
    this->gamma = 2.2f;

    this->skyboxMap.loadDDSTexture("textures/output_skybox.dds");
    this->irradianceMap.loadDDSTexture("textures/output_iem.dds");
    this->radianceMap.loadDDSTexture("textures/output_pmrem.dds");

    this->sphere = ofSpherePrimitive(1.0f, 24);

    this->debug = false;
}

//--------------------------------------------------------------
void ofApp::createRandomLights()
{
    this->lightingSystem.clearPointLights();

    const auto positionDist = 330.0f;
    const auto radius = 60.0f;

    const auto numPointLights = 60;
    for (int i = 0; i < numPointLights; ++i)
    {
        auto & offset = ofVec3f(ofRandom(-positionDist, positionDist), 0.0f, ofRandom(-positionDist, positionDist));
        auto & light = ofxRTK::lighting::PointLight(offset, ofVec3f(1.0f, 1.0f, 1.0f), radius, 6000.0f);
        light.color = ofVec3f(ofRandom(0.0f, 1.0f), ofRandom(0.0f, 1.0f), ofRandom(0.0f, 1.0f)).getNormalized();
        this->lightingSystem.addPointLight(light);
    }
}

//--------------------------------------------------------------
void ofApp::animateLights()
{
    auto & pointLights = this->lightingSystem.getPointLights();
    for (int idx = 0; idx < pointLights.size(); ++idx)
    {
        auto & light = pointLights[idx];
        light.position.y = (sinf((ofGetElapsedTimeMillis() + idx * 40) / 1400.0f) * 0.5f + 0.5f) * 100.0f;
    }
}

//--------------------------------------------------------------
void ofApp::setDebug(bool debug)
{
    if (debug)
    {
        this->camera.disableMouseInput();
        this->debugCamera.enableMouseInput();
    }
    else
    {
        this->camera.enableMouseInput();
        this->debugCamera.disableMouseInput();
    }

    this->debug = debug;
}

//--------------------------------------------------------------
void ofApp::imGui()
{
    this->gui.begin();
    {
        ImGui::Text("Material Properties");
        ImGui::ColorEdit4("Base Color", (float *)&this->material.baseColor);
        ImGui::SliderFloat("Emissive Intensity", &this->material.emissiveIntensity, 0.0f, 1.0f);
        ImGui::ColorEdit4("Emissive Color", (float *)&this->material.emissiveColor);

        ImGui::Separator();
        ImGui::Text("Camera Settings");
        ImGui::SliderFloat("Exposure", &this->exposure, 0.01f, 10.0f);
        ImGui::SliderFloat("Gamma", &this->gamma, 0.01f, 10.0f);

        ImGui::BeginGroup();
        ImGui::Text("Stats");
        ImGui::Text("Visible Lights: %u", this->lightingSystem.getNumVisibleLights());
        ImGui::Text("Culled Lights: %u", this->lightingSystem.getNumCulledPointLights());
        ImGui::Text("Num Affected Clusters: %u", this->lightingSystem.getNumAffectedClusters());
        ImGui::Text("Num Light Indices: %u", this->lightingSystem.getNumPointLightIndices());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::EndGroup();

        ImGui::Separator();
        ImGui::Text("Lights");

        ImGui::SliderFloat("Ambient IBL Strength", &this->lightingSystem.ambientIntensity, 0.0f, 3.0f);

        if (ImGui::Button("Create Point Lights"))
        {
            createRandomLights();
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear Point Lights"))
        {
            this->lightingSystem.clearPointLights();
        }

        if (ImGui::Button("Add Dir Light"))
        {
            this->lightingSystem.clearDirectionalLights();
            auto & light = ofxRTK::lighting::DirectionalLight(ofVec3f(1.0f, -1.0f, 0.0f).getNormalized(), ofVec3f(1.0f, 1.0f, 1.0f), 1.0f);
            this->lightingSystem.addDirectionalLight(light);
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear Dir Light"))
        {
            this->lightingSystem.clearDirectionalLights();
        }

        auto & dirLights = this->lightingSystem.getDirectionalLights();
        if (!dirLights.empty())
        {
            auto & light = dirLights.front();
            ImGui::ColorEdit3("Color", (float *)&light.color);
            ImGui::SliderFloat3("Direction", (float *)&light.direction, -1.0f, 1.0f);
            ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 15.0f);
        }
    }
    this->gui.end();
}

//--------------------------------------------------------------
void ofApp::drawSkybox()
{
    glDisable(GL_CULL_FACE);
    ofDisableDepthTest();

    this->skyboxShader.begin();
    this->skyboxShader.setUniform1f("uExposure", this->exposure);
    this->skyboxShader.setUniform1f("uGamma", this->gamma);
    this->skyboxShader.setUniform1i("uCubeMap", 3);
    {
        // Draw full-screen quad.
        glBindVertexArray(this->defaultVao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    this->skyboxShader.end();

    ofEnableDepthTest();
    glEnable(GL_CULL_FACE);
}

//--------------------------------------------------------------
void ofApp::drawScene()
{
    glCullFace(GL_FRONT);

    int numSpheres = 8;

    float radius = 30.0f;
    float spacing = radius * 2.0f + 15.0f;
    float offset = -numSpheres * spacing * 0.5f;

    for (int z = 0; z < numSpheres; ++z)
    {
        float zPercent = z / (float)(numSpheres - 1);

        for (int x = 0; x < numSpheres; ++x)
        {
            float xPercent = x / (float)(numSpheres - 1);
            this->material.metallic = std::max(zPercent, 0.001f);
            this->material.roughness = std::max(xPercent * xPercent, 0.001f);
            this->material.setUniforms(this->shader);

            ofPushMatrix();
            {
                ofTranslate(offset + x * spacing, radius * 2.0, offset + z * spacing);
                ofScale(radius);
                this->shader.setUniformMatrix4f("uNormalMatrix", ofGetCurrentNormalMatrix());
                
                this->sphere.draw();
            }
            ofPopMatrix();
        }
    }

    glCullFace(GL_BACK);
}

//--------------------------------------------------------------
void ofApp::update()
{
    this->animateLights();

    if (this->mouseOverGui)
    {
        this->camera.disableMouseInput();
    }
    else
    {
        this->camera.enableMouseInput();
    }

    this->mouseOverGui = false;
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofClear(0.0f, 1.0f);
 
    ofDisableAlphaBlending();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    this->viewUbo.bind();
    {
        this->skyboxMap.bind(14);
        this->irradianceMap.bind(2);
        this->radianceMap.bind(3);

        if (this->debug)
        {
            this->debugCamera.begin();
            {
                this->viewUbo.update(this->camera);
                this->lightingSystem.update(this->camera);

                ofSetColor(255, 255, 255, 255);
                this->lightingSystem.debugDrawFrustum(this->camera);

                this->lightingSystem.debugDrawCulledPointLights();
                this->lightingSystem.debugDrawClusteredPointLights();
                this->lightingSystem.debugDrawOccupiedClusters(this->camera);
            }
            this->debugCamera.end();
        }
        else
        {
            this->camera.begin();
            {
                this->viewUbo.update(this->camera);
                this->lightingSystem.update(this->camera);

                ofSetColor(255, 255, 255, 255);

                this->drawSkybox();

                this->lightingSystem.begin();
                {
                    this->shader.begin();
                    {
                        this->material.setUniforms(this->shader);
                        this->shader.setUniform1f("uExposure", this->exposure);
                        this->shader.setUniform1f("uGamma", this->gamma);
                        this->shader.setUniform1i("uIrradianceMap", 2);
                        this->shader.setUniform1i("uRadianceMap", 3);
                    
                        this->drawScene();
                    }
                    this->shader.end();
                }
                this->lightingSystem.end();
            }
            this->camera.end();
        }

        this->skyboxMap.unbind(14);
        this->irradianceMap.unbind(2);
        this->radianceMap.unbind(3);
    }
    this->viewUbo.unbind();

    this->imGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == ' ')
    {
        this->debug ^= 1;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
