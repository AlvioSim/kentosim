#ifndef UTILSPARAVERENTRY_H
#define UTILSPARAVERENTRY_H

#include <string>

using namespace std;

namespace Utils {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** Implements a parver entry file */
class ParaverEntry{
public:
    ParaverEntry(string entry,double time,int id);
    ~ParaverEntry();
    
    string entry; /** The string containing the entry */
    double time; /** The time associated to the entry*/
    int id; /** The id for the entry*/
    

};

}

#endif
