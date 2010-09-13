#include "testApp.h"





//--------------------------------------------------------------
void testApp::setup(){

    //*****************
    // MAIN
    //*****************
    ofSetVerticalSync(true);

	width 		= 720;	// try to grab at this size.
	height 		= 576;

    //*****************
    // VJ
    //*****************
    vj.setVerbose(true);
	vj.setDeviceID(1);
	vj.initGrabber(width,height);

    vjInput.allocate(720,576);

    smallVjInput.allocate(320,240);

    //*****************
    // CAMERA
    //*****************
	cam.setVerbose(true);
	cam.setDeviceID(0);
	cam.initGrabber(width,height);

	camInput.allocate(720,576);

    //*****************
    // OPENCV
    //*****************
    smallCamInput.allocate(320,240);

	grayImage.allocate(320,240);
	grayBg.allocate(320,240);
	grayDiff.allocate(320,240);

    test.allocate(320,240);

	bLearnBakground = true;
	threshold = 80;
	blur = 0;
    //*****************
    // MASKING
    //*****************
    videoMasked	= new unsigned char[320*240*3];

    //*****************
    // WHITE SCREEN
    //*****************
	white	= new unsigned char[720*576*3];
    for (int i = 0; i < 1244160; i++){
            white[i] = 255;
		}

    //*****************
    // BLACK SCREEN
    //*****************
	black	= new unsigned char[720*576*3];
    for (int i = 0; i < 1244160; i++){
            black[i] = 0;
		}

    //*****************
    // OUTPUT
    //*****************
	output.allocate(720,576, GL_RGB);
	output_feedback.allocate(320,240, GL_RGB);
	output_still.allocate(320,240, GL_RGB);

    //*****************
    // GUI
    //*****************
    warpPoints[0].x = 0;
    warpPoints[0].y = 0;
    warpPoints[1].x = 320;
    warpPoints[1].y = 0;
    warpPoints[2].x = 320;
    warpPoints[2].y = 240;
    warpPoints[3].x = 0;
    warpPoints[3].y = 240;

    gui.addTitle("Options");
    gui.config->gridSize.set(340,0,0);
    gui.addFPSCounter();
    gui.addSlider("Threshold",threshold,0,255);
    gui.addSlider("Blur",blur,0,100);
    gui.addToggle("Testscreen",testscreen);
    gui.addToggle("Set VJ to White",bSetVjToWhite);
    gui.addToggle("Debug Circle",bDrawDebugCircle);
    gui.addQuadWarper("Warp", smallCamInput, warpPoints).newColumn = true;
    gui.addContent("Gray Image", grayImage,320);
    gui.addContent("Start Pic", grayBg,320);
    gui.addContent("Difference with Feedback", grayDiff,320);
    gui.addContent("Difference with Feedback MASKED", output_feedback,320);
    gui.addContent("Diffrence Still", test,320).newColumn = true;
    gui.addContent("Diffrence Still MASKED", output_still,320);
    gui.addContent("Output", output,320).newColumn = true;
    gui.addContent("VJ Input", vjInput,320);

    gui.loadFromXML();
    gui.show();

    ofBackground(80,80,80);

    //*****************
    // BOOLS
    //*****************
    testscreen = false;
    bDrawDebugCircle = false;
    bSetVjToWhite = false;
    bOutput = 0;

    bShowFeedback = false;
}


