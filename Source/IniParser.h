#ifndef _INIPARSER_
#define _INIPARSER_

#include <Windows.h>
#include <vector>

//INIParser class which defines all the structures with the different values for the algorithms
class INIParser{
public:

	int		_screenWidth, _screenHeight;
	bool	_windowed;
	int		_maxFramerate;

	int		_maxParticle;
	int		_initRadius;
	float	_quadLength;

	float	_velocityTranslate;
	float	_velocityRotate;

	void loadINI();
	bool writeINI();
	void loadDefault();
	
	INIParser(char* filepath){_filepath = filepath;}
	~INIParser(){}
private:
	bool load();

	std::string			numberToString(float l_value);
	std::string			numberToString(int l_value);
	std::string			numberToString(bool l_value);
	std::string			numberToString(unsigned char l_value);
	float				stringToFloat(std::string l_str);
	int					stringToInt(std::string l_str);
	char*				_filepath;
};


#endif