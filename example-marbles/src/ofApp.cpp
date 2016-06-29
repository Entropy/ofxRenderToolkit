#include "ofApp.h"
#include "glm/glm.hpp"

using namespace glm;

//--------------------------------------------------------------
void ofApp::setup()
{
    m_gui.setup();

    // Set up cameras
    m_camera.setupPerspective( false, 60.0f, 0.1f, 2000.0f );
    m_debugCamera.setupPerspective( false, 60.0f, 1.0f, 10000.0f );

    ofLogNotice() << ofGetWindowWidth() << ", " << ofGetWindowHeight() << endl;

    m_camera.setAspectRatio( ofGetWindowWidth() / (float)ofGetWindowHeight() );
    m_debugCamera.setAspectRatio( ofGetWindowWidth() / (float)ofGetWindowHeight() );

    m_camera.setPosition( 0.0, 0.0, 600.0 );
    
    m_camera.setAutoDistance( false );
    m_camera.setDistance( 500 );

    ofLogNotice() << m_camera.getNearClip() << ", " << m_camera.getFarClip() << endl;

    // Load Shader
    m_shader.load( "shaders/main.vert", "shaders/main.frag" );
    m_shader.printActiveUniforms();
    m_shader.printActiveUniformBlocks();

    m_skyboxShader.load( "shaders/sky_box.vert", "shaders/sky_box.frag" );
    glGenVertexArrays( 1, &m_defaultVao );

    // Set up view ubo.
    const int viewUboBinding = 1;
    this->viewUbo.setup(viewUboBinding);
    this->viewUbo.configureShader(m_shader);
    this->viewUbo.configureShader(m_skyboxShader);

    // Set up lighting
    setupLighting();

    m_sphere = ofSpherePrimitive( 1.0f, 24 );

    m_material.setBaseColor( ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) );
    m_material.setMetallic( 0.0f );
    m_material.setRoughness( 0.0f );
    m_material.setEmissiveColor( ofFloatColor( 1.0f, 0.4f, 0.0f, 1.0f ) );
    m_material.setEmissiveIntensity( 0.0f );
   
    float aperture = 0.5f;
    float shutterSpeed = 1.0f / 60.0f;

    m_exposure = ofxRTK::CalcEVFromCameraSettings( aperture, shutterSpeed );
    m_gamma = 2.2f;

    m_skyboxMap.loadDDSTexture( "textures/output_skybox.dds" );
    m_irradianceMap.loadDDSTexture( "textures/output_iem.dds" );
    m_radianceMap.loadDDSTexture( "textures/output_pmrem.dds" );

    setAppMode( AppMode::NORMAL_VIEW );

    glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
}

void ofApp::setupLighting()
{  
    m_lightSystem.setup( m_camera );
    m_lightSystem.configureShader( m_shader );
    m_lightSystem.setAmbientIntensity( 0.5f );
}

void ofApp::createRandomLights()
{
    // create some random lights
    float positionDist = 330;
    float radius = 60.0f;

    int numPointLights = 60;

    m_lightSystem.clearPointLights();

    for ( int i = 0; i < numPointLights; ++i )
    {
        glm::vec3 offset = vec3( ofRandom( -positionDist, positionDist ), 0.0f, ofRandom( -positionDist, positionDist ) );
        ofxRTK::PointLight l( offset, vec3( 1.0f, 1.0f, 1.0f ), radius, 6000.0f );
        l.color = vec3( glm::normalize( vec3( ofRandom( 0.0f, 1.0f ), ofRandom( 0.0f, 1.0f ), ofRandom( 0.0f, 1.0f ) ) ) );
        m_lightSystem.addPointLight( l );
    }
}

void ofApp::animateLights()
{
    std::vector<ofxRTK::PointLight>& pointLights = m_lightSystem.getPointLights();
    for ( int idx = 0; idx < pointLights.size(); ++idx )
    {
        ofxRTK::PointLight& light = pointLights[ idx ];
        light.position.y = ( sinf( ( ofGetElapsedTimeMillis() + idx * 40 ) / 1400.0f ) * 0.5f + 0.5f ) * 100.0f; 
    }
}

void ofApp::setAppMode( const AppMode _mode )
{
    switch ( _mode )
    {
        case AppMode::NORMAL_VIEW:
        {
            m_camera.enableMouseInput();
            m_debugCamera.disableMouseInput();
        }
        break;

        case AppMode::DEBUG_VIEW:
        {
            m_camera.disableMouseInput();
            m_debugCamera.enableMouseInput();
        }
    }

    m_appMode = _mode;
}

