#pragma once

#include "ofMain.h"
#include "FrameTimer.h"
#include "Parameter.h"

#include "ofxHttpUtils.h"
#include "ofxJSON.h"

class ofApp : public ofBaseApp{

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
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		Param* _param;
		float _cur_millis;

		enum LMODE {SLEEP,ROLL,FINAL};
		LMODE _mode;
		
		FrameTimer _timer_blink[3];		
		FrameTimer _timer_roll[3];		
		FrameTimer _timer_final;

		float _dest_roll[3];
		float _src_roll[3];
		float _pos_roll[3];

		float _vel_roll[3];
		
		void changeMode(LMODE set_);

		void onRollEnd(int &data);
		void onFinalEnd(int &data);

		void drawCircles();
		void drawInCircle(int index_,ofImage& img_);
		void drawInCircle(int index_,ofImage& img_,float part_);

		ofImage _img_hint;
		ofImage _img_sleep;

		void getPrize();
		void setPrize(string prize_);
		
		void sendPrint(wstring prize_);

		ofSerial _serial;
		void sendBalloon(bool up_);

		ofFbo _mask;
		
		
		ofxHttpUtils _http_util;
		void httpResponse(ofxHttpResponse & response);
		
		int _got_prize;
		bool _wait_print;
		wstring _print_text;
};
