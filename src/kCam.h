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

    bool visible;       // TODO:whether to show them in MSB or not

    // methods
    void setCameraPosition();           // TODO change the transform matrix - refactor from current findFingerMatrix
    void setCameraStartTime();          // drop cam and adjust startFrame

    void createID();

    int getCameraStartTime();

    //void updateSortOrder();

};

#endif
