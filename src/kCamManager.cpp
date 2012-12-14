#include "kCamManager.h"
#include "msbOFCore.h"
#include "kCam.h"

bool lessThanKey(kCam a, kCam b) {
    return a.startFrame < b.startFrame;
}
//---------------------------------------------
kCamManager::kCamManager() {
    // set the initial properties
}
//---------------------------------------------
void kCamManager::setup() {
    camKey = 0;              // start unique key at 0
    addCamera(0);            // add one camera at the beginning of the playhead
    ofSender.setup("127.0.0.1", 31842);
    activeCam = 0;
}

//---------------------------------------------
void kCamManager::addCamera(int frame) {
    kCam c;
    c.id=camKey;
    c.startFrame = frame;   // start the camera where the playhead is. TODO: THIS IS NOT SAVING THE RIGHT NUMBER - ONLY 0
    c.transform.data[15];

    roster.push_back(c);    // push this camera to the end of the stack by default
    cout << "added camera : " << c.id << endl;

    // and then make this added camera active
    activeCam = c.id;           // set new camera as active
    selectedCam = activeCam;    // and select it

    // send OSC
    ofxOscMessage myMessage;
    string oscAddress = "/cameraAdded";
    if(camKey !=0) {
        myMessage.addIntArg(camKey);
        myMessage.addIntArg(frame);
        myMessage.setAddress(oscAddress);
        ofSender.sendMessage(myMessage);
    }

    camKey++;               // increment unique key

    updateSortOrder();
}

//---------------------------------------------
bool kCamManager::bChangeActiveCam(int frame) {

    int camIndex = getActiveCamKey();
    cout << camIndex << endl;

    int previousCamStartFrame;
    int nextCamStartFrame;

    // TODO: WHY DOES THE START FRAME READ OUT ZERO
    if((camIndex > 1) && (camIndex < getNumCams() - 1)) {
        previousCamStartFrame = roster[camIndex - 1].startFrame;
        nextCamStartFrame = roster[camIndex + 1].startFrame;
    } else if (camIndex == 0) {
        previousCamStartFrame = -1;
        nextCamStartFrame = roster[camIndex + 1].startFrame;
    } else if (camIndex == (getNumCams() - 1)) {
        previousCamStartFrame = roster[camIndex - 1].startFrame;
        nextCamStartFrame = -1;
    }

    cout << "previousCamStartFrame: " << previousCamStartFrame << endl;
    cout << "CAM INDEX: " << camIndex << endl;
    cout << "nextCamStartFrame: " << previousCamStartFrame << endl;

    return false;

}

//---------------------------------------------
void kCamManager::removeCamera() { // we can only remove the activeCam

    if (getNumCams() == 1) {
        cout << "You cannot delete the only camera in the scene" << endl; // TODO: print this to the screen;
        return;
    }

    // you can only erase the camera if it's currently active
    // So erase the active camera from the the vector
      for(int i = 0; i < getNumCams(); i++) {
        if(roster[i].id == activeCam) {
            if (i == 0) {
                cout << "You cannot delete the first camera" << endl;
            } else {

           // send OSC
                ofxOscMessage myMessage;
                string oscAddress = "/cameraRemoved";
                if(camKey !=0) {
                    myMessage.addIntArg(activeCam);
                    myMessage.setAddress(oscAddress);
                    ofSender.sendMessage(myMessage);
                }
                // set the activeCam to the one prior to the camera we just deleted
                activeCam = roster[i-1].id;
                // erase the camera from the roster
                roster.erase(roster.begin() + i);
            }
        }
    }

    // and update the sort order to reflect the new arrangement
    updateSortOrder();
}



//---------------------------------------------
void kCamManager::updateSortOrder() {
    std::sort(roster.begin(), roster.end(), lessThanKey);
    cout << "sorting cameras" << endl;

}

//---------------------------------------------

int kCamManager::getNumCams() {
    return roster.size();
}

//---------------------------------------------
kCam kCamManager::getActiveKCam(){
    for(int i = 0; i < getNumCams(); i++) {
        if(roster[i].id == activeCam) {
            return roster[i];
        }
    }
}

//---------------------------------------------
int kCamManager::getActiveCamKey() {
     for(int i = 0; i < getNumCams(); i++) {
        if(roster[i].id == activeCam) {
            return i;
        }
    }
}
