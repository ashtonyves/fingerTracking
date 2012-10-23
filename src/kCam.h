#ifndef _KCAM
#define _KCAM

#include "ofMain.h"
#include "msbOFCore.h"

class KCam {

public:
    // Constructor
    KCam();

    // methods
    void grab();        // make selected and change the start time.
    void setPosition();    // change the transform matrix
    void drop();        // drop cam and adjust start time
    void remove();      // remove from the timeline

    // properties
    int id;             // id of the camer
    int startFrame;     // starting frame, corresponding to x position of blob
    Matrix4f transform; // transform matrix

    // maybe these are best indicated in the main class, rather than declaring each one active or selected...
    bool active;        // is this camera active?
    bool selected;      // is this camera selected
};



#endif _KCAM
