#ifndef SIMULATORLOG_H
#define SIMULATORLOG_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;


namespace Utils {

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/** Class for storage of errors and debug messages*/
class Log{
  
public:
  //Constructors
  Log(string ErrorPath, string LogPath,int level);
  ~Log();  	
  
  //Auxiliar methods
  void error(string error);
  void debug(string log);
  void debug(string log,int levelmsg);
  
  //set and gets methods
  void setLevel(int& theValue);
  int getlevel();
  void setVerboseMode(bool theValue);
  bool getverboseMode() const;
	

private:
  //Variables that indicate the paths stuff
  string ErrorPath; /**<indicates the path of the file where the errors will be stored */
  string DebugPath; /**<indicates the path of the file where the logs will be stored */
  int level; /**<Indicates the debug level , the errors are always stored. Logs are displayed if the level is greater than 1 */
  
  ofstream ErrorFile; /**< Strem for the error file */
  ofstream LogFile;/**< Stream for the log file  */
  
  bool verboseMode;/**< all the stuff is shown in the STDIO*/
  int line; /**< The current line in the debug file */
};

}

#endif
