#include "kCamManager.h"
#include "msbOFCore.h"
#include "kCam.h"

kCamManager::kCamManager() {
    // set the initial properties
}

void kCamManager::setup() {
    camKey = 0;              // start unique key at 0
    addCamera(0);            // add one camera at the beginning of the playhead
}

//---------------------------------------------
void kCamManager::addCamera(int frame) {
    kCam c;
    c.id=camKey;
    c.startFrame = frame;   // start the camera where the playhead is.
    c.transform.data[15];
    roster.push_back(c);

    activeCam = c.id;       // set active camera to this new one.
    selectedCam = activeCam;

    // TODO: send this information out VIA OSC

    camKey++;               // increment unique key

    updateSortOrder();

    cout << "added camera : " << c.id << endl;
    cout << "number of cameras : " << getNumCams() << endl;
}

//---------------------------------------------
void kCamManager::removeCamera() { // we can only remove the activeCam

    if (getNumCams() == 1) {
        cout << "You cannot delete the only camera in the scene" << endl; // TODO: print this to the screen;
        return;
    }

      for(int i = 0; i < getNumCams(); i++) {
        if(roster[i].id == activeCam) {
            roster.erase(roster.begin() + i);
            // TODO: destroy the kCam item at roster[i]
            selectedCam = activeCam = i - 1;  // set the activeCam to the one prior to the camera we just deleted
        }
      }
    // remove the active camera from the roster
   /* for(int i = 0; i < getNumCams(); i++) { // we skip 0, the first element which cannot be deleted
        if(roster.[i] == getActiveCam) {
            //roster.erase(i);
            // TODO: destroy the kCam item at roster[i]
            activeCam = i - 1;  // set the activeCam to the one prior to the camera we just deleted
        }
    }
    */


    // and update the sort order to reflect the new arrangement
    updateSortOrder();

}


//---------------------------------------------
void kCamManager::updateSortOrder() {



    // TODO: compaison function
    // std::sort(roster.begin(), roster.end(), [ comparison function ] );
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
