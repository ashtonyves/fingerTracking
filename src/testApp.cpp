#include "testApp.h"

#include "actor.h"
#include "kCam.h"
#include "sliderButton.h"
#include "textInputButton.h"
#include "assignButton.h"
#include "msbLight.h"
#include <iostream>
#include <fstream>
#include <ctime>

#define BUF_SIZE 640*480*4*32
#define NUM_CAMS 20


//--------------------------------------------------------------
void testApp::setup(){

    totalNumFrames = 300; // matches GUI number of frames

    type.loadFont("DroidSans.ttf", 12);

 // TODO: create kCamManager Class to handle Camera vector array?
    // ------------ cameras
    playheadFrame = 0;      // starting position of the playhead. GUI should start the same way

    selectedCamera = 0; // perhaps change this to the first camera in the created vector array?

    bFullscreen=false;
    bLearnBackground=true;
    cutOffDepth=4096;
    bSetCutoffToZero=false;
    bufferLength=10;

    thresh= 66;

    bFoundMat=false;
    bFingerOut=false;

    bTwoHands = false;
    bOneHand = false;

    bSending=false;
    bCameraSelected=false;
    bScrubingPlayhead=false; // do not scrub playhead to start

    fistFactor=0;


    oldActiveHandX = -1; // start with the impossible value
    deltaHandX = 0;

    offsetVector=Vector3f(400,600,3);
    //   offsetVector=Vector3f(0,0,0);
    testMode="Creativity";

    ofSetFrameRate(30);

   // ipAddressMSB="143.215.199.192"; // NORMAL VERSION
    ipAddressMSB = "143.215.199.31";
   // ipAddressMSB="143.215.199.228";
  // ipAddressMSB="127.0.0.1";
    ipAddressProc="127.0.0.1";

    osc_senderMSB.setup(ipAddressMSB,31841);
    osc_senderProcessing.setup(ipAddressProc,31842);

    msbSetup();
    interfaceSetup();
    manager.setup();    // add one camera at 0

    //OF_STUFF

	//kinect.init(true);  //shows infrared image
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();

	myImage.allocate(640,480, OF_IMAGE_COLOR);
	myImage.setUseTexture(true);

	colorImg.allocate(640,480);
	grayImage.allocate(640,480);
	grayBg.allocate(640,480);
	grayDiff.allocate(640,480);

	pixelData=new unsigned char[640*480*3];
	frameCount = 0;
	frameCount = 0;

    interestAreaRect = ofRectangle(0,0,640,480);
    processingViewArea = ofRectangle(-80, -100, 210, 130);

	inCalibrationMode = true;
	usingScreenCalibration = true;

	// Create a file to save our run data to.
    // Get the current date/time and build file name
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    ostringstream ss;
    ss  << "output\\"
        << "MatrixTransform"<< '-'
        << (now->tm_mon + 1) << '-'
        << now->tm_mday << '-'
        << (now->tm_year+1900)<< '-'
        << now->tm_hour << '-'
        << now->tm_min << '-'
        << now->tm_sec
        <<".txt";
    saveFile = ss.str();
    cout << saveFile;
}

void testApp::msbSetup(){

    //MSB setup
    //MSB setup
    renderer=Renderer::getInstance();
    input=Input::getInstance();

    renderer->loadPreferences();

    renderer->setup();

    renderer->camActor=new Actor;
    renderer->camActor->setLocation(Vector3f(0,0,-5));
    renderer->camActor->postLoad();

    registerProperties();

    if (!kinect.bImage)
        return;

    // AG: what is this?
    //Adding MSB content
    //heightfield based on videoTexture from OF
    Actor* myActor = new Actor;
    myActor->setLocation(Vector3f(-0.5,-1.0,-5));
    myActor->setRotation(Vector3f(0,180,0));
    myActor->color=Vector4f(1,1,1,1);
    myActor->drawType=DRAW_POINTPATCH;
    myActor->particleScale=250;
    myActor->scale=Vector3f(1,-1,1);
    myActor->bTextured=true;
    myActor->textureID="NULL";
    myActor->sceneShaderID="heightfield";
    myActor->setup();

    renderer->actorList.push_back(myActor);
    renderer->layerList[0]->actorList.push_back(myActor);

    patchActor=myActor;

}

void testApp::interfaceSetup(){

    TextInputButton *tiBut;

    tiBut= new TextInputButton;
    tiBut->location.x=650;
    tiBut->location.y=5;
    tiBut->scale.x=100;
    tiBut->scale.y=12;
    tiBut->color=Vector4f(0.5,0.5,0.5,1.0);
    tiBut->textureID="icon_flat";
    tiBut->name="offsetVector";
    tiBut->bDrawName=true;
    tiBut->setLocation(tiBut->location);
    tiBut->parent=this;
    tiBut->buttonProperty="OFFSETVECTOR";
    renderer->buttonList.push_back(tiBut);
    tiBut->bPermanent=true;
    tiBut->setup();


    tiBut= new TextInputButton;
    tiBut->location.x=650;
    tiBut->location.y=20;
    tiBut->scale.x=100;
    tiBut->scale.y=12;
    tiBut->color=Vector4f(0.5,0.5,0.5,1.0);
    tiBut->textureID="icon_flat";
    tiBut->name="CV_Threshhold";
    tiBut->bDrawName=true;
    tiBut->setLocation(tiBut->location);
    tiBut->parent=this;
    tiBut->buttonProperty="THRESH";
    renderer->buttonList.push_back(tiBut);
    tiBut->bPermanent=true;
    tiBut->setup();


    tiBut= new TextInputButton;
    tiBut->location.x=650;
    tiBut->location.y=35;
    tiBut->scale.x=100;
    tiBut->scale.y=12;
    tiBut->color=Vector4f(0.5,0.5,0.5,1.0);
    tiBut->textureID="icon_flat";
    tiBut->name="Mode";
    tiBut->bDrawName=true;
    tiBut->setLocation(tiBut->location);
    tiBut->parent=this;
    tiBut->buttonProperty="TESTMODE";
    renderer->buttonList.push_back(tiBut);
    tiBut->bPermanent=true;
    tiBut->setup();

}


