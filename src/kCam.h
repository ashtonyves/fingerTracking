#ifndef _KCAM
#define _KCAM

#include "ofMain.h"
#include "msbOFCore.h"

class kCam {

public:
    Matrix4f * camTransform; // transform matrix

    // Constructor
    kCam();

     // properties
    int id;             // id of the camera
    int startFrame;     // starting frame, corresponding to x position of blob
    bool visible;       // TODO:whether to show them in MSB or not

    // methods
    //void setCameraPosition();           // TODO change the transform matrix - refactor from current findFingerMatrix
    //void setCameraStartTime();          // drop cam and adjust startFrame

    // should just be getters in this class
    int getCameraStartTime();
    Matrix4f getCameraPosition();
    void setCameraPosition(Matrix4f* newPos);

    //void updateSortOrder();

};

#endif
