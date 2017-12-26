#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){


	ofSetVerticalSync(true);
	//ofSetFullscreen(true);

	_param=new Param();

	for(int i=0;i<3;++i){
		_timer_blink[i]=FrameTimer(1200,300*i);
		_timer_blink[i].setContinuous(true);
		
		_timer_roll[i]=FrameTimer(_param->_time_roll_win+100*i,50*i);

		_pos_roll[i]=floor(ofRandom(_param->_mprize));
		_vel_roll[i]=ofRandom(1,2)*.002;
	}

	//ofAddListener(_timer_roll[2].finish_event,this,&ofApp::onRollEnd);

	_timer_final=FrameTimer(_param->_time_final);
	//ofAddListener(_timer_final.finish_event,this,&ofApp::onFinalEnd);


	_cur_millis=ofGetElapsedTimeMillis();


	_img_sleep.load("00_pull.png");
	_img_hint.load("00_pull_text.png");
	_img_lose.load("13_unlucky.png");

	//setup serial
	_serial_balloon.setup(_param->_port_balloon,9600);
	//_serial_light.setup(_param->_port_light,9600);
	for(int i=0;i<_param->_port_light.size();++i){
		ofSerial* serial_=new ofSerial();
		serial_->setup(_param->_port_light[i],9600);
		_serial_light.push_back(serial_);
	}

	_serial_walk.setup(_param->_port_walk,9600);

	//setup mask
	_mask.allocate(ofGetWidth(),ofGetHeight());
	_mask.begin();
		ofClear(200,255);
		ofDisableAlphaBlending();
		ofSetCircleResolution(100);
		
		ofSetColor(0,0);
		for(int i=0;i<3;++i){			
			ofDrawCircle(CircleX+(CircleMargin+CircleRad)*i+CircleRad/2,CircleY+CircleRad/2,CircleRad/2);
		}
	_mask.end();
	
	ofAddListener(_http_util.newResponseEvent,this,&ofApp::httpResponse);
	_http_util.start();

	_wait_print=false;
	_got_prize=false;


	_bgm_sleep.load("sound/bgm.wav");
	_bgm_sleep.setLoop(true);
	_bgm_sleep.play();
	

	_bgm_roll.load("sound/roll_loop.wav");
	_bgm_roll.setLoop(true);
	_bgm_roll.play();

	_sound_london.load("sound/announcement.mp3");
	_sound_win.load("sound/win.wav");
	_sound_lose.load("sound/lose.wav");
	_sound_trigger.load("sound/start.wav");
	
	_timer_bgm=FrameTimer(3000);


	//connect
	_udp.Create();
	_udp.Connect("127.0.0.1",11999);
	_udp.SetNonBlocking(true);



	changeMode(LMODE::SLEEP);
	

}

//--------------------------------------------------------------
void ofApp::update(){


	updateSerial();

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
			if(_timer_roll[2].val()==1){
				sendPrint(_print_text);
				_wait_print=false;
				
				changeMode(LMODE::FINAL);
			}
		//	cout<<endl;
			break;
		case FINAL:			
			for(int i=0;i<3;++i){
				if(_got_prize>-1) _pos_roll[i]=_got_prize;
				_timer_blink[i].update(dm);
			}
			_timer_final.update(dm);			
			if(_timer_final.val()==1){
				changeMode(LMODE::SLEEP);
				_timer_final.reset();
			}
			break;
	}
	
	
	ofSoundUpdate();
	updateBgm(dm);

	ofSetWindowTitle(ofToString(ofGetFrameRate()));

}

void ofApp::updateSerial(){

	if(_serial_walk.isInitialized() && _serial_walk.available()){

		vector<string> val=readSerialString(_serial_walk,'#');
		if(val.size()<1) return;
		ofLog()<<"serial read: "<<val[0];

	
		if(val[0]=="go"){
			if(_mode==LMODE::SLEEP) changeMode(LMODE::ROLL);
		}	
	}
}

void ofApp::changeMode(LMODE set_){

	switch(set_){
		case SLEEP:
			for(int i=0;i<3;++i){
				_timer_blink[i].restart();
				_vel_roll[i]=ofRandom(1,2)*.001;
			}
			_mode=set_;
			sendLight(LMODE::SLEEP);
			_timer_bgm.restart();			
			
			break;
		case ROLL:
			_sound_trigger.play();
			getPrize();
			sendLight(LMODE::ROLL);

			break;
		case FINAL:
			_mode=set_;

			if(_got_prize>-1){
				sendLight(LMODE::WIN);
				if(_got_prize==0){
					_sound_london.play();
					_timer_final.setDue(40000+_param->_time_final);
				}else{
					_sound_win.play();
					_timer_final.setDue(_param->_time_final);
				}
				
				for(int i=0;i<3;++i){
					_pos_roll[i]=_got_prize;
					_src_roll[i]=_dest_roll[i]=0;					
				}

			}else{
				sendLight(LMODE::LOSE);
				_sound_lose.play();
			}

			for(int i=0;i<3;++i){
				_timer_roll[i].reset();
				_timer_blink[i].restart();
			}
			_timer_final.setDue(_param->_time_final);
			_timer_final.restart();

			_timer_bgm.reset();		

			break;
	}
	
}