void testApp::registerProperties(){
   createMemberID("OFFSETVECTOR",&offsetVector,this);
   createMemberID("THRESH",&thresh,this);
   createMemberID("TESTMODE",&testMode,this);
}

int testApp::shareMemory(){


    //store xyz values in rgb pixels, soon.
    if (kinect.getDepthPixels()){


        //construct full color image
        for (int i=0;i<640*480*4;i+=4){
            myPic[i]=(float)kinect.getPixels()[640*480 * 3 - (i/4) *3] * 1.0f/255.0f;
            myPic[i+1]=(float)kinect.getPixels()[640*480 * 3 - (i/4) *3 + 1] * 1.0f/255.0f;
            myPic[i+2]=(float)kinect.getPixels()[640*480 * 3 - (i/4) *3 + 2] * 1.0f/255.0f;
            //alpha from here
            myPic[i+3]=(float)kinect.getDistancePixels()[640*480 - i/4];
            if (myPic[i+3]>cutOffDepth){
                if (bSetCutoffToZero)
                    myPic[i+3]=0.0f;
                else
                    myPic[i+3]=1.0f;
            }else{
                myPic[i+3]=myPic[i+3]/cutOffDepth;
                if (myPic[i+3]<=0){
                    if (bSetCutoffToZero)
                        myPic[i+3]=0.0f;
                    else
                        myPic[i+3]=1.0f;
                }

            }

        }
	}
   // _getch();
}

//--------------------------------------------------------------
void testApp::update(){
/*
cout << "X: "<<interestAreaRect.x << endl;
cout << "Y: "<<interestAreaRect.y << endl;
cout << "WIDTH: "<<interestAreaRect.width << endl;
cout << "HEIGHT: "<<interestAreaRect.height << endl;
*/


    fingerStart*= 0;
    fingerEnd*= 0;

    bSending = false;

    bTwoHands = false;
    bOneHand = false;

    bFoundMat=false;
    bFingerOut=false;
    bScrubingPlayhead=false;

	ofBackground(100, 100, 100);
	kinect.update();

    renderer->update();

// --- TEMPORARY CODE FOR TESTING
    if(usingScreenCalibration){
 /*       for (int i=0;i<interestAreaRect.width;i++){
            for(int j=0;j<interestAreaRect.height;j++){
                int kinectIndex = ((j+interestAreaRect.y)*640)+i+interestAreaRect.x;
                int pixelDataIndex = (j*interestAreaRect.width)+i;
                if (kinect.depthPixels[pixelDataIndex]>0){
                    pixelData[pixelDataIndex*3]=kinect.depthPixels[kinectIndex];
                    pixelData[pixelDataIndex*3+1]=kinect.depthPixels[kinectIndex];
                    pixelData[pixelDataIndex*3+2]=kinect.depthPixels[kinectIndex];
                }else{
                    pixelData[pixelDataIndex*3]=0;
                    pixelData[pixelDataIndex*3+1]=0;
                    pixelData[pixelDataIndex*3+2]=0;
                }
            }
        }

        colorImg.setFromPixels(pixelData, interestAreaRect.width,interestAreaRect.height);

        grayImage = colorImg;
        grayImage.threshold(thresh);
        grayImage.invert();

        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        // AG: might need to change value for nConsidered from 5 to 10 to track 2 palms
        contourFinder.findContours(grayImage, 1000,interestAreaRect.width*interestAreaRect.height/2 , 5, false);	// no find holes
 */
        for (int i=0;i<640*480;i++){
            if (kinect.depthPixels[i]>0){
                pixelData[i*3]=kinect.depthPixels[i];
                pixelData[i*3+1]=kinect.depthPixels[i];
                pixelData[i*3+2]=kinect.depthPixels[i];
            }else{
                pixelData[i*3]=0;
                pixelData[i*3+1]=0;
                pixelData[i*3+2]=0;
            }
        }

        colorImg.setFromPixels(pixelData, 640,480);

        grayImage = colorImg;
        grayImage.threshold(thresh);
        grayImage.invert();

        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        // AG: might need to change value for nConsidered from 5 to 10 to track 2 palms
        contourFinder.findContours(grayImage, 1000,640*480/2 , 5, false);	// no find holes
    }

    else{
        for (int i=0;i<640*480;i++){
                if (kinect.depthPixels[i]>0){
                    pixelData[i*3]=kinect.depthPixels[i];
                    pixelData[i*3+1]=kinect.depthPixels[i];
                    pixelData[i*3+2]=kinect.depthPixels[i];
                }else{
                    pixelData[i*3]=0;
                    pixelData[i*3+1]=0;
                    pixelData[i*3+2]=0;
                }
        }

        colorImg.setFromPixels(pixelData, 640,480);

        grayImage = colorImg;
        grayImage.threshold(thresh);
        grayImage.invert();

        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        // AG: might need to change value for nConsidered from 5 to 10 to track 2 palms
        contourFinder.findContours(grayImage, 1000,640*480/2 , 5, false);	// no find holes
    }

// --- END TEMPORARY CODE FOR TESTING




    Matrix4f idMatrix;
    idMatrix.identity();

    findHands();  //and additionally this will check if both hands are palms or palm and fist

    if(bTwoHands) {

        if(bScrubingPlayhead) {
            sendData("sendPlayhead");
            bSending = true;
            //manager.bChangeActiveCam(playheadFrame);
            manager.updateActiveCamera(playheadFrame);
            sendData("activeCamera");

            fingerTransformation = manager.getActiveKCam().getCameraPosition();
            //sendData("setCameraPos"); //  oh noes. Don't set the camera position when two hands are in the frame... AG 5-2-2013
        } if(bScrubbingCamera) {
            //TODO sendData("moveCamera");
            //bSending = true;
        }

        // and if the right hand is a fist, then we grab the active camera
    } else if (bOneHand) {
        // set booleans by running functions on each loop
        bFoundMat=findFingerMatrix();
        if(bFoundMat){
            bFingerOut=findFinger();
            if (bFoundMat && bFingerOut){ // do not set position if two hands are in the frame
                sendData("setCameraPos");
                bSending=true;
            }
        }
    }

    if(usingScreenCalibration && inCalibrationMode){
        calibrateToScreen();
    }
    frameCount ++;
}

