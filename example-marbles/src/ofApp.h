#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxRenderToolkit.h"

class ofApp : public ofBaseApp
{
    enum class AppMode
    {
        NORMAL_VIEW = 0,
        DEBUG_VIEW = 1
    };

public:
    void setup();
    void update();
    void draw();

    void setupLighting();

    void createRandomLights();

    void animateLights();

    void setAppMode(const AppMode _mode);

    void drawSkybox();
    void drawScene();
    void drawSphereGrid();

    void imGui();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

private:
    AppMode                     m_appMode;

    ofxRTK::util::ViewUbo       viewUbo;
    ofxRTK::LightSystem         m_lightSystem;

    ofxRTK::CubeMapTexture      m_radianceMap;
    ofxRTK::CubeMapTexture      m_irradianceMap;
    ofxRTK::CubeMapTexture      m_skyboxMap;

    ofxRTK::Material            m_material;

    ofShader                    m_shader;
    ofShader                    m_skyboxShader;
    GLuint                      m_defaultVao;

    ofShader                    m_integrateDFGShader;
    ofFbo                       m_dfgFbo;

    float                       m_exposure;
    float                       m_gamma;

    ofEasyCam                   m_camera;
    ofEasyCam                   m_debugCamera;

    ofSpherePrimitive           m_sphere;

    ofxImGui                    m_gui;
    bool                        m_bMouseOverGui;
};
