#!/bin/bash

#This is needed if no standard instal·lation of xerces and xalanc
#export XERCESCROOT=/home/irodero/kento-sim_64/XMLLibs/xerces-c_2_8_0-x86_64-linux-gcc_3_4
#export XALANCROOT=/home/irodero/kento-sim_64/XMLLibs_64/xml-xalan/c
#export LD_LIBRARY_PATH=/home/irodero/kento-sim_64/XMLLibs/xerces-c-src_2_7_0/lib:/home/irodero/kento-sim_64/XMLLibs/xml-xalan/c/lib:$LD_LIBRARY_PATH

#valgrind example
#valgrind --tool=massif --depth=30 --format=html ./kento_sim -S simulationConfiguration.xml

./kento_sim -S simulationConfiguration.xml > out.txt &


#COUNTER=0
#while [  $COUNTER -lt 10 ]; do
#sleep 60
#top -b -n 1 >> top.txt
#done