//--------------------------------------------------------------
bool testApp::findFingerMatrix(){

    for (int i = 0; i < contourFinder.nBlobs; i++){

        ofPoint* edge=NULL;
        ofPoint* ctroid=NULL;

        ofPoint* edOne=NULL;
        ofPoint* edTwo=NULL;



        ///TOP
        if (contourFinder.blobs[i].boundingRect.y < 10){


                //not in picture enough
                if (contourFinder.blobs[i].boundingRect.height<150)
                    return false;


                //find furthest point

                //if wider than tall - figure out if point closest to edge is left or right
                if (contourFinder.blobs[i].boundingRect.width>contourFinder.blobs[i].boundingRect.height){


                    //find right edge point
                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].x>contourFinder.blobs[i].boundingRect.x+ contourFinder.blobs[i].boundingRect.width-10 )
                            edOne=&contourFinder.blobs[i].pts[b];
                    }

                    //find left edge point
                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].x<contourFinder.blobs[i].boundingRect.x+10)
                            edTwo=&contourFinder.blobs[i].pts[b];
                    }

                    //compare to see which one is further out
                    if (edOne->y>edTwo->y){
                        edge=edOne;
                    }
                    else{
                        edge=edTwo;
                    }

                }
                //if longer (height) than wide(width)
                else{

                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].y>contourFinder.blobs[i].boundingRect.height-10)
                            edge=&contourFinder.blobs[i].pts[b];
                    }
                }

            }

            ///LEFT
            if (contourFinder.blobs[i].boundingRect.x < 10){

                //not in picture enough
                if (contourFinder.blobs[i].boundingRect.width<150)
                    return false;


                //find furthest point
                // Things flip to longer than wide here!
                //if longer than wide - figure out if point closest to edge is up or right
                if (contourFinder.blobs[i].boundingRect.width<contourFinder.blobs[i].boundingRect.height){


                    //find lower edge point
                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].y>contourFinder.blobs[i].boundingRect.y+ contourFinder.blobs[i].boundingRect.height-10 )
                            edOne=&contourFinder.blobs[i].pts[b];
                    }

                    //find upper edge point
                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].y<contourFinder.blobs[i].boundingRect.y+10)
                            edTwo=&contourFinder.blobs[i].pts[b];
                    }

                    //compare to see which one is further out
                    if (edOne->x>edTwo->x){
                        edge=edOne;
                    }
                    else{
                        edge=edTwo;
                    }

                }
                //if wider (width) than long(height)
                else{

                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].x>contourFinder.blobs[i].boundingRect.width-10)
                            edge=&contourFinder.blobs[i].pts[b];

                    }
                }

            }

            ///RIGHT
            if (contourFinder.blobs[i].boundingRect.x+ contourFinder.blobs[i].boundingRect.width> 630){

                //not in picture enough
                if (contourFinder.blobs[i].boundingRect.x>550)
                    return false;


                //find furthest point
                // Things flip to longer than wide here!
                //if longer than wide - figure out if point closest to edge is up or right
                if (contourFinder.blobs[i].boundingRect.width<contourFinder.blobs[i].boundingRect.height){

                    //find lower edge point
                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].y>contourFinder.blobs[i].boundingRect.y+ contourFinder.blobs[i].boundingRect.height-10 )
                            edOne=&contourFinder.blobs[i].pts[b];
                    }

                    //find upper edge point
                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].y<contourFinder.blobs[i].boundingRect.y+10)
                            edTwo=&contourFinder.blobs[i].pts[b];
                    }

                    //compare to see which one is further out
                    if (edOne->x<edTwo->x){
                        edge=edOne;
                    }
                    else{
                        edge=edTwo;
                    }



                }
                //if wider (width) than long(height)
                else{

                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].x<contourFinder.blobs[i].boundingRect.x+10)
                            edge=&contourFinder.blobs[i].pts[b];

                    }
                }

            }
        ///BOTTOM
        if (contourFinder.blobs[i].boundingRect.height + contourFinder.blobs[i].boundingRect.y > 470){

                //not in picture enough
                if (contourFinder.blobs[i].boundingRect.y>400)
                    return false;


                //find furthest point

                //if wider than long - figure out if point closest to edge is left or right
                if (contourFinder.blobs[i].boundingRect.width>contourFinder.blobs[i].boundingRect.height){



                    //find right edge point
                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].x>contourFinder.blobs[i].boundingRect.x+ contourFinder.blobs[i].boundingRect.width-10 )
                            edOne=&contourFinder.blobs[i].pts[b];
                    }

                    //find left edge point
                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].x<contourFinder.blobs[i].boundingRect.x+10)
                            edTwo=&contourFinder.blobs[i].pts[b];
                    }

                    //compare to see which one is further out
                    if (edOne->y<edTwo->y){
                        edge=edOne;
                    }
                    else{
                        edge=edTwo;
                    }

                }
                //if longer (height) than wide(width)
                else{

                    for (int b=0;b<contourFinder.blobs[i].nPts;b++){
                        if (contourFinder.blobs[i].pts[b].y<contourFinder.blobs[i].boundingRect.y +10)
                            edge=&contourFinder.blobs[i].pts[b];
                    }
                }

            }


            //centroid to fingertip
            ctroid=&contourFinder.blobs[i].centroid;

            if (!edge)
                return false;
            else{
                fingerStart=*edge;
                fingerEnd=*ctroid;
            }

            float depthEdge=0.0f;
            float depthCtroid=0.0f;

            depthEdge=kinect.getDistanceAt(*edge);
            depthCtroid=kinect.getDistanceAt(*ctroid);

            if (depthCtroid-depthEdge<-500)
                return false;

            Vector3f pointHand=Vector3f(ctroid->x,depthCtroid,ctroid->y);   //because we're watching from the top, so y is z
            Vector3f pointFinger=Vector3f(edge->x,depthEdge,edge->y);
            Vector3f zA=pointHand-pointFinger;
          //  zA=pointFinger- pointHand;
            //zA.y=0.0;
            zA.normalize();

            zA.z=-zA.z;
         //   zA=-zA;

            Vector3f yA=Vector3f(0,-1,0);
            Vector3f xA=zA.crossProduct(yA);
            //xA.y=0.0;
            xA.normalize();

            yA=xA.crossProduct(zA);

            yA.normalize();

            //cout << xA << " and " << zA << endl;


            fingerTransformation.data[0]=xA.x;
            fingerTransformation.data[1]=xA.y;
            fingerTransformation.data[2]=xA.z;

            fingerTransformation.data[4]=yA.x;
            fingerTransformation.data[5]=yA.y;
            fingerTransformation.data[6]=yA.z;

            fingerTransformation.data[8]=zA.x;
            fingerTransformation.data[9]=zA.y;
            fingerTransformation.data[10]=zA.z;

            fingerTransformation.data[15]=1.0;
            //fingerTransformation=fingerTransformation.transpose();

           // fingerTransformation.setTranslation(Vector3f(edge->x*offsetVector.z -offsetVector.x,-(depthCtroid-1000.0),(-edge->y* offsetVector.z)+offsetVector.y  )*0.1);
            if(   edge->x > interestAreaRect.x && edge->x < interestAreaRect.x+interestAreaRect.width
               && edge->y > interestAreaRect.y && edge->y < interestAreaRect.y+interestAreaRect.height){
                fingerTransformation.setTranslation(Vector3f((1-(edge->x-interestAreaRect.x)/interestAreaRect.width)*processingViewArea.width+processingViewArea.x,(1000-depthCtroid)*0.1,(1-(edge->y-interestAreaRect.y)/interestAreaRect.height)*processingViewArea.height+processingViewArea.y));
              // fingerTransformation.setTranslation(Vector3f(30,13,-20));

              //  cout << Vector3f((1-(edge->x-interestAreaRect.x)/interestAreaRect.width)*processingViewArea.width+processingViewArea.x,(1000-depthCtroid)*0.1,-((((edge->y-interestAreaRect.y)/interestAreaRect.height)*processingViewArea.height)+processingViewArea.y));
            }
            else {
                return false; // Finger out of bounds. Do not use.
                //fingerTransformation.setTranslation(Vector3f(edge->x,-(depthCtroid-1000.0),-edge->y)*0.1);
            }
  //          fingerTransformation.setTranslation(Vector3f((edge->x/interestAreaRect.width)*169-52,(1000-depthCtroid)*0.1,(edge->y/interestAreaRect.height)*105.25-68));

            Vector3f myT=fingerTransformation.getTranslation();

            myT.x=64.0-myT.x;
            myT.z=-48-myT.z;

            //myT+=offsetVector;

            fingerTransformation.setTranslation(myT);

            //cout << myT << endl;


            transformBuffer.push_back(fingerTransformation);
            if (transformBuffer.size()>bufferLength)
                transformBuffer.erase(transformBuffer.begin());

            fingerTransformation.identity();
            for (int i=0;i<transformBuffer.size();i++)
                fingerTransformation=fingerTransformation+transformBuffer[i];

            fingerTransformation=fingerTransformation/(float)bufferLength;


            //cout << fingerTransformation << endl;

            fingerTransformation.data[15]=1.0;


            manager.getActiveKCam().setCameraPosition(&fingerTransformation);

            free(edge);
            free(ctroid);
            return true;
    }

    return false;
}



