#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxRenderToolkit.h"

//#define USE_INSTANCED 1
//#define USE_FBO 1

class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();

    void createRandomLights();

    void animateLights();

    void setDebug(bool debug);

    void drawSkybox();
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
    ofxRTK::pbr::CubeMapTexture skyboxMap;

    ofxRTK::pbr::Material material;

    ofShader shader;
    ofShader skyboxShader;
    GLuint defaultVao;

    ofShader integrateDFGShader;
    ofFbo dfgFbo;

    float exposure;
    float gamma;

    ofEasyCam camera;
    ofEasyCam debugCamera;

    ofSpherePrimitive sphere;

#ifdef USE_INSTANCED
	ofVboMesh vboMesh;
	ofBufferObject bufferObject;
	ofTexture bufferTexture;
#endif

#ifdef USE_FBO
	ofFbo fbo;
#endif

    ofxImGui gui;
    bool mouseOverGui;
};
