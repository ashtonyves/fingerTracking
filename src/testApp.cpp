#include "testApp.h"

#include "actor.h"
#include "sliderButton.h"
#include "textInputButton.h"
#include "assignButton.h"
#include "msbLight.h"

#define BUF_SIZE 640*480*4*32


//--------------------------------------------------------------
void testApp::setup(){

    bFullscreen=false;
    bLearnBackground=true;
    cutOffDepth=4096;
    bSetCutoffToZero=false;
    channelMSB=1;
    channelProc=2;
    bufferLength=10;

    selectedCamera=0;
    thresh= 66;

    bFoundMat=false;
    bFingerOut=false;
    bSending=false;

    fistFactor=0;

    offsetVector=Vector3f(400,600,3);

    ofSetFrameRate(25);

    //ipAddress="128.61.22.174"; // Nick's IP
    //ipAddressMSB="192.168.1.3";
    ipAddressMSB="169.254.75.192";
    //ipAddress="169.254.208.35";
    //ipAddressMSB="127.0.0.1";
    //ipAddressMSB="143.215.199.192";

    ipAddressProc="127.0.0.1";
    //ipAddress="192.168.1.24";

    // osc_senderMSB.setup(ipAddressMSB,3184+channelMSB);
    // osc_senderProcessing.setup(ipAddressProc,3184+channelProc);

    osc_senderMSB.setup(ipAddressMSB,31841);
    osc_senderProcessing.setup(ipAddressProc,31842);

    msbSetup();

    //interface setup
    interfaceSetup();

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
}

void testApp::msbSetup(){

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
    tiBut->location.x=10;
    tiBut->location.y=550;
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
    tiBut->location.x=100;
    tiBut->location.y=650;
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


}


void testApp::registerProperties(){

   createMemberID("OFFSETVECTOR",&offsetVector,this);
   createMemberID("THRESH",&thresh,this);
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

    fingerStart*=0;
    fingerEnd*=0;

    bSending=false;
    bFoundMat=false;
    bFingerOut=false;


	ofBackground(100, 100, 100);
	kinect.update();

    renderer->update();

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
    contourFinder.findContours(grayImage, 1000,640*480/2 , 5, false);	// find holes


    Matrix4f idMatrix;
    idMatrix.identity();


    bFoundMat=findFingerMatrix();
    bFingerOut=findFinger();

    //I made these changes to test data transfer, please remove comments!
    if (bFoundMat && bFingerOut){
        sendData();
        bSending=true;
    }


}


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
                ///Things flip to longer than wide here!
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
                ///Things flip to longer than wide here!
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
            zA=pointFinger- pointHand;
            //zA.y=0.0;
            zA.normalize();

            zA.z=-zA.z;
            zA=-zA;

            Vector3f yA=Vector3f(0,-1,0);
            Vector3f xA=zA.crossProduct(yA);
            //xA.y=0.0;
            xA.normalize();

            yA=zA.crossProduct(xA);

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

            fingerTransformation.setTranslation(Vector3f(edge->x*offsetVector.z -offsetVector.x,-(depthCtroid-1000.0),(-edge->y* offsetVector.z)+offsetVector.y  )*0.1);

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



            free(edge);
            free(ctroid);

            return true;
    }

    return false;
}


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

                     if (wb[i*640+j]>0)   bIn=true;

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

void testApp::sendData(){


        ofxOscMessage myMessage;


        cout<< "I am sending stuff to:" << ipAddressMSB << endl;

        //fingerTransformation.transpose();

        string oscAddress = "/setPropertyForSelected/string/matrix4f";
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

        myMessage.setAddress(oscAddress);

        osc_senderMSB.sendMessage(myMessage);

        //send to Processing
        osc_senderProcessing.sendMessage(myMessage);

    //cout << "sending..." << fingerTransformation << endl;
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
        grayImage.draw(660,50,320,240);

        glPushMatrix();

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
}

//--------------------------------------------------------------
void testApp::exit(){


}

//--------------------------------------------------------------
void testApp::keyPressed (int key){

    input->normalKeyDown(key,mouseX,mouseY);
    input->specialKeyDown(key,mouseX,mouseY);

}

void testApp::keyReleased(int key){

    input->keyUp(key,mouseX,mouseY);
    input->specialKeyUp(key,mouseX,mouseY);

    if (key>'0' && key<='4'){
        selectedCamera=key-'1';

        ofxOscMessage myMessage;

        string oscAddress = "/selectActorByName";
        string sendString="Camera";
        sendString+=key;

        cout << "selected camera " << selectedCamera << "using sting: " << sendString << endl;

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