void testApp::calibrateToScreen(){
   // cout << "Frame Count " << frameCount << " -- Mod " << frameCount % 15;
    if(frameCount % 10 == 0){
        //check image
        int centerVal = kinect.getDepthPixels()[(240*640)+320];
        int numPoints = 10;
        int depthTolerance = 7;

        ofPoint testPoint = ofPoint(0,0);

        int pixCol = 0, pixRow = 0;
        int numInThresh = 0;
        int testVal = 0;

        bool bFound = false;

        // Start X
        while(pixCol<320 && !bFound){
            testPoint.x = pixCol;
            testPoint.y = 0;
            numInThresh = 0;
            for(int i=0;i<numPoints;i++){
               // cout << "x: " << testPoint.x << "y: "<< testPoint.y << endl;
                if( ( abs(      kinect.getDepthPixels()[(int)((testPoint.y*640)+testPoint.x)]  ) - centerVal) < depthTolerance ){
                    numInThresh++;
                }
                testPoint.y += (int)480/numPoints;
            }
            if(numInThresh >= numPoints/2){
                bFound=true;
                //cout << "Screen edge found at: " << pixCol << endl;
                interestAreaRect.x = pixCol;
            }
            else {
                pixCol++;
            }
        }

        // Start Y
        pixRow = 0;
        bFound = false;
        while(pixRow<240 && !bFound){
            testPoint.x = 0;
            testPoint.y = pixRow;
            numInThresh = 0;
            for(int i=0;i<numPoints;i++){
               // cout << "x: " << testPoint.x << "y: "<< testPoint.y << endl;
                if( ( abs(      kinect.getDepthPixels()[(int)((testPoint.y*640)+testPoint.x)]  ) - centerVal) < depthTolerance ){
                    numInThresh++;
                }
                testPoint.x += (int)640/numPoints;
            }
            if(numInThresh >= numPoints/2){
                bFound=true;
               // cout << "Screen edge found at: " << pixCol << endl;
                interestAreaRect.y = pixRow;
            }
            else {
                pixRow++;
            }
        }

        // End X
        pixCol = 640;
        bFound = false;
        while(pixCol>320 && !bFound){
            testPoint.x = pixCol;
            testPoint.y = 0;
            numInThresh = 0;
            for(int i=0;i<numPoints;i++){
               // cout << "x: " << testPoint.x << "y: "<< testPoint.y << endl;
                if( ( abs(      kinect.getDepthPixels()[(int)((testPoint.y*640)+testPoint.x)]  ) - centerVal) < depthTolerance ){
                    numInThresh++;
                }
                testPoint.y += (int)480/numPoints;
            }
            if(numInThresh >= numPoints/2){
                bFound=true;
               // cout << "Screen edge found at: " << pixCol << endl;
                interestAreaRect.width = pixCol-interestAreaRect.x;
            }
            else {
                pixCol--;
            }
        }

        // End Y
        pixRow = 479;
        bFound = false;
        while(pixRow>240 && !bFound){
            testPoint.x = 0;
            testPoint.y = pixRow;
            numInThresh = 0;
            for(int i=0;i<numPoints;i++){
               // cout << "x: " << testPoint.x << "y: "<< testPoint.y << endl;
                if( ( abs(      kinect.getDepthPixels()[(int)((testPoint.y*640)+testPoint.x)]  ) - centerVal) < depthTolerance ){
                    numInThresh++;
                }
                testPoint.x += (int)640/numPoints;
            }
            if(numInThresh >= numPoints/2){
                bFound=true;
               // cout << "Screen edge found at: " << pixCol << endl;
                interestAreaRect.height = pixRow-interestAreaRect.y;
            }
            else {
                pixRow--;
            }
        }

        // colorImg = ofxCvColorImage();
      //  colorImg.resize(interestAreaRect.width,interestAreaRect.height);
      //  grayImage.resize(interestAreaRect.width,interestAreaRect.height);
    }
}



