#ifndef Configurator_H
#define Configurator_H

#include <string>
#include <unordered_map>

class Configurator
{
	private:
	std::unordered_map< std::string, int > intParams;
	std::unordered_map< std::string, std::string > stringParams;
//	std::string configFileName;

	public:
//	Configurator();
//	Configurator( Configurator& other );
//	~Configurator();

	void loadConfigFile( std::string filename, bool checkExistence = false );
//	void addParameter( std::string longName, char acronym, T defaultVal, bool required );
	void addParameter( std::string longName, int defaultVal );
	void addParameter( std::string longName, std::string defaultVal );
	void addParameter( std::string longName );

	void setParameter( std::string parName, int value );
	void setParameter( std::string parName, std::string value );

	int getIntParameter( std::string parName );
	std::string getStringParameter( std::string parName );

};

#endif // Configurator_H
