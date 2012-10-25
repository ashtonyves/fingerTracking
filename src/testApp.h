#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "kCam.h"
#include "ofxKinect.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"
#include "msbOFCore.h"
#include "actor.h"

struct actorID;
struct memberID;

class testApp : public ofBaseApp, public Actor
{

	public:

		void setup();

        void msbSetup();
		void interfaceSetup();
		void filemappingSetup();
		void cameraListSetup();

		void update();

        bool findFingerMatrix();
        bool findFinger();
        bool findHands();

		void sendData(string e);

        void draw();
		void exit();

        int shareMemory();

		void keyPressed  (int key);
		void keyReleased  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

        //msbSpecific
        void registerProperties();
        void trigger(Actor* other);

        ofxKinect           kinect;
        ofxOscSender        osc_senderMSB;
        ofxOscSender        osc_senderProcessing;

        string              ipAddressMSB;
        string              ipAddressProc;

        bool                bSetCutoffToZero;
        bool                bFullscreen;
        bool                bLearnBackground;

        //msbTools specific

        float*          myPic;

        Input*          input;
        Renderer*       renderer;
        Actor*          patchActor;

        float           cutOffDepth;
        float           thresh;

        ofxCvColorImage		    colorImg;
        ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;

        ofxCvContourFinder 	contourFinder;

        ofImage         myImage;
        unsigned char*  pixelData;

        Matrix4f        fingerTransformation;
        Vector3f        offsetVector;

        vector<Matrix4f> transformBuffer;

        int     bufferLength;

        int     playheadFrame;
        int     oldPlayheadFrame;   // to caluclate distanace from old to new and add or subtract that value

        bool    bFoundMat;
        bool    bFingerOut;
        bool    bTwoHands;

        bool    bCameraSelected;    // maybe this should be a property of Cam

        bool    bSending;
        bool    bScrubingPlayhead;
        // bool bPalmOut

        // TODO: these should both be camera objects assigned after fired events
        // Assigned because the camera's start time preceded the playheadFrame
        int     selectedCamera; // should be kCam
        kCam    activeCamera;

        int         fistFactor;

        ofPoint     fingerStart,
                          fingerEnd,
                          one,
                          two,
                          three,
                          four,
                          five,
                          six;


        void    scrubPlayhead(int pos);

        // list of all cameras in scene
        vector<kCam*> cameraList;

};

#endif
