#pragma once
#ifndef PARMAMETR_H
#define PARAMETER_H

#include <codecvt>
#include "ofMain.h"
#include "ofxXmlSettings.h"

class Param{

	static Param* _instance;	
	
public:
	static string ParamFilePath;	
    
	vector<string> _prize_id;
	vector<ofImage> _prize_img;
	vector<ofImage> _prize_img_name;
	vector<wstring> _prize_print;

	vector<string> _port;
	
	string _lottery_url;
	string _print_folder;


	int _mprize;
	int _mrolling;


	int _time_roll_win;
	int _time_roll_lose;

	int _time_final;

	string _port_light;
	string _port_balloon;
	string _port_walk;

	Param(){
		readParam();

	}
	static Param* GetInstance(){
		if(!_instance)
			_instance=new Param();
		return _instance;
	}
	void readParam(){

		_prize_id.clear();
		_prize_img.clear();
		_prize_img_name.clear();
		_prize_print.clear();
		
		_port.clear();
		


		ofxXmlSettings _param;
		bool file_exist=true;
		if(_param.loadFile(ParamFilePath) ){
			ofLog()<<ParamFilePath<<" loaded!";
		}else{
			ofLog()<<"Unable to load xml check data/ folder";
			file_exist=false;
		}


	
		
        int mport=_param.getNumTags("PORT");
		for(int i=0;i<mport;++i){
			_port.push_back(_param.getValue("PORT","",i));
		}

		

		_mprize=_param.getNumTags("IMG");
		for(int i=0;i<_mprize;++i){
			_prize_id.push_back(_param.getAttribute("IMG","id","",i));	
			string str_=_param.getAttribute("IMG","print","",i);			
			//_prize_print.push_back(wstring(str_.begin(),str_.end()));
			_prize_print.push_back(utf82ws(str_));

			ofImage img_;
			img_.load(_param.getValue("IMG","",i));
			_prize_img.push_back(img_);


			ofImage nimg_;
			string name_=_param.getAttribute("IMG","name","",i);
			if(name_!="") nimg_.load(name_);
			_prize_img_name.push_back(nimg_);

		}
		
		_lottery_url=_param.getValue("LOTTERY_URL","");
		_print_folder=_param.getValue("PRINT_FOLDER","");


		_time_roll_win=_param.getValue("TIME_ROLL_WIN",0);
		_time_roll_lose=_param.getValue("TIME_ROLL_LOSE",0);

		_time_final=_param.getValue("TIME_FINAL",0);

		_port_light=_param.getValue("LIGHT_PORT","");
		_port_walk=_param.getValue("WALK_PORT","");
		_port_balloon=_param.getValue("BALLOON_PORT","");


		_mrolling=2;

		if(!file_exist) saveParameterFile();

	
	}
	string ws2utf8(std::wstring &input){
		std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
		return utf8conv.to_bytes(input);
	}

	wstring utf82ws(std::string &input)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
		return utf8conv.from_bytes(input);
	}

	void saveParameterFile(){


		ofxXmlSettings _param;
      
        for(int i=0;i<_port.size();++i){
			_param.setValue("PORT",_port[i],i);
		}
		for(int i=0;i<_prize_id.size();++i){
			_param.setValue("IMG",_prize_id[i],i);						
		}
		
		_param.setValue("LOTTERY_URL",_lottery_url);
		_param.setValue("PRINT_FOLDER",_print_folder);

		_param.setValue("TIME_ROLL_WIN",_time_roll_win);
		_param.setValue("TIME_ROLL_LOSE",_time_roll_lose);

		_param.setValue("TIME_FINAL",_time_final);

		_param.setValue("LIGHT_PORT",_port_light);
		_param.setValue("WALK_PORT",_port_walk);
		_param.setValue("BALLOON_PORT",_port_balloon);

		_param.save(ParamFilePath);


	}


};





#endif

