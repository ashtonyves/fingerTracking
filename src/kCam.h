#ifndef _KCAM
#define _KCAM

#include "ofMain.h"
#include "msbOFCore.h"

class kCam {

public:
    // Constructor
    kCam();

     // properties
    int id;             // id of the camera
    int startFrame;     // starting frame, corresponding to x position of blob
    Matrix4f transform; // transform matrix

    bool visible;       // whether to show them in MSB or not

    // maybe these are best indicated in the main class, rather than declaring each one active or selected...
    bool active;        // is this camera active?
    bool selected;      // is this camera selected


    // methods
    void setCameraPosition();           // change the transform matrix - refactor from current findFingerMatrix
    void setCameraStartTime();          // drop cam and adjust start time
    void removeCamera();                // remove from the timeline

    void createID();

    int getCameraStartTime();

    //void updateSortOrder();

};

#endif
