#ifndef _KCAMMAN
#define _KCAMMAN

#include "ofMain.h"
#include "msbOFCore.h"
#include "kCam.h"
#include "ofxOsc.h"

class kCamManager {

public:
    // Constructor
    kCamManager();

    vector<kCam> roster;       // list of all cameras in scene

    int camKey;                 // increment the unique key for new cameras. Used to make "camera2" id for MSB

    int activeCam;              // kCam.id
    int selectedCam;            // kCam.id

    // methods
    void setup();
    void addCamera(int startFrame);
    void removeCamera();        // removes the active camera

    void updateSortOrder();     // runs a comparison function to easily identify the start times of previous and next shots in the sequence

    bool bChangeActiveCam(int playhead);     // listen if the activeCam changes based on the playhead position

    // getters
    int getNumCams();

    int getActiveCamKey();
    kCam getActiveKCam();

    ofxOscSender ofSender;

    void updateActiveCamera(int playheadFrame);

};

#endif
