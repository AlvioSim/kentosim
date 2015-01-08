#include <utils/log.h>
#include <assert.h>
#include <time.h>
#include <utils/utilities.h>

namespace Utils {

/***************************
  Constructor and destructors
***************************/

/**
 * The constructor for the class 
 * @param ErrorPath indicates the path of the file where the errors will be stored
 * @param DebugPath indicates the path of the file where the debug messages will be stores
 * @param level Indicates the debug level 
 */
Log::Log(string ErrorPath,string DebugPath,int level)
{
  this->ErrorPath = ErrorPath;
  this->DebugPath = DebugPath;
  this->level = level;
  this->line = 0;
  
  this->verboseMode = false;
  
  //opening the log and error files
  
  this->ErrorFile.open(this->ErrorPath.c_str(),ios::app);
  
  if(!this->ErrorFile.is_open())
  {
    cout << "Error: The Error file "+this->ErrorPath+ " has not been openend. Some error has occurred" << endl; //to be modified
    cout << "	    Please check that the file path is correct and all the directories exist" << endl;
  }
  
  this->LogFile.open(this->DebugPath.c_str(),ios::app);
    
  if(!this->LogFile.is_open())
  {  
    cout << "Error: The Log file "+this->DebugPath+ " has not been openend. Some error has occurred" << endl; //to be modified
    cout << "	    Please check that the file path is correct and all the directories exist" << endl;
  }
}

/**
 * The default destructor for the class 
 */
Log::~Log()
{
  this->ErrorFile.close();
  this->LogFile.close();
}
/***************************
  Auxiliar functions
***************************/

/**
 * Stores the error 
 * @param error The error to be stored 
 */
void Log::error(string error)
{
  time_t rawtime;

  time ( &rawtime );
  char * time = ctime (&rawtime);
  time[24] = ' '; //chomp te \n character  
  
  this->ErrorFile << "[ERROR - "<< time << "] " << error << endl;
  
  if(this->verboseMode)
   std::cout << "[ERROR - "<< time << "] " << error << endl;
   
  this->line++;
}

/**
 * Stores a debug message 
 * @param log The message to be stored 
 */
void Log::debug(string log)
{
  time_t rawtime;

  time ( &rawtime );
  char * time = ctime (&rawtime);
  time[24] = ' '; //chomp te \n character
  
  if(this->verboseMode)
   std::cout  << "[DEBUG -"<<time << " "<< this->line <<"] " << log << endl;

  this->LogFile   << "[DEBUG -"<<time << " "<< this->line << "] " << log << endl;
   
  this->line++; 
}

/**
 * Debugs a message with a given level. If the levelmsg is lower than the level set the message will be stored 
 * @param log The debug message 
 * @param levelmsg The level of the message 
 */
void Log::debug(string log,int levelmsg)
{
  if(levelmsg < 2  || (this->level >= levelmsg))
  	this->debug(log+" (L"+itos(levelmsg)+")");
}

/***************************
   Sets and gets functions  
***************************/

/**
 * Returns the current debug level 
 * @return The debug level 
 */
int Log::getlevel() 
{
  return level;
}


/**
 * Sets the debug level 
 * @param theValue The debug level 
 */
void Log::setLevel(int& theValue)
{
  assert(theValue >= 0);
  level = theValue;
}


/**
 * Sets the verbose mode , if true the messages will be shown in the Standard outpu 
 * @return A bool indicating if the verboseMode is on 
 */
bool Log::getverboseMode() const
{
  return verboseMode;
}


/**
 * Sets the verbose mode 
 * @param theValue The verbose mode
 */
void Log::setVerboseMode(bool theValue)
{
  verboseMode = theValue;
}

}