//--------------------------------------------------------------
void testApp::update(){

	//*************************
	// VJ INPUT
	//*************************
	vj.grabFrame();

	if (vj.isFrameNew()) {
        vjInput.setFromPixels(vj.getPixels(),720,576);
        smallVjInput.scaleIntoMe(vjInput);
	}

    //WHITE
    if(bSetVjToWhite) {
        vjInput.setFromPixels(white, 720, 576);
        smallVjInput.scaleIntoMe(vjInput);
    }

	//*************************
	// CAMERA INPUT
	//*************************
    cam.grabFrame();

	if (cam.isFrameNew()){
		camInput.setFromPixels(cam.getPixels(), 720,576);
		smallCamInput.scaleIntoMe(camInput);
        grayImage = smallCamInput;
        grayImage.warpPerspective(warpPoints[0],warpPoints[1],warpPoints[2],warpPoints[3]);

        grayImage.blur(blur);

		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBakground = false;
		}

        if (bMakeStill == true){
            test = grayDiff;
            bMakeStill = false;
		}

        // take the abs value of the difference between background and incoming and then threshold:


        grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);



		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		//contourFinder.findContours(grayDiff, 20, (720*576)/3, 3, false, true);	// find holes

        int totalPixels = 320*240*3;
		unsigned char * pixels = smallVjInput.getPixels();

        //********
        //FEEDBACK
        //********
        alpha = grayDiff.getPixels();
		for (int i = 0; i < totalPixels; i++){
			videoMasked[i] = 0;
			if(alpha[(int)floor(i/3)] != 0) {
                videoMasked[i] = pixels[i];
			}
		}
		output_feedback.loadData(videoMasked, 320, 240, GL_RGB);

        //********
        //STILL
        //********
        alpha = test.getPixels();
		for (int i = 0; i < totalPixels; i++){
			videoMasked[i] = 0;
			if(alpha[(int)floor(i/3)] != 0) {
                videoMasked[i] = pixels[i];
			}
		}
		output_still.loadData(videoMasked, 320, 240, GL_RGB);

	}

	//*************************
	// CAMERA INPUT
	//*************************
        switch(bOutput) {
            case 0:
                output.loadData(black, 720,576, GL_RGB);
                bShowFeedback = false;
                bShowStill = false;
            break;
            case 1:
                output.loadData(camInput.getPixels(), 720,576, GL_RGB);
                bShowFeedback = false;
                bShowStill = false;
            break;
            case 2:
                output.loadData(vjInput.getPixels(), 720,576, GL_RGB);
                bShowFeedback = false;
                bShowStill = false;
            break;
            case 3:
                output.loadData(white, 720,576, GL_RGB);
                bShowFeedback = false;
                bShowStill = false;
            break;
            case 4:
                bShowFeedback = true;
                bShowStill = false;
            break;
            case 5:
                bShowStill = true;
                //may set the still to the feedback texture to avoid jumps
                bShowFeedback = false;
            break;

        }
}

//--------------------------------------------------------------
void testApp::draw(){
    //GUI
    gui.draw();

    //OUTPUT
    if(bShowFeedback) {
        output_feedback.draw(1920,0,720,576);
        //ugly hack for output view
        output_feedback.draw(1383,114,320,260);
    } else if(bShowStill) {
        output_still.draw(1920,0,720,576);
        //ugly hack for output view
        output_still.draw(1383,114,320,260);
    } else {
        output.draw(1920,0,720,576);
    }

    //DEBUG
    if(testscreen) {
            ofSetColor(0xff0000);
            ofRect(1920,0,width/3,height);
            ofSetColor(0x00ff00);
            ofRect(1920+width/3,0,width/3,height);
            ofSetColor(0x0000ff);
            ofRect(1920+2*(width/3),0,width/3,height);
    }

    if(bDrawDebugCircle) {
        ofSetColor(0xffffff);
        ofCircle(mouseX,mouseY,30);
    }
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){

	switch (key){
	    case '0':
            bOutput = 0;
            break;
	    case '1':
            bOutput = 1;
            break;
	    case '2':
            bOutput = 2;
            break;
 	    case '3':
            bOutput = 4;
            break;
 	    case '4':
            bOutput = 5;
            break;
	    case '9':
            bOutput = 3;
            break;
		case 'b':
			bLearnBakground = true;
			break;
		case ' ':
			bMakeStill = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
        case 't':
                if(testscreen) {
                    testscreen = false;
                } else {
                    testscreen = true;
                }
            break;
	}
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}
