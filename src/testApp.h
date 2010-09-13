#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxSimpleGuiToo.h"

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		ofVideoGrabber 		cam, vj;
		unsigned char * 	videoMasked, * white, * white2, * black, * alpha;
		ofTexture			output, output_feedback, output_still;
		int 				width, height;
		int                 bOutput;

		bool                testscreen, bSetVjToWhite;

        ofxCvColorImage		camInput, smallCamInput, vjInput, smallVjInput;

        ofImage             blubb;

        ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff, test;

        ofxCvContourFinder 	contourFinder;

		int 				threshold, blur;
		bool				bMakeStill, bLearnBakground, bDrawDebugCircle, bShowFeedback, bShowStill;

		ofPoint             warpPoints[4];
};

#endif