//--------------------------------------------------------------
//
bool testApp::findFinger(){

        Vector3f vec;
        vec.x=fingerStart.x-fingerEnd.x;
        vec.z=fingerStart.y-fingerEnd.y;

        vec*=0.1;

        //check the points below, if two or more are white, there is no finger extended!

        //rectangular vector
        Vector3f yAx=Vector3f(0,1,0);
        Vector3f perp=yAx.crossProduct(vec);

        perp*=0.65;

        ofSetColor(64,64,255);
        one=ofPoint(fingerStart.x- vec.x-perp.x,fingerStart.y- vec.z -perp.z);
        two=ofPoint(fingerStart.x- vec.x+perp.x,fingerStart.y- vec.z +perp.z);

        perp+=perp;

        three=ofPoint(fingerStart.x- vec.x-perp.x,fingerStart.y- vec.z -perp.z);
        four=ofPoint(fingerStart.x- vec.x+perp.x,fingerStart.y- vec.z +perp.z);

        perp+=perp;

        five=ofPoint(fingerStart.x- vec.x-perp.x,fingerStart.y- vec.z -perp.z);
        six=ofPoint(fingerStart.x- vec.x+perp.x,fingerStart.y- vec.z +perp.z);



        //check image
        int width=640;
        unsigned char* wb;
        wb=grayImage.getPixels();

        fistFactor=0;
        int grain=6;

        for (int i=0;i<480;i+=grain){
                for (int j=0;j<640;j+=grain){

                    bool bIn=false;

                     if (wb[i*((int)640)+j]>0)   bIn=true;

                    //check point one
                    if (abs(one.y-i)<grain && abs(one.x-j)<grain ){
                        if (bIn) fistFactor++;
                    }

                    //check point two
                    if (abs(two.y-i)<grain && abs(two.x-j)<grain ){
                        if (bIn) fistFactor++;
                    }

                    //check point three
                    if (abs(three.y-i)<grain && abs(three.x-j)<grain ){
                        if (bIn) fistFactor++;
                    }

                    //check point four
                    if (abs(four.y-i)<grain && abs(four.x-j)<grain ){
                        if (bIn) fistFactor++;
                     }

                    //check point five
                    if (abs(five.y-i)<grain && abs(five.x-j)<grain ){
                        if (bIn)  fistFactor++;
                    }

                    //check point six
                    if (abs(six.y-i)<grain && abs(six.x-j)<grain ){
                        if (bIn)  fistFactor++;
                    }
                }
        }

    if (fistFactor<10)
        return true;
     else
        return false;

}

