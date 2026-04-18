#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main(){

	// OpenGL 3.2+ core profile — required for GLSL 1.50 shaders, ofShader, and ofCubeMap / HDRI.
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.setSize(1024, 768);
	settings.windowMode = OF_WINDOW; // can also be OF_FULLSCREEN

	auto window = ofCreateWindow(settings);

	ofRunApp(window, std::make_shared<ofApp>());
	ofRunMainLoop();
}
