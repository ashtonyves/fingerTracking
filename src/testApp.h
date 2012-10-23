#ifndef _TEST_APP
#define _TEST_APP




#include "ofMain.h"

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
        int                     channelMSB;
        int                     channelProc;


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

        ofxCvColorImage		colorImg;

        ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;

        ofxCvContourFinder 	contourFinder;

        ofImage         myImage;
        unsigned char*           pixelData;

        Matrix4f        fingerTransformation;
        Vector3f        offsetVector;

        vector<Matrix4f> transformBuffer;

        int     bufferLength;

        // TODO: these should both be camera objects
        int     selectedCamera;
        int     activeCamera;


        bool    bFoundMat;

        bool    bFingerOut;
        bool    bTwoHands;
        bool    bCameraSelected;    // maybe this should be a property of Cam

        bool    bSending;
        // bool bPalmOut

        int         fistFactor;

        ofPoint     fingerStart,
                          fingerEnd,
                          one,
                          two,
                          three,
                          four,
                          five,
                          six;

        bool    bScrubingPlayhead;
        int     playheadFrame;
        void    sendPlayheadData();
        int     oldX;

};

#endif
