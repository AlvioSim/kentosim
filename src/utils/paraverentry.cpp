#include <utils/paraverentry.h>

namespace Utils {

/**
 * The default constructor for the class
 * @param entry the content of the entry
 * @param time the time associated to the entry
 * @param the id for the entry
 */
ParaverEntry::ParaverEntry(string entry,double time,int id)
{
  this->entry = entry;
  this->time = time;
  this->id = id;
}

/**
 * The default destructor for the class 
 */
ParaverEntry::~ParaverEntry()
{
}


}