//--------------------------------------------------------------
void testApp::findHands() {
    if(contourFinder.nBlobs == 2) {
        bTwoHands=true;
        scrubPlayhead();
    } else if (contourFinder.nBlobs < 2){
        oldActiveHandX = -1; // reset the key for the two-handed delta value
        if(contourFinder.nBlobs == 1) {
            bOneHand = true;
        } else if (contourFinder.nBlobs == 0) {
            // no hands in frame
            sendData("noContours");
        }
    }

}

//--------------------------------------------------------------
int testApp::mapPosToFrame(int pixelPos) { // called by findHands - convert the pixel value from the kinect to a frame on the timeline
    int frameValue = (totalNumFrames*pixelPos)/640;  // 640 is the Kinect width. Perhaps...should be a constant?
    return frameValue;
}

//--------------------------------------------------------------
void testApp::scrubPlayhead() {

         // find the blob with the highest x value (the one on the right) and set it to activeHand
        ofxCvBlob activeHand;

        if(contourFinder.blobs[0].centroid.x > contourFinder.blobs[1].centroid.x) {
            activeHand = contourFinder.blobs[0];
        } else  {
            activeHand = contourFinder.blobs[1];
        }

        if(oldActiveHandX != -1) {
            deltaHandX = activeHand.centroid.x - oldActiveHandX;
        } else {            // hand just entered the frame. Just store its value on this run.
            deltaHandX = 0; // spit out a value so the program doesn't cry.
        }
        oldActiveHandX = activeHand.centroid.x; // store activeHandX for next run
        //cout << "ACTIVE HAND X: " << activeHand.centroid.x << endl;
        //cout << "DELTA HAND X: " << deltaHandX << endl;

    // TODO Check if we are scrubbing the playhead or moving a camera as well:
            //if (activeHand is a palm) -->
                //bScrubbingPlayhead = true
            // else if (activeHand is a fist)
                //bScrubbingCamera = true
    bScrubingPlayhead = true; // temp

    if(bScrubingPlayhead) { // we are moving the playhead
        int newPlayheadFrame = playheadFrame + mapPosToFrame(deltaHandX);

        // set upper and lower bounds for frames we can scrub to
        if (newPlayheadFrame < 0) {
            playheadFrame = 0;
        } else if (newPlayheadFrame > totalNumFrames) {
            playheadFrame = totalNumFrames;
        } else {
            playheadFrame = newPlayheadFrame;
        }
    } else if (bScrubbingCamera) { // we are moving a camera (AND the playhead? this may need to do inside the previous function
        // first snap playhead to first frame of active camera
        // then start moving playhead with camera. (see how this affects Brandon's tick movement.
    }



}



//--------------------------------------------------------------
void testApp::sendData(string e){

    ofxOscMessage myMessage;
    string oscAddress;


    //if(e == "setCameraPos" && !bTwoHands) {
        if(e == "setCameraPos") {
            //fingerTransformation.transpose();
            oscAddress = "/setPropertyForSelected/string/matrix4f";
            //oscAddress = "/pilot/float/float"
            myMessage.addStringArg("TRANSFORMMATRIX");
            myMessage.addFloatArg(fingerTransformation.data[0]);
            myMessage.addFloatArg(fingerTransformation.data[1]);
            myMessage.addFloatArg(fingerTransformation.data[2]);
            myMessage.addFloatArg(fingerTransformation.data[3]);
            myMessage.addFloatArg(fingerTransformation.data[4]);
            myMessage.addFloatArg(fingerTransformation.data[5]);
            myMessage.addFloatArg(fingerTransformation.data[6]);
            myMessage.addFloatArg(fingerTransformation.data[7]);
            myMessage.addFloatArg(fingerTransformation.data[8]);
            myMessage.addFloatArg(fingerTransformation.data[9]);
            myMessage.addFloatArg(fingerTransformation.data[10]);
            myMessage.addFloatArg(fingerTransformation.data[11]);
            myMessage.addFloatArg(fingerTransformation.data[12]);
            myMessage.addFloatArg(fingerTransformation.data[13]);
            myMessage.addFloatArg(fingerTransformation.data[14]);
            myMessage.addFloatArg(fingerTransformation.data[15]);
    }

    else if(e == "sendPlayhead") { // scrub mode
        oscAddress = "/setPlayheadFrame/int";
        myMessage.addIntArg(playheadFrame);
        cout << "OSC playheadFrame currently at: " << playheadFrame << endl;
    }
    else if(e == "cameraAdded") { // Added camera. We assume the camera has already been added to the roster (in testApp), and that it is now active.
        oscAddress = "/cameraAdded/int/int";
        //myMessage.addIntArg(manager.activeCam);
        //myMessage.addIntArg(manager.getActiveKCam().startFrame);

        myMessage.addIntArg(24);
        myMessage.addIntArg(200);

     //   myMessage.addIntArg(playheadFrame);
     //   cout << "OSC playheadFrame currently at: " << playheadFrame << endl;
    }
    else if(e == "activeCamera") { // Sends across the currently active camera
        oscAddress = "/activeCamera/int";
        myMessage.addIntArg(manager.activeCam);
    }
    else if(e=="noContours") { // no hands in staging area
        oscAddress = "/anyBlobs/string";
        myMessage.addStringArg("false");
        //cout << "no counters" << endl;
    }
        myMessage.setAddress(oscAddress);
        osc_senderMSB.sendMessage(myMessage);
        osc_senderProcessing.sendMessage(myMessage);


}

void testApp::selectMsbObjectByName(string n){
    ofxOscMessage myMessage;

    string oscAddress = "/selectActorByName";
    string sendString=n;

    cout << "Selected actor " << n;

    myMessage.addStringArg(n);
    myMessage.setAddress(oscAddress);

    osc_senderMSB.sendMessage(myMessage);

}

//--------------------------------------------------------------
void testApp::draw(){


    if (bFullscreen){

        kinect.drawDepth(0, 0, renderer->screenX, renderer->screenY);

    }else{


        glEnable(GL_LIGHTING);
        renderer->draw();
        glDisable(GL_LIGHTING);

        //ofSetHexColor(0xffffff);

        kinect.drawDepth(0, 0, 640, 480);

    }
        //kinect.draw(420, 50, 400, 300);
        //colorImg.draw(420,50,400,300);
        //colorImg.draw(420,50,400,300);

        grayImage.draw(660,50,320,240);
        //grayImage.draw(660,50);

        glPushMatrix();
/*
        ofSetColor(0,255,0);
        ofLine(fingerStart.x+interestAreaRect.x,fingerStart.y+interestAreaRect.y,fingerEnd.x+interestAreaRect.x,fingerEnd.y+interestAreaRect.y);

        ofSetColor(128,0,0);
        ofRect(fingerStart.x+interestAreaRect.x,fingerStart.y+interestAreaRect.y,10,10);

        ofSetColor(255,0,0);
        ofRect(fingerEnd.x+interestAreaRect.x,fingerEnd.y+interestAreaRect.y,10,10);

        ofSetColor(64,64,255);
        ofRect(one.x+interestAreaRect.x,one.y+interestAreaRect.y,5,5);
        ofRect(two.x+interestAreaRect.x,two.y+interestAreaRect.y,5,5);
        ofRect(three.x+interestAreaRect.x,three.y+interestAreaRect.y,5,5);
        ofRect(four.x+interestAreaRect.x,four.y+interestAreaRect.y,5,5);
        ofRect(five.x+interestAreaRect.x,five.y+interestAreaRect.y,5,5);
        ofRect(six.x+interestAreaRect.x,six.y+interestAreaRect.y,5,5);
*/

        ofSetColor(0,255,0);
        ofLine(fingerStart.x,fingerStart.y,fingerEnd.x,fingerEnd.y);

        ofSetColor(128,0,0);
        ofRect(fingerStart.x,fingerStart.y,10,10);

        ofSetColor(255,0,0);
        ofRect(fingerEnd.x,fingerEnd.y,10,10);

        ofSetColor(64,64,255);
        ofRect(one.x,one.y,5,5);
        ofRect(two.x,two.y,5,5);
        ofRect(three.x,three.y,5,5);
        ofRect(four.x,four.y,5,5);
        ofRect(five.x,five.y,5,5);
        ofRect(six.x,six.y,5,5);
        if(usingScreenCalibration && inCalibrationMode) {
            // Draw edge calibration debug
            ofSetColor(255,64,64);
            ofRect(interestAreaRect.x,0,4,480);
            ofRect(0,interestAreaRect.y,640,4);
            ofRect(interestAreaRect.x+interestAreaRect.width,0,4,480);
            ofRect(0,interestAreaRect.y+interestAreaRect.height,640,4);

            ofSetColor(64,255,64,75);
            ofRect(interestAreaRect.x+4, interestAreaRect.y+4, interestAreaRect.width-4, interestAreaRect.height-4);

            ofSetColor(255,64,64);

            type.drawString("Calibrate: Press ENTER when you are satisfied with position of the play zone.", 15,500);
        }


     if (bFingerOut)
         ofSetColor(0,255,0);
     else
         ofSetColor(128,128,128);
    ofCircle(700,500,20);

    if (bSending)
        ofSetColor(255,0,0);
    else
        ofSetColor(0,0,0);
    ofCircle(700,550,20);

    glPopMatrix();

    if(usingScreenCalibration){
        //contourFinder.draw(interestAreaRect.x,interestAreaRect.y);
        contourFinder.draw(0,0);
    }
    else {
        contourFinder.draw(0,0);
    }

    ofSetColor(255,255,255);
     // print report String to interface
    char CameraReportStr[1024];
    sprintf(CameraReportStr, "CAMERA INFO\nnumber of cameras: %i\nselected camera id: %i\nactive camera id: %i\n", manager.getNumCams(), manager.selectedCam, manager.activeCam);

    char BlobReportStr[1024];
    sprintf(BlobReportStr, "BLOB INFO\nnumber of blobs %i\nFist factor: %i", contourFinder.nBlobs, fistFactor);

    type.drawString(CameraReportStr, 20, 600);
    type.drawString(BlobReportStr, 320, 600);
    type.drawString("Playhead: " + ofToString(playheadFrame),20, 550);

    type.drawString("CAMERAS", 650, 380);
    for(int i = 0; i < manager.getNumCams(); i++) {
       type.drawString("id: " + ofToString(manager.roster[i].id) + " start: " + ofToString(manager.roster[i].startFrame), 650, 400 + 20*i);
    }







}


//--------------------------------------------------------------
void testApp::exit(){


}

