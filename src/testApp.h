#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxOsc.h"
#include "ofxOpenCv.h"
#include "msbOFCore.h"

#include "kCam.h"
#include "kCamManager.h"
#include "ofxKinect.h"

#include "actor.h"



struct actorID;
struct memberID;

class testApp : public ofBaseApp, public Actor
{

	public:

        ofTrueTypeFont type;

		void setup();
        void msbSetup();
		void interfaceSetup();
		void filemappingSetup();

		void update();

        bool findFingerMatrix();
        bool findFinger();
        void findHands();

        int mapFrame();

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

        bool    bTwoHands;
        bool    bOneHand;

        bool    bFoundMat;          //one hand checks
        bool    bFingerOut;

        bool    bScrubingPlayhead;  // two hand checks
        bool    bScrubbingCamera;

        bool    bCameraSelected;    // maybe this should be a property of kCamManager
        bool    bSending;


        // bool bPalmOut

        // TODO: these should both be camera objects assigned after fired events
        // Assigned because the camera's start time preceded the playheadFrame
        int         selectedCamera; // This is actually the active camera, set in the kCamManagerClass.


        // PLAYHEAD STUFF
        int         oldActiveHandX;     // pixel value
        int         deltaHandX;         // pixel value

        int         totalNumFrames;     // frame value
        int         playheadFrame;      // frame value
        int         oldPlayheadFrame;   // to caluclate distanace from old to new and add or subtract that value

        int         mapPosToFrame(int pos);

        int         fistFactor;

        ofPoint     fingerStart,
                          fingerEnd,
                          one,
                          two,
                          three,
                          four,
                          five,
                          six;


        void    scrubPlayhead();

        kCamManager manager; // camera manager key

        // int camKey;

        // Hand Recognition Stuff


};

#endif
