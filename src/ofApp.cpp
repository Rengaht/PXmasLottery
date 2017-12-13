#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){


	_param=new Param();

	for(int i=0;i<3;++i){
		_timer_blink[i]=FrameTimer(1200,300*i);
		_timer_blink[i].setContinuous(true);
		
		_timer_roll[i]=FrameTimer(_param->_time_roll+100*i,50*i);

		_pos_roll[i]=floor(ofRandom(_param->_mprize));
		_vel_roll[i]=ofRandom(1,2)*.001;
	}

	ofAddListener(_timer_roll[2].finish_event,this,&ofApp::onRollEnd);

	_timer_final=FrameTimer(_param->_time_final);
	ofAddListener(_timer_final.finish_event,this,&ofApp::onFinalEnd);

	changeMode(LMODE::SLEEP);
	_cur_millis=ofGetElapsedTimeMillis();


	_img_sleep.load("00_pull.png");
	_img_hint.load("00_pull_text.png");

	//setup serial


	//setup mask
	_mask.allocate(ofGetWidth(),ofGetHeight());
	_mask.begin();
		ofClear(200,255);
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

	_wait_print=false;
	_got_prize=false;

}

//--------------------------------------------------------------
void ofApp::update(){

	float dm=ofGetElapsedTimeMillis()-_cur_millis;
	_cur_millis+=dm;

	switch(_mode){
		case SLEEP:
			for(int i=0;i<3;++i){
				_timer_blink[i].update(dm);
				_pos_roll[i]=fmod(_pos_roll[i]+_vel_roll[i],_param->_mprize);
			}
			break;
		case ROLL:
			for(int i=0;i<3;++i){
				_timer_roll[i].update(dm);
				_pos_roll[i]=_src_roll[i]+_dest_roll[i]*_timer_roll[i].valEase();
				//cout<<_pos_roll[i]<<" ";
				//if(i==0) cout<<_pos_roll[i]<<endl;
			}
		//	cout<<endl;
			break;
		case FINAL:
			for(int i=0;i<3;++i){
				_timer_blink[i].update(dm);
			}
			_timer_final.update(dm);
			break;
	}
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

void ofApp::changeMode(LMODE set_){

	switch(set_){
		case SLEEP:
			for(int i=0;i<3;++i) _timer_blink[i].restart();
			_mode=set_;
			
			break;
		case ROLL:
			getPrize();
			break;
		case FINAL:
			for(int i=0;i<3;++i) _timer_blink[i].restart();
			_timer_final.restart();

			_mode=set_;

			break;
	}
	
}


void ofApp::onRollEnd(int &data){
	if(_mode==LMODE::ROLL){
		changeMode(LMODE::FINAL);

		if(_got_prize>-1){
			for(int i=0;i<3;++i) _pos_roll[i]=_got_prize;
		}
		if(_wait_print){
			sendPrint(_print_text);
			_wait_print=false;
		}
	}
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
			for(int i=0;i<3;++i){
				int loop_=_timer_blink[i].num();
				if(_timer_blink[i].num()%4>2){
					
					ofPushStyle();
					ofSetColor(255,255.0*_timer_blink[i].valFade());
						drawInCircle(i,_img_sleep);
					ofPopStyle();
				}
			}
			break;
		case ROLL:
			drawCircles();
			break;
		case FINAL:
			for(int i=0;i<3;++i){
				int index_=(int)(floor(_pos_roll[i]))%_param->_mprize;
				drawInCircle(i,_param->_prize_img[index_]);
			
				if(_got_prize>-1 && _timer_blink[i].num()%2==1){

					ofPushStyle();
					ofSetColor(255,255.0*_timer_blink[i].valFade());
						drawInCircle(i,_param->_prize_img_name[index_]);
					ofPopStyle();
				}
			}
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
			if(_mode==LMODE::SLEEP) changeMode(LMODE::ROLL);
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
	

	
	if(prize_!="noprize"){
		auto it=find(_param->_prize_id.begin(),_param->_prize_id.end(),prize_);
		if(it!=_param->_prize_id.end()){
			auto index=distance(_param->_prize_id.begin(),it);
			int dest_=index;
			for(int i=0;i<3;++i){

				_src_roll[i]=_pos_roll[i];
				_dest_roll[i]=(_param->_mprize-_src_roll[i])
							 +(i+_param->_mrolling)*_param->_mprize+dest_;		
				
				cout<<"src= "<<_src_roll[i]<<"  delta= "<<_dest_roll[i]
					<<"dest= "<<_src_roll[i]+_dest_roll[i]<<endl;

				
			}

			sendBalloon(true);
			_wait_print=true;
			_print_text=_param->_prize_print[dest_];
			_got_prize=dest_;
		}
			

	}else{

		int dest_=floor(ofRandom(_param->_mprize));

		for(int i=0;i<3;++i){
			
			dest_+=floor(ofRandom(_param->_mprize/2));

			_src_roll[i]=_pos_roll[i];
			_dest_roll[i]=(_param->_mprize-_src_roll[i])
						+(i+_param->_mrolling)*_param->_mprize+dest_;		
		}
		sendBalloon(false);
		_wait_print=false;
		_got_prize=-1;
	}

	for(int i=0;i<3;++i){
		_timer_roll[i].restart();
	}
	_mode=LMODE::ROLL;
	
}

void ofApp::sendPrint(wstring prize_){

	if(prize_.size()<1) return;

	std::ofstream f(_param->_print_folder+ofGetTimestampString(), ios_base::binary);
	f<<_param->ws2utf8(prize_);
	f.close();
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