//--------------------------------------------------------------
void testApp::keyPressed (int key){

    input->normalKeyDown(key,mouseX,mouseY);
    input->specialKeyDown(key,mouseX,mouseY);


    switch(key) {
        case(13): // ENTER key is pressed
        {
            if(usingScreenCalibration){
                inCalibrationMode = !inCalibrationMode;
                if(!inCalibrationMode){
                    interestAreaRect.x -= 0;
                    interestAreaRect.y -= 0;
                    interestAreaRect.width += 0*2;
                    interestAreaRect.height += 0*2;
              //     colorImg.resize(interestAreaRect.width,interestAreaRect.height);
              //     grayImage.resize(interestAreaRect.width,interestAreaRect.height);
                }
            }
        }
        break;
        case('c'): // ENTER key is pressed
        {
 /*           cout << fingerTransformation.data[0] << endl;
            cout << fingerTransformation.data[1]<< endl;
            cout << fingerTransformation.data[2]<< endl;
            cout << fingerTransformation.data[3]<< endl;
            cout << fingerTransformation.data[4]<< endl;
            cout << fingerTransformation.data[5]<< endl;
            cout << fingerTransformation.data[6]<< endl;
            cout << fingerTransformation.data[7]<< endl;
            cout << fingerTransformation.data[8]<< endl;
            cout << fingerTransformation.data[9]<< endl;
            cout << fingerTransformation.data[10]<< endl;
            cout << fingerTransformation.data[11]<< endl;
            cout << fingerTransformation.data[12]<< endl;
            cout << fingerTransformation.data[13]<< endl;
            cout << fingerTransformation.data[14]<< endl;
            cout << fingerTransformation.data[15]<< endl;
*/



            selectMsbObjectByName("Camera1");
            /*
            fingerTransformation.data[0]=-0.1231;
            fingerTransformation.data[1]=0;
            fingerTransformation.data[2]=0.5121;

            fingerTransformation.data[4]=-0.01638;
            fingerTransformation.data[5]=0.9272;
            fingerTransformation.data[6]=-0.3067;

            fingerTransformation.data[8]=-0.5316;
            fingerTransformation.data[9]=-0.3915;
            fingerTransformation.data[10]=0.04186;

            fingerTransformation.data[15]=1.0;
            fingerTransformation.setTranslation(Vector3f(41,32,24));

            */
            fingerTransformation.data[0]=1;
            fingerTransformation.data[1]=0;
            fingerTransformation.data[2]=0;

            fingerTransformation.data[4]=0;
            fingerTransformation.data[5]=1;
            fingerTransformation.data[6]=0;

            fingerTransformation.data[8]=0;
            fingerTransformation.data[9]=0;
            fingerTransformation.data[10]=1;

            fingerTransformation.data[15]=1.0;
            fingerTransformation.setTranslation(Vector3f(100,30,-30));

            sendData("setCameraPos");

            selectMsbObjectByName("SkeletalActor31");

        }
        break;
        case('s'):
        {
           // Save out
            cout << "Saving contents to file: "<< saveFile.c_str();
            ofstream ofile;
            ifstream ifile(saveFile.c_str());
            ofile.open(saveFile.c_str(), std::ios_base::app);


            // Create a file to save our run data to.
            // Get the current date/time and build file name
            time_t t = time(0);   // get time now
            struct tm * now = localtime( & t );
            ostringstream ss;
            ss  << "Time: "
                << now->tm_hour << ':'
                << now->tm_min << ':'
                << now->tm_sec
                << endl;
             ofile << ss.str();

            ofile << "Test Mode: " << testMode;


            ofile << "\n----------\n";
            ofile <<"["<<fingerTransformation.data[0]<<"]";
            ofile <<"["<<fingerTransformation.data[1]<<"]";
            ofile <<"["<<fingerTransformation.data[2]<<"]";
            ofile <<"["<<fingerTransformation.data[3]<<"]";
            ofile << endl;
            ofile <<"["<<fingerTransformation.data[4]<<"]";
            ofile <<"["<<fingerTransformation.data[5]<<"]";
            ofile <<"["<<fingerTransformation.data[6]<<"]";
            ofile <<"["<<fingerTransformation.data[7]<<"]";
            ofile << endl;
            ofile <<"["<<fingerTransformation.data[8]<<"]";
            ofile <<"["<<fingerTransformation.data[9]<<"]";
            ofile <<"["<<fingerTransformation.data[10]<<"]";
            ofile <<"["<<fingerTransformation.data[11]<<"]";
            ofile << endl;
            ofile <<"["<<fingerTransformation.data[12]<<"]";
            ofile <<"["<<fingerTransformation.data[13]<<"]";
            ofile <<"["<<fingerTransformation.data[14]<<"]";
            ofile <<"["<<fingerTransformation.data[15]<<"]";

            ofile << "\n----------\n";
            ofile << endl;
            ofile.close();
        }
        break;
    }

}

void testApp::keyReleased(int key){

    input->keyUp(key,mouseX,mouseY);
    input->specialKeyUp(key,mouseX,mouseY);

    // change to select cameras by selecting the camera from the data manager class that has a start time
    // closest the the value for playheadFrame.
    // set that camera as activeCam, and run this OSC message when it is selected
    if (key>'0' && key<='4'){
        selectedCamera=key-'1';

        ofxOscMessage myMessage;

        string oscAddress = "/selectActorByName";
        string sendString="Camera";
        sendString+=key;

        cout << "selected camera " << selectedCamera << "using string: " << sendString << endl;

        myMessage.addStringArg(sendString);
        myMessage.setAddress(oscAddress);

        osc_senderMSB.sendMessage(myMessage);

        //send switch camera to processing
        osc_senderProcessing.sendMessage(myMessage);


        ofxOscMessage myMessageSwitch;

        oscAddress = "/switchCameraToSelected";
        myMessageSwitch.setAddress(oscAddress);
        osc_senderMSB.sendMessage(myMessageSwitch);
    }


    switch(key) { // TODO: replace for big ol' imputs.
        case('a'):
        {

            manager.addCamera(playheadFrame);
            sendData("cameraAdded");
            sendData("activeCamera");
        }

            break;
        case('r'):
        {

            manager.removeCamera();
            manager.updateActiveCamera(playheadFrame);
            sendData("activeCamera");
        }
            break;
    }

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

    input->moveMouse(x,y);

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

    input->dragMouse(x,y);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

    input->pressedMouse(button,0,x,y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

    input->pressedMouse(button,1,x,y);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}

void testApp::trigger(Actor* other){

}

//--------------------------------------------------------------
// hand tracking stuff
//--------------------------------------------------------------
