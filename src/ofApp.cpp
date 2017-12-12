#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){


	_param=new Param();

	_timer_blink=FrameTimer(500);
	_timer_blink.setContinuous(true);

	_timer_roll[0]=FrameTimer(10000);
	_timer_roll[1]=FrameTimer(10200);
	_timer_roll[2]=FrameTimer(10400);
	ofAddListener(_timer_roll[2].finish_event,this,&ofApp::onRollEnd);

	_timer_final=FrameTimer(10000);
	ofAddListener(_timer_final.finish_event,this,&ofApp::onFinalEnd);

	changeMode(LMODE::SLEEP);
	_cur_millis=ofGetElapsedTimeMillis();


	_img_sleep.load("00_pull.png");
	_img_hint.load("00_pull_text.png");

	//setup serial


	//setup mask
	_mask.allocate(ofGetWidth(),ofGetHeight());
	_mask.begin();
		ofClear(255,255);
		ofDisableAlphaBlending();
		ofSetCircleResolution(100);
		float rad_=ofGetWidth()/3*.8;
		float px_=ofGetWidth()/3*.1;
		float py_=50;
		ofSetColor(0,0);
		for(int i=0;i<3;++i) ofDrawCircle((px_*2+rad_)*i+px_+rad_/2,py_+rad_/2,rad_/2);
		
	_mask.end();
	
	ofAddListener(_http_util.newResponseEvent,this,&ofApp::httpResponse);
	_http_util.start();

}

//--------------------------------------------------------------
void ofApp::update(){

	float dm=ofGetElapsedTimeMillis()-_cur_millis;
	_cur_millis+=dm;

	switch(_mode){
		case SLEEP:
			_timer_blink.update(dm);
			for(int i=0;i<3;++i) _pos_roll[i]+=.01;

			break;
		case ROLL:
			for(int i=0;i<3;++i){
				_timer_roll[i].update(dm);
				_pos_roll[i]=(float)_dest_roll[i]*_timer_roll[i].valEase();
				//if(i==0) cout<<_pos_roll[i]<<endl;
			}
			break;
		case FINAL:
			_timer_final.update(dm);
			break;
	}
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

void ofApp::changeMode(LMODE set_){

	switch(set_){
		case SLEEP:
			_timer_blink.restart();
			break;
		case ROLL:
			for(int i=0;i<3;++i) _timer_roll[i].restart();
			getPrize();
			break;
		case FINAL:
			_timer_final.restart();
			break;
	}
	_mode=set_;
}


void ofApp::onRollEnd(int &data){
	changeMode(LMODE::FINAL);
}
void ofApp::onFinalEnd(int &data){
	changeMode(LMODE::SLEEP);
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofBackground(255,252,243);

	switch(_mode){
		case SLEEP:
			drawCircles();
			for(int i=0;i<3;++i) 
				if(_timer_blink.num()%4>=(i+1)) drawInCircle(i,_img_sleep);
			
			break;
		case ROLL:
			drawCircles();
			break;
		case FINAL:
			for(int i=0;i<3;++i) 
				drawInCircle(i,_param->_prize_img[_dest_roll[i]%_param->_mprize]);
			break;
	}

	
	
	_mask.draw(0,0);
}
void ofApp::drawCircles(){
	for(int i=0;i<3;++i){

		int ind_=ceil(_pos_roll[i]);
		float pos_=ind_-_pos_roll[i];
		ind_%=_param->_mprize;

		drawInCircle(i,_param->_prize_img[((ind_-1)+_param->_mprize)%_param->_mprize],pos_-1);
		drawInCircle(i,_param->_prize_img[ind_],pos_);
		drawInCircle(i,_param->_prize_img[(ind_+1)%_param->_mprize],pos_+1);
	}

}

void ofApp::drawInCircle(int index_,ofImage& img_){
	drawInCircle(index_,img_,0);
}
void ofApp::drawInCircle(int index_,ofImage& img_,float part_){
	
	float rad_=ofGetWidth()/3*.8;
	float px_=ofGetWidth()/3*.1;
	float py_=50;

	img_.draw((px_*2+rad_)*index_+px_,py_+rad_*part_,rad_,rad_);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case 'a':
			changeMode(LMODE::ROLL);
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


void ofApp::getPrize(){

	//send request
	ofxHttpForm form_;
	form_.action=_param->_lottery_url;
	form_.method=OFX_HTTP_GET;
	_http_util.addForm(form_);


	/*for(int i=0;i<3;++i){
		int dest_=floor(ofRandom(_param->_mprize));
		_dest_roll[i]=(i+1)*_param->_mrolling*_param->_mprize+dest_;
	}*/
}
void ofApp::setPrize(string prize_){
	for(int i=0;i<3;++i){
		int dest_=floor(ofRandom(_param->_mprize));
		if(prize_!="noprize"){
			auto it=find(_param->_prize_id.begin(),_param->_prize_id.end(),prize_);
			if(it!=_param->_prize_id.end()){
				auto index=distance(_param->_prize_id.begin(),it);
				dest_=index;
			}
			sendBalloon(true);
			sendPrint(prize_);

		}else{
			sendBalloon(false);
		}
		_dest_roll[i]=(i+1)*_param->_mrolling*_param->_mprize+dest_;
	}
	
}

void ofApp::sendPrint(string prize_){
	

}

void ofApp::sendBalloon(bool up_){


}

void ofApp::httpResponse(ofxHttpResponse & response){
	/*ofLog()<<ofToString(response.status)<<" : "
		   <<(string)response.responseBody;*/

	ofxJSONElement json_;
	json_.parse(response.responseBody);
	if(!json_["error"].empty()){
		ofLog()<<json_["error"].asString();
	}else if(!json_["prize"].empty()){
		ofLog()<<"get prize: "<<json_["prize"].asString();
		setPrize(json_["prize"].asString());
	}else{
		ofLog()<<"something wrong...";
	}

}
