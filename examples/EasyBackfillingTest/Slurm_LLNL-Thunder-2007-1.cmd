#!/bin/bash
# @ job_name = LLNL-Thunder-2007-1 
# @ initialdir = /home/bsc15/bsc15879/experiments/large_data
# @ output = /home/bsc15/bsc15879/experiments/large_data/slurmLLNL-Thunder-2007-1.out
# @ error = /home/bsc15/bsc15879/experiments/large_data/slurmLLNL-Thunder-2007-1.err
# @ total_tasks = 1
# @ wall_clock_limit = 20:00:00
# @ tasks_per_node = 1
#@ cpus_per_task = 4

/home/bsc15/bsc15879/scripts/monitor.sh&

export XERCESCROOT=/home/bsc15/bsc15879/aplic/XMLLibs/xerces-c-src_2_7_0
export XALANCROOT=/home/bsc15/bsc15879/aplic/XMLLibs/xml-xalan/c
export PATH=$XERCESCROOT/bin:$PATH
export PATH=$XALANCROOT/bin:$PATH
export LD_LIBRARY_PATH=/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/lib:$LD_LIBRARY_PATH

/home/bsc15/bsc15879/sim_mn2/src/sim_mn2 -S /home/bsc15/bsc15879/experiments/large_data/simulationConfiguration_LLNL-Thunder-2007-1.xml

