#ifndef _KCAMMAN
#define _KCAMMAN

#include "ofMain.h"
#include "msbOFCore.h"
#include "kCam.h"

class kCamManager {

public:
    // Constructor
    kCamManager();

    vector<kCam> roster;       // list of all cameras in scene

    int camKey;                 // increment the unique key for new cameras. Used to make "camera2" id for MSB
    int activeCam;
    int selectedCam;

    // methods
    void setup();
    void addCamera(int startFrame);
    void removeCamera();

    void updateSortOrder();     // runs a comparison function to easily identify the start times of previous and next shots in the sequence

    // getters
    int getNumCams();
    kCam getActiveCam();
    kCam getSelectedCam();

};

#endif