void ofApp::onRollEnd(int &data){
	//if(_mode==LMODE::ROLL){
	//		
 // 		if(_wait_print){
	//		sendPrint(_print_text);
	//		_wait_print=false;
	//	}

	//	changeMode(LMODE::FINAL);
	//}
}
void ofApp::onFinalEnd(int &data){
	/*changeMode(LMODE::SLEEP);

	_timer_final.reset();*/
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
			
				if(_timer_blink[i].num()%2==1){
					
					ofPushStyle();
					ofSetColor(255,255.0*_timer_blink[i].valFade());
						if(_got_prize>-1) drawInCircle(i,_param->_prize_img_name[index_]);
						else drawInCircle(i,_img_lose);
					ofPopStyle();
				}
			}
			break;
	}

	
	
	
	_mask.draw(0,0);

	/*for(int i=0;i<3;++i){	
		ofDrawBitmapString(ofToString(_pos_roll[i]),CircleX+(CircleMargin+CircleRad)*i,CircleY-20);
	}*/
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
	
	img_.draw(CircleX+(CircleMargin+CircleRad)*index_,CircleY+CircleRad*part_,CircleRad,CircleRad);
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case ' ':
			if(_mode==LMODE::SLEEP) changeMode(LMODE::ROLL);
			break;
		case 'f':
		case 'F':
			ofToggleFullscreen();
			break;
		case 't':
			int i=floor(ofRandom(_param->_prize_print.size()));
			sendPrint(_param->_prize_print[i]);
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
				
				//cout<<"src= "<<_src_roll[i]<<"  delta= "<<_dest_roll[i]
				//	<<"dest= "<<_src_roll[i]+_dest_roll[i]<<endl;

				
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
		_wait_print=true;
		_print_text=_param->_xmas_string;

		_got_prize=-1;
	}

	for(int i=0;i<3;++i){
		_timer_roll[i].setDue(((_got_prize>-1)?_param->_time_roll_win:_param->_time_roll_lose)
								+RollDelay*i);
		
		_timer_roll[i].restart();
	}
	_mode=LMODE::ROLL;
	
}

void ofApp::sendPrint(wstring prize_){

	/*std::ofstream f(_param->_print_folder+ofGetTimestampString(), ios_base::binary);
	f<<_param->ws2utf8(prize_);
	f.close();*/
	string str=_param->ws2utf8(prize_);
	_udp.Send(str.c_str(),str.length());

}

void ofApp::sendBalloon(bool up_){

	if(up_){
		_serial_balloon.writeByte('1');
	}else{
		_serial_balloon.writeByte('0');
	}
}
void ofApp::sendLight(LMODE mode_){
	switch(mode_){
		case SLEEP:
			for(int i=0;i<_serial_light.size();++i)
				_serial_light[i]->writeByte('c');
			break;
		case ROLL:
			for(int i=0;i<_serial_light.size();++i)
				_serial_light[i]->writeByte('e');
			break;
		case WIN:
			for(int i=0;i<_serial_light.size();++i)
				_serial_light[i]->writeByte('f');
			break;
		case LOSE:
			for(int i=0;i<_serial_light.size();++i)
				_serial_light[i]->writeByte('b');
			break;	
	}
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
		//setPrize("noprize");

	}else{
		ofLog()<<"something wrong...";
	}

}



void ofApp::updateBgm(float dt_){
	
	_timer_bgm.update(dt_);
	
	
	float p=_timer_bgm.val();
	switch(_mode){
		case SLEEP:
			_bgm_sleep.setVolume(1.0);
			_bgm_roll.setVolume(0.0);
			break;
		case ROLL:
			p=_timer_roll[2].val();
			if(p<=.1){
				_bgm_sleep.setVolume(1.0-p*10);
				_bgm_roll.setVolume(p*10);
			}else if(p>=.9) _bgm_roll.setVolume(1.0-10*(p-.9));		
			break;
		case FINAL:
			if(!_timer_bgm.isStart()){

				//if(_got_prize>0) ofLog()<<_sound_win.getPosition();

				if((_got_prize==0 && _sound_london.getPosition()>=0.95)
					||(_got_prize>0 && _sound_win.getPosition()>=0.95)
					||(_got_prize<0 && _sound_lose.getPosition()>=0.95)){
					_timer_bgm.restart();
				}
			}
			_bgm_sleep.setVolume(p);			
			break;
	}

}
