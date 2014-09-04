#include "Configurator.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

void Configurator::loadConfigFile(
		std::string filename,
		bool checkExistence )
{
	std::ifstream file( filename );
	while( file.good() )
	{
		std::string parameterName, value;
		//Name, equal, value;
		std::getline(file, parameterName, '=');
		std::getline(file, value);
		parameterName.erase(
				std::remove_if( parameterName.begin(), parameterName.end(), ::isspace ),
				parameterName.end() );
		value.erase( std::remove_if( value.begin(), value.end(), ::isspace ), value.end() );
		if( parameterName.empty() || value.empty() )
		{
			std::cerr << "Error: Config file ill-formated: "
				<< parameterName << " = " << value << std::endl;
		}
		else if( value.front() == '"' && value.back() == '"' )
		{
			std::string strValue = value.substr(1,value.size()-2);
			stringParams[parameterName] = strValue;
		}
		else
		{
			intParams[parameterName] = std::stoi( value );
		}
	}
}

int Configurator::getIntParameter( std::string parName )
{
	return intParams[parName];
}

std::string Configurator::getStringParameter( std::string parName )
{
	return stringParams[parName];
}
