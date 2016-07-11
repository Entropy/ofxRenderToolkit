#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxRenderToolkit.h"

class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();

    void createRandomLights();

    void animateLights();

    void setDebug(bool debug);

    void drawScene();

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
    bool debug;

    ofxRTK::util::ViewUbo viewUbo;
    ofxRTK::lighting::System lightingSystem;

    ofxRTK::pbr::CubeMapTexture radianceMap;
    ofxRTK::pbr::CubeMapTexture irradianceMap;
    ofxRTK::pbr::Skybox skybox;
    ofxRTK::pbr::Material material;

    ofShader shader;

    ofShader integrateDFGShader;
    ofFbo dfgFbo;

    float exposure;
    float gamma;

    ofEasyCam camera;
    ofEasyCam debugCamera;

    ofSpherePrimitive sphere;

    ofxImGui gui;
    bool mouseOverGui;
};
