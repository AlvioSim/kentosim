#include <datamining/wekaobject.h>
#include <utils/utilities.h>
#include <time.h>

#include <string>
#include <utils/executecommand.h>
#include <assert.h>

using namespace Utils;

namespace Datamining {

/**
 * The default constructor for the class
 */
WekaObject::WekaObject(Log* log)
{
  this->logWeka = log;
}


/**
 * The default destructor for the class 
 */
WekaObject::~WekaObject()
{

}

/**
 * This function using the command line invoques to a given class, a given functionality with a given parameters 
 * @param WekaClass The class to be invoques. I.e: weka.classifiers.trees.J48 
 * @param Parameters The parameters to be proviede. I.e: -l j48.model -T test.arff -p 0 2
 * @param neededOutput The parameter indicates if the weka ouptut is required 
 * @return A list of strings containing the output for the execution  
 */
vector<string>* WekaObject::InvoqueWekaFunctionality(string WekaClass, string Parameters,bool neededOutput)
{

  string command = " -classpath "+this->wekaPath+" "+WekaClass+" "+Parameters;
  vector<string>* output;
 
  ExecuteCommand* com = new ExecuteCommand(this->JavaBinary,command,neededOutput,this->logWeka,this->TemporaryDir);
  
  output = com->execute();
  
  return output;

}


/**
 * Function that initialize the parameters required for invoquing the weka command line interface
 * @param JavaBinary String containing the file path for the java binary file 
 * @param classpath String containing the class path for the java
 * @param wekaPath String containing the path for the weka jar file 
 * @param TemporaryDir String containing the path for the temporary files dir 
 */
void WekaObject::setWekaEnv(string JavaBinary,string classpath,string wekaPath,string TemporaryDir)
{
  this->JavaBinary = JavaBinary;
  this->classpath = classpath;
  this->wekaPath = wekaPath;
  this->TemporaryDir = TemporaryDir;
}

/**
 * Given a set of instances and the definition of its attribute names and attribute types provided in the header entries, will create an arff file with such information in the specified filepaht
 * @param FilePath Contains the filepath where the arff file will be dump 
 * @param instances The set of instances to be dump 
 * @param headerEntries The header entries for the file 
 * @param numberOfInstances The number of instances in the dataset 
 * @return A bool indicating if the file has been correctly created 
 */
bool WekaObject::generateArffFileWithHeader(string FilePath, InstancesSet* instances,vector<string>* headerEntries,int numberOfInstances)
{

 vector<string> InstanceLine(numberOfInstances,string(""));
  
 int processedAtributed = -1;
    
 /* We will have to iterate over all the attributes and creating in one hand the header stuff, and on the other the arfff content */
 int totalAtts = instances->size();
 
 for(InstancesSet::iterator it = instances->begin(); it != instances->end();++it)
 {
    processedAtributed++;
            
    vector<string>* values = *it;
  
    int instanceId = 0;
    
    map<string,int> literalCount;
    
    for(vector<string>::iterator itv = values->begin();itv != values->end();++itv)
    {
      string st = *itv;
    
      map<string,int>::iterator inserted = literalCount.find(st);
            
      /* Generateing the attribute content for the instance */
      if(processedAtributed < totalAtts-1)   
        InstanceLine[instanceId]+=st+",";
      else
        /* last instance  */
        InstanceLine[instanceId]+=st;
        
      instanceId++;
    }

  }  

  
  this->saveArffFile(FilePath,headerEntries,&InstanceLine);
}


/**
 * Given a set of instances and the definition of its attribute names and attribute types, will create an arff file with such information in the specified filepaht
 * @param FilePath Contains the filepath where the arff file will be dump 
 * @param instances The set of instances to be dump 
 * @param attributes The attributes names for the instances 
 * @param attributesTypes The attributes types for the instances 
 * @param numberOfInstances The number of instances in the dataset 
 * @return A bool indicating if the file has been correctly created 
 */
bool WekaObject::generateArffFile(string FilePath,InstancesSet* instances,vector<string>* attributes, vector<native_t>* attributesTypes,int numberOfInstances)
{
 
 vector<string> InstanceLine(numberOfInstances,string(""));
 vector<string> AtributeLine;
  
 generateHeaderLines(instances,attributes,attributesTypes,numberOfInstances,&InstanceLine, &AtributeLine);
  
 this->saveArffFile(FilePath,&AtributeLine,&InstanceLine);
}

void WekaObject::generateHeaderLinesForEvaluate(vector<string>* toClassify,vector<string>* attributes, vector<native_t>* attributesTypes,vector<string> * InstanceLine,vector<string>* AtributeLine )
{
   int processedAtributed = -1;
    
 /* We will have to iterate over all the attributes and creating in one hand the header stuff, and on the other the arfff content */
 int totalAtts = toClassify->size();
 
 for(vector<string>::iterator it = toClassify->begin(); it != toClassify->end();++it)
 {
    processedAtributed++;
    
    string attType = this->GetAttributeWekaType(attributesTypes->operator[](processedAtributed));
    string attName = attributes->operator[](processedAtributed);    
        
    
    map<string,int> literalCount;

    //now we check if there is any new field in the instance to classify
    string st = *it;
    
    map<string,int>::iterator inserted = literalCount.find(st);
    
    if(inserted == literalCount.end())
      literalCount.insert(pair<string,int>(st,1));
    else
      literalCount[st] = literalCount[st]+1;
        
    /* Generateing the attribute content for the instance */
    if(processedAtributed < totalAtts-1)   
      InstanceLine->operator[](0)+=st+",";
    else
      /* last instance  */
      InstanceLine->operator[](0)+=st;


    //If it is nominal more stuff has to be done 
    string literals  = "{";
    int processedLiterals = 0;
  
    //now we create the string and count them 
    for(map<string,int>::iterator itc = literalCount.begin(); itc != literalCount.end();++itc)
    {
      processedLiterals++;
    
      string literal = itc->first;
      
      if(processedLiterals < literalCount.size())
        literals+= literal+",";
      else
        literals+= literal;
      
    }
    
    literals+="}";
    
    AtributeLine->push_back("@attribute "+attName+" "+literals);
  }  
}

void WekaObject::generateHeaderLines(InstancesSet* instances,vector<string>* attributes, vector<native_t>* attributesTypes,int numberOfInstances,vector<string> * InstanceLine,vector<string>* AtributeLine )
{
 int processedAtributed = -1;
    
 /* We will have to iterate over all the attributes and creating in one hand the header stuff, and on the other the arfff content */
 int totalAtts = instances->size();
 
 for(InstancesSet::iterator it = instances->begin(); it != instances->end();++it)
 {
    processedAtributed++;
    
    string attType = this->GetAttributeWekaType(attributesTypes->operator[](processedAtributed));
    string attName = attributes->operator[](processedAtributed);    
        
    vector<string>* values = *it;
  
    int instanceId = 0;
    
    map<string,int> literalCount;
    
    for(vector<string>::iterator itv = values->begin();itv != values->end();++itv)
    {
      string st = *itv;
    
      map<string,int>::iterator inserted = literalCount.find(st);
    
      if(inserted == literalCount.end())
        literalCount.insert(pair<string,int>(st,1));
      else
        literalCount[st] = literalCount[st]+1;
        
      /* Generateing the attribute content for the instance */
      if(processedAtributed < totalAtts-1)   
        InstanceLine->operator[](instanceId)+=st+",";
      else
        /* last instance  */
        InstanceLine->operator[](instanceId)+=st;
        
      instanceId++;
    }

    if(attType.compare("real") == 0)
    {
      AtributeLine->push_back("@attribute "+attName+" real");
      continue;
    }

    //If it is nominal more stuff has to be done 
    string literals  = "{";
    int processedLiterals = 0;
  
    //now we create the string and count them 
    for(map<string,int>::iterator itc = literalCount.begin(); itc != literalCount.end();++itc)
    {
      processedLiterals++;
    
      string literal = itc->first;
      
      if(processedLiterals < literalCount.size())
        literals+= literal+",";
      else
        literals+= literal;
      
    }
    
    literals+="}";
    
    AtributeLine->push_back("@attribute "+attName+" "+literals);
  }  


}

/**
 * This function saves the attribute definition and instance to the file path provided 
 * @param FilePath Contains the filepath where the arff file will be dump 
 * @param AttLines Lines defining the attributes 
 * @param InstanceLines Lines containing the instances definition 
 * @return A bool indicating if the file has bee correctly saved
 */
bool WekaObject::saveArffFile(string FilePath,vector<string>* AttLines,vector<string>* InstanceLines)
{
  ofstream arffFile( FilePath.c_str(), ios::trunc );     
  assert(!arffFile.fail());
  
  arffFile <<  "@relation AlvioSimWekaModels" << endl;
  arffFile << endl;
  
  //we dump the att 
  for(vector<string>::iterator it = AttLines->begin();it != AttLines->end();++it)
  {
    arffFile << *it << endl;
  }
  
  arffFile << endl;
  arffFile <<  "@data" << endl;

  //we dump the instace 
  for(vector<string>::iterator it = InstanceLines->begin();it != InstanceLines->end();++it)
  {
    arffFile << *it << endl;
  }  
  
  arffFile.close();
}

/**
 * Given a load Arffile the function will return the InstanceSet containing all the attributes contained in the file 
 * @param FilePath The file path where the arffile to be loaded is stored 
 * @param NumberAttributes The number of attributes that the Arffile Has 
 * @return The InstancesSet containing the instances included in the file 
 */
InstancesSet* WekaObject::loadInstancesArffFile(string FilePath,int NumberAttributes)
{
  ifstream arff (FilePath.c_str());
  string line;
  string separator(",");

  
  if (!arff.is_open())
  {
    return NULL;
  }
  
  InstancesSet* instances = new InstancesSet();
  
  //we initialize the vector for each attribute 
  for(;NumberAttributes>0;NumberAttributes--)
  {
    instances->push_back(new vector<string>());  
  }
  
  while (! arff.eof())
  {
      
    getline (arff,line);      
      
    //for each line we try to parse the swf
    //if ";" means its header stuff so we should process it
    string::size_type loc = line.find("@", 0 );
      
    if( loc != string::npos )
    {           
      continue;
    }
    
    deque<string> fields;     
    SplitLine(line,separator,fields);
    
    int attindex = 0;
    
    for(deque<string>::iterator it = fields.begin(); it != fields.end();++it)
    {
      instances->operator[](attindex)->push_back(*it);
      attindex++;
    }
  }
  
  return instances;
}

/**
 * Given a native type the function returns the string contining the weka type associated to such type 
 * @param type The native type 
 * @return The string containing the file type 
 */
string WekaObject::GetAttributeWekaType(native_t type)
{
  switch(type)
  {
    case INTEGER:
    case DOUBLE:
      return "real";
    case STRING:
      return "";
    defaul:
     assert(false);     
  }
}

/**
 * Given an arff file loads to the provided string vector the definition of the attributes 
 * @param file the arff file to be analized
 * @param output the vector where the attribute definition will be dumped
 */
void WekaObject::readAttributesDeffinition(string file,vector<string>* output)
{
  ifstream arff (file.c_str());
  string line;
  string separator(",");

  
  if (!arff.is_open())
  {
    this->logWeka->error("error when oppening the arf file "+file);
  }
  
  InstancesSet* instances = new InstancesSet();
  
  int skipLines = 2;
  
  while (! arff.eof())
  {
    
    getline (arff,line);      
    
    if(skipLines > 0)
    {
      skipLines--;
      continue;  
    }
            
    //for each line we try to parse the swf
    //if ";" means its header stuff so we should process it
    string::size_type loc = line.find("@", 0 );
      
    if( loc == string::npos && line.size() < 1 )
    {           
      return;
    }
    
    output->push_back(line);
    
    int attindex = 0;
  }  
}

/**
 * Given a real attribute but that has been discretized in the intervals provided in the string the function returns the interval where the real belongs 
 * @param attributeDefinition The intervals definition 
 * @param real The real attribute to be classified in one of the provided intervals 
 * @return The interval where the real belongs 
 */
string WekaObject::getIntervalFromReal(string attributeDefinition, string real)
{
  
  bool notFound = true;
    
  //In some cases the {All} will be the unique value of the intervals .. so before do any parsing we will try to use it
  string::size_type all = attributeDefinition.find("All",0);
  
  if(all != string::npos)
    return string("'\\'All\\''");
    
    
  //before anything we must remove the stuff @...{ } so we will do it through a substr
  string::size_type atdefstart = attributeDefinition.find("{");
  string::size_type atfedend = attributeDefinition.find("}");
    
  string attributes = attributeDefinition.substr(atdefstart+1,atfedend-atdefstart-1);
    
  string separator = ",";
  deque<string> fields;     
  SplitLine(attributes,separator,fields);
   
  
  for(deque<string>::iterator it = fields.begin();it != fields.end();++it)
  {
    string classification = *it;
           
    //The following formats can be returned by the classifier ..
    //  (-inf-XXXX] -> type 1  .. 
    //  (XXX-XXXX]  -> type 2 
    //  (XXXX-inf)  -> type 3
    //where the XXX is a string containing a double 
       
    string::size_type end = classification.find(")",0);
        
    if(end == string::npos)
      end =  classification.find("]",0);            
          
    string::size_type start = classification.find("-",0);
    
    //if the infinite is the upper bound this is the good interval ..
    if(classification.find("-inf",start) != string::npos)
    {
      start = classification.find("-",start+1);
    }
    if(classification.find("inf",start) != string::npos)
      return classification;    
    
    assert(start != string::npos && end != string::npos);     
    
    //we get the last part of the interval that by construction will allow to us to know if the real belongs to this interval 
    //inf can not be .. we have previously checked 
    string upperBound = classification.substr(start+1,end-start-1);
     
    if(atof(upperBound.c_str()) > atof(real.c_str()))
     return classification;        
  }
}

}
