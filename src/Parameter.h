#pragma once
#ifndef PARMAMETR_H
#define PARAMETER_H


#include "ofMain.h"
#include "ofxXmlSettings.h"

class Param{

	static Param* _instance;	
	
public:
	static string ParamFilePath;	
    
	vector<string> _prize_id;
	vector<ofImage> _prize_img;
	vector<string> _port;

	string _lottery_url;


	int _mprize;
	int _mrolling;

	Param(){
		readParam();

	}
	static Param* GetInstance(){
		if(!_instance)
			_instance=new Param();
		return _instance;
	}
	void readParam(){


		ofxXmlSettings _param;
		bool file_exist=true;
		if(_param.loadFile(ParamFilePath) ){
			ofLog()<<ParamFilePath<<" loaded!";
		}else{
			ofLog()<<"Unable to load xml check data/ folder";
			file_exist=false;
		}


		_port.clear();
	
        int mport=_param.getNumTags("PORT");
		for(int i=0;i<mport;++i){
			_port.push_back(_param.getValue("PORT","",i));
		}

		_mprize=_param.getNumTags("IMG");
		for(int i=0;i<_mprize;++i){
			_prize_id.push_back(_param.getAttribute("IMG","id","",i));		

			ofImage img_;
			img_.load(_param.getValue("IMG","",i));
			_prize_img.push_back(img_);
		}
		
		_lottery_url=_param.getValue("LOTTERY_URL","");

		_mrolling=2;

		if(!file_exist) saveParameterFile();

	
	}
	void saveParameterFile(){


		ofxXmlSettings _param;
      
        for(int i=0;i<_port.size();++i){
			_param.setValue("PORT",_port[i],i);
		}
		for(int i=0;i<_prize_id.size();++i){
			_param.setValue("IMG",_prize_id[i],i);						
		}
		
		_param.setValue("LOTTERY_URL","");

		_param.save(ParamFilePath);


	}


};





#endif
