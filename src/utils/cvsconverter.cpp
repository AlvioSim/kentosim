#include <utils/cvsconverter.h>

namespace Utils {

/**
 * The default constructor for the class
 * @param FilePath The file path where to store the cvs file 
 * @param log The logging engine 
 */
CSVConverter::CSVConverter(string FilePath,Log* log)
{
  this->FilePath = FilePath;
  this->log = log;
  this->separator = ",";
}

/**
 * The default destructor for the class 
 */
CSVConverter::~CSVConverter()
{
}

/**
 * Returns the CSVHeaderFields
 * @return A a vector containing the CSVHeaderFields
 */
vector< string >* CSVConverter::getCSVHeaderFields() const
{
  return CSVHeaderFields;
}

/**
 * Sets the CSVHeaderFields to the CSVConverter 
 * @param theValue The CSVHeaderFields
 */
void CSVConverter::setCSVHeaderFields(vector< string >* theValue)
{
  CSVHeaderFields = theValue;
}

/**
 * Returns the fieldTypes
 * @return A a vector containing the fieldTypes
 */
vector< file_field_t >* CSVConverter::getfieldTypes() const
{
  return fieldTypes;
}

/**
 * Sets the fieldTypes to the CSVConverter 
 * @param theValue The fieldTypes
 */
void CSVConverter::setFieldTypes(vector< file_field_t >* theValue)
{
  fieldTypes = theValue;
}

/**
 * Opens the output cvs file 
 */
void CSVConverter::open()
{
  this->cvsFile = new ofstream( this->FilePath.c_str(), ios::trunc);
  this->opened = true;
   
  bool ini = true;
   
  string CSVLine = "";
  
  for(vector<string>::iterator itf = this->CSVHeaderFields->begin();itf != this->CSVHeaderFields->end();++itf)
  {
    if(!ini)
      CSVLine+=this->separator;
    else
      ini = false;   
     
    CSVLine+="\""+*itf+"\""; 
  }
  
   (*this->cvsFile) << CSVLine << endl;
}

/**
 * Adds a new row to the cvs output file 
 * @param fields The fields to be added 
 */
void CSVConverter::addEntry(vector<string>* fields)
{
  assert(opened);
  
  int i = 0;
  string CSVLine = "";
  
  bool ini = true;
  
  for(vector<string>::iterator itf = fields->begin();itf != fields->end();++itf)
  {
    if(!ini)
     CSVLine+=this->separator;
    else
     ini = false;
     
    file_field_t type =  this->fieldTypes->operator[](i);
    
    switch(type)
    {
      case STRING_FIELD:
        CSVLine+="\""+*itf+"\""; 
        break;
      case DOUBLE_FIELD:
      case INT_FIELD:
        CSVLine+=*itf;
        break;
      default:
        assert(false);
    }
  }
  
  (*this->cvsFile) << CSVLine << endl;
  
}

/**
 * Closes the cvs file 
 */
void CSVConverter::close()
{
  this->cvsFile->close();
}


/**
 * Returns the cvs converter type implemented by the current converter
 * @return A cvs_converter_t indicating the cvs converter type implemented by the current class 
 */
csv_converter_t CSVConverter::gettype() const
{
  return type;
}


/**
 * Sets the cvs converter type implemented by the current converter 
 * @param theValue The type of converter implemented by the current class
 */
void CSVConverter::setType(const csv_converter_t& theValue)
{
  type = theValue;
}


/**
 * Returns the file path of the csv that will be created by the class 
 * @return A string containing the file path 
 */
string CSVConverter::getFilePath() const
{
  return FilePath;
}


/**
 * Set the file path of the csv that will be created by the class 
 * @param theValue The file path for the file 
 */
void CSVConverter::setFilePath(const string& theValue)
{
  FilePath = theValue;
}

}

