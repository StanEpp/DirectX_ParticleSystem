#pragma warning (disable:4800)
#include "INIParser.h"
#include <boost/lexical_cast.hpp>
#include <fstream>

//Functions which converts different numbers to strings or vice versa as helping-functions for the INIparser
std::string INIParser::numberToString(float l_value){
	std::string temp;

	try{
		temp = boost::lexical_cast<std::string>(l_value);
	}catch(boost::bad_lexical_cast &e){
		return "boost::lexical_cast error";
	}

	return temp;
}

std::string INIParser::numberToString(int l_value){
	std::string temp;

	try{
		temp = boost::lexical_cast<std::string>(l_value);
	}catch(boost::bad_lexical_cast &e){
		return "boost::lexical_cast error";
	}

	return temp;
}


std::string INIParser::numberToString(bool l_value){
	std::string temp;

	if(l_value == true){
		temp = "1";
		return temp;
	}else{
		temp = "0";
		return temp;
	}

}

std::string INIParser::numberToString(unsigned char l_value){
	std::string temp;

	try{
		temp = boost::lexical_cast<std::string>((int)(l_value));
	}catch(boost::bad_lexical_cast &e){
		return "boost::lexical_cast error";
	}

	return temp;
}

float INIParser::stringToFloat(std::string l_str){
	float temp;

	try{
		temp = boost::lexical_cast<float>(l_str);
	}catch(boost::bad_lexical_cast &e){
		return -1;
	}

	return temp;
}

int INIParser::stringToInt(std::string l_str){
	int temp;

	try{
		temp = boost::lexical_cast<int>(l_str);
	}catch(boost::bad_lexical_cast &e){
		return -1;
	}

	return temp;
}

void INIParser::loadINI(){
	if(!load()){
		MessageBoxA(NULL, "INI-File could not be loaded!\nDefault values will be applied.", "Error", MB_OK | MB_ICONWARNING | MB_APPLMODAL); 
		loadDefault();
		writeINI();
	}
}

//Loads all the default Values if desired
void INIParser::loadDefault(){
	_screenWidth	= 1024;
	_screenHeight	= 768;
	_windowed		= true;
	_maxFramerate	= 70;

	_maxParticle	= 100000;
	_quadLength		= 0.1f;
	_initRadius		= 50;

	_velocityTranslate	= 20.0f;
	_velocityRotate		= 0.5f;
}

//Loads all the values from the .ini-file
bool INIParser::load(){

	int windowed	= 0;

	_screenWidth	= GetPrivateProfileInt("Window", "width", -1, _filepath);
	_screenHeight	= GetPrivateProfileInt("Window", "height", -1, _filepath);
	windowed		= GetPrivateProfileInt("Window", "windowed", -1, _filepath);
	_maxFramerate	= GetPrivateProfileInt("Window", "maxFPS", -1, _filepath);

	_maxParticle	= GetPrivateProfileInt("Particles", "maxParticle", -1, _filepath);
	_quadLength		= static_cast<float>(GetPrivateProfileInt("Particles", "quadLength", -1, _filepath));
	_initRadius		= GetPrivateProfileInt("Particles", "iniRadius", -1, _filepath);

	_velocityTranslate	= static_cast<float>(GetPrivateProfileInt("Camera", "translation_velocity", -1, _filepath));
	_velocityRotate		= static_cast<float>(GetPrivateProfileInt("Camera", "rotation_velocity", -1, _filepath));

	if(_screenWidth == -1 || _screenHeight == -1 || windowed == -1 || _maxParticle == -1 || _quadLength == -1 || _velocityTranslate == -1 || _velocityRotate == -1 || _maxFramerate == -1 ||
		_initRadius == -1){
		return false;
	} else {
		_windowed = windowed;
		_quadLength /= 1000.0f;
		_velocityTranslate /= 1000.0f;
		_velocityRotate	/= 1000.0f;
		return true;
	}
}

//Writes all the default values in the .ini-file
bool INIParser::writeINI(){

	if(WritePrivateProfileString("Window", "width", numberToString(1024).c_str(), _filepath) &&
		WritePrivateProfileString("Window", "height", numberToString(768).c_str(), _filepath) &&
		WritePrivateProfileString("Window", "windowed", numberToString(true).c_str(), _filepath) &&
		WritePrivateProfileString("Window", "maxFPS", numberToString(70).c_str(), _filepath) &&

		WritePrivateProfileString("Particles", "maxParticle", numberToString(100000).c_str(), _filepath) &&
		WritePrivateProfileString("Particles", "quadLength", numberToString(0.1f*1000.0f).c_str(), _filepath) &&
		WritePrivateProfileString("Particles", "iniRadius", numberToString(50).c_str(), _filepath) &&


		WritePrivateProfileString("Camera", "translation_velocity", numberToString(20.0f*1000.0f).c_str(), _filepath) &&
		WritePrivateProfileString("Camera", "rotation_velocity", numberToString(0.5f*1000.0f).c_str(), _filepath)
		){
		
			return true;
	} else {
		MessageBoxA(NULL, "INI-File could not be written!", "Error", MB_OK | MB_ICONWARNING | MB_APPLMODAL);
		return false;
	}

}