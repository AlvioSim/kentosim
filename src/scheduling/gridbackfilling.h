#ifndef SIMULATORGRIDBACKFILLING_H
#define SIMULATORGRIDBACKFILLING_H

#include <scheduling/schedulingpolicy.h>
#include <utils/fcfsjobqueue.h>
#include <predictors/predictor.h>

#include <list>
#include <vector>
#include <map>

using namespace std;
using namespace PredictionModule;
using std::list;
using std::vector;
using std::map;

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/


class GridBackfillingPolicy : public SchedulingPolicy{
public:
  GridBackfillingPolicy();
  ~GridBackfillingPolicy();
};

}

#endif
