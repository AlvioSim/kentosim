#!/bin/bash
# @ job_name = Collision   sdsc exp36
# @ initialdir = /gpfs/home/bsc15/bsc15879/experiments/CollisionesEASY/SDSC-SP2/experiment36
# @ output = /gpfs/home/bsc15/bsc15879/experiments/CollisionesEASY/SDSC-SP2/experiment36/slurm.out
# @ error = /gpfs/home/bsc15/bsc15879/experiments/CollisionesEASY/SDSC-SP2/experiment36/slurm.err
# @ total_tasks = 1
# @ wall_clock_limit = 20:00:00
# @ tasks_per_node = 1

export XERCESCROOT=/home/bsc15/bsc15879/aplic/XMLLibs/xerces-c-src_2_7_0
export XALANCROOT=/home/bsc15/bsc15879/aplic/XMLLibs/xml-xalan/c
export PATH=$XERCESCROOT/bin:$PATH
export PATH=$XALANCROOT/bin:$PATH
export LD_LIBRARY_PATH=/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/lib:$LD_LIBRARY_PATH

/home/bsc15/bsc15879/scripts/monitor.sh &

/home/bsc15/bsc15879/sim_mn2/src/sim_mn2 -S /gpfs/home/bsc15/bsc15879/experiments/CollisionesEASY/SDSC-SP2/experiment36/simulationConfiguration.xml

