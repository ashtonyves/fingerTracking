#include "kCamManager.h"
#include "msbOFCore.h"
#include "kCam.h"

bool lessThanKey(kCam a, kCam b) {
    return a.startFrame < b.startFrame;
}

kCamManager::kCamManager() {
    // set the initial properties
}

void kCamManager::setup() {
    camKey = 0;              // start unique key at 0
    addCamera(0);            // add one camera at the beginning of the playhead
    ofSender.setup("127.0.0.1", 31842);
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

    // TODO: send this information out VIA OSC
    ofxOscMessage myMessage;
    string oscAddress = "/cameraAdded";
    myMessage.addIntArg(c.id);
    myMessage.addIntArg(c.startFrame);
    myMessage.setAddress(oscAddress);

    if(c.id !=0) {
        ofSender.sendMessage(myMessage);
    }



    camKey++;               // increment unique key

    updateSortOrder();


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

           // TODO: send OSC

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
kCam kCamManager::getActiveCam(){
    for(int i = 0; i < getNumCams(); i++) {
        if(roster[i].id == activeCam) {
            return roster[i];
        }
    }
}

//---------------------------------------------
kCam kCamManager::getSelectedCam(){
    for(int i = 0; i < getNumCams(); i++) {
       if(roster[i].id == selectedCam) {
            return roster[i];
        }
    }
}