void ofApp::imGui()
{
    m_gui.begin();
    {
        ImGui::Text( "Material Properties" );
        ImGui::ColorEdit4( "Base Color", (float*)&m_material.baseColor );
        ImGui::SliderFloat( "Emissive Intensity", &m_material.emissiveIntensity, 0.0f, 1.0f );
        ImGui::ColorEdit4( "Emissive Color", (float*)&m_material.emissiveColor );

        ImGui::Separator();
        ImGui::Text( "Camera Settings" );
        ImGui::SliderFloat( "Exposure", &m_exposure, 0.01f, 10.0f );
        ImGui::SliderFloat( "Gamma", &m_gamma, 0.01f, 10.0f );

        ImGui::BeginGroup();
        ImGui::Text( "Stats" );
        ImGui::Text( "Visible Lights: %u", m_lightSystem.getNumVisibleLights() );
        ImGui::Text( "Culled Lights: %u", m_lightSystem.getNumCulledPointLights() );
        ImGui::Text( "Num Affected Clusters: %u", m_lightSystem.getNumAffectedClusters() );
        ImGui::Text( "Num Light Indices: %u", m_lightSystem.getNumPointLightIndices() );
        ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate );
        ImGui::EndGroup();

        ImGui::Separator();
        ImGui::Text( "Lights" );

        ImGui::SliderFloat( "Ambient IBL Strength", &m_lightSystem.m_ambientIntensity, 0.0f, 3.0f );

        if ( ImGui::Button( "Create Point Lights" ) )
        {
            createRandomLights();
        }

        ImGui::SameLine();

        if ( ImGui::Button( "Clear Point Lights" ) )
        {
            m_lightSystem.clearPointLights();
        }


        if ( ImGui::Button( "Add Dir Light" ) )
        {
            m_lightSystem.clearDirectionalLights();

            ofxRTK::DirectionalLight dirLight1;
            dirLight1.color = glm::vec3( 1.0f, 1.0f, 1.0f );
            dirLight1.direction = glm::normalize( glm::vec3( 1.0f, -1.0f, 0.0f ) );
            dirLight1.intensity = 1.0f;

            m_lightSystem.addDirectionalLight( dirLight1 );
        }

        ImGui::SameLine();

        if ( ImGui::Button( "Clear Dir Light" ) )
        {
            m_lightSystem.clearDirectionalLights();
        }

        std::vector<ofxRTK::DirectionalLight>& dirLights = m_lightSystem.getDirectionalLights();
        if ( dirLights.size() != 0 )
        {
            ImGui::ColorEdit3( "Color", (float *)&dirLights.at(0).color );
            ImGui::SliderFloat3( "Direction", (float *)&dirLights.at(0).direction, -1.0f, 1.0f );
            ImGui::SliderFloat( "Intensity", &dirLights.at(0).intensity, 0.0f, 15.0f );
        }
    }
    m_gui.end();
}

void ofApp::drawSkybox()
{
    glDisable( GL_CULL_FACE );
    ofDisableDepthTest();

    m_skyboxShader.begin();
        m_skyboxShader.setUniform1f( "uExposure", m_exposure );
        m_skyboxShader.setUniform1f( "uGamma", m_gamma );
        m_skyboxShader.setUniform1i( "uCubeMap", 3 );

        // draw full-screen quad
        glBindVertexArray( m_defaultVao );
        glDrawArrays( GL_TRIANGLES, 0, 3 );
    m_skyboxShader.end();

    ofEnableDepthTest();
    glEnable( GL_CULL_FACE );
}

void ofApp::drawScene()
{
    drawSphereGrid();
 }

void ofApp::drawSphereGrid()
{
    glCullFace( GL_FRONT );

    int numSpheres = 8;

    float radius = 30.0f;
    float spacing = radius * 2.0f + 15.0f;
    float offset = -numSpheres * spacing * 0.5f;

    for ( int z = 0; z < numSpheres; ++z )
    {
        float zPercent = z / (float)( numSpheres - 1 );

        for ( int x = 0; x < numSpheres; ++x )
        {
            float xPercent = x / (float)( numSpheres - 1 );
            m_material.metallic = std::max( zPercent, 0.001f );
            m_material.roughness = std::max( xPercent * xPercent, 0.001f );
            m_material.setUniforms( m_shader );

            ofPushMatrix();
            ofTranslate( offset + x * spacing, radius * 2.0, offset + z * spacing );
            ofScale( radius );
            m_shader.setUniformMatrix3f( "normalMatrix", ofxRTK::GetNormalMatrix() );
            m_sphere.draw();
            ofPopMatrix();
        }
    }

    glCullFace( GL_BACK );
}

//--------------------------------------------------------------
void ofApp::update()
{
    animateLights();

    if ( m_bMouseOverGui ) 
    {
        m_camera.disableMouseInput();
    }
    else 
    {
        m_camera.enableMouseInput();
    }

    m_bMouseOverGui = false;
}

//--------------------------------------------------------------
void ofApp::draw()
{
    glEnable( GL_CULL_FACE );
    glCullFace( GL_FRONT );

    ofClear( ofFloatColor( 0.0f, 0.0f, 0.0f, 1.0f ) );
    ofDisableAlphaBlending();

    this->viewUbo.bind();
    {

        m_skyboxMap.bindTexture(14);
        m_irradianceMap.bindTexture(2);
        m_radianceMap.bindTexture(3);

        switch (m_appMode)
        {
        case AppMode::NORMAL_VIEW:
        {
            m_camera.begin();
            {
                this->viewUbo.update(m_camera);
                m_lightSystem.update(m_camera);

                ofSetColor(255, 255, 255, 255);

                drawSkybox();

                m_lightSystem.begin();
                m_shader.begin();
                m_material.setUniforms(m_shader);
                m_shader.setUniform1f("uExposure", m_exposure);
                m_shader.setUniform1f("uGamma", m_gamma);
                m_shader.setUniform1i("uIrradianceMap", 2);
                m_shader.setUniform1i("uRadianceMap", 3);
                drawScene();
                m_shader.end();
                m_lightSystem.end();
            }
            m_camera.end();

            imGui();
        }
        break;

        case AppMode::DEBUG_VIEW:
        {
            m_debugCamera.begin();
            {
                this->viewUbo.update(m_camera);
                m_lightSystem.update(m_camera);

                ofSetColor(255, 255, 255, 255);
                m_lightSystem.debugDrawFrustum(m_camera);

                m_lightSystem.debugDrawCulledPointLights();
                m_lightSystem.debugDrawClusteredPointLights();
                m_lightSystem.debugDrawOccupiedClusters(m_camera);
            }
            m_debugCamera.end();
        }
        break;
        }
    }
    this->viewUbo.unbind();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch ( key )
    {
        case '1':
            setAppMode( AppMode::NORMAL_VIEW );
        break;

        case '2':
            setAppMode( AppMode::DEBUG_VIEW );
        break;
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
