#include "kCam.h"
#include "msbOFCore.h"
#include "ofMain.h"

kCam::kCam() {
    // set the initial properties
    camTransform = new Matrix4f();
}

/*void kCam::setCameraStartTime() {
    cout << "camera position set";
}*/

/*void kCam::setCameraPosition() {
    cout << "camera position set in space";
}*/

Matrix4f kCam::getCameraPosition() {
    return *camTransform;
}

void kCam::setCameraPosition(Matrix4f* newPos) {
    for(int i=0;i<16;i++){
        camTransform->data[i] = newPos->data[i];
    }
}
