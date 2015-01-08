#Time to plot the load. First we must do all the prerpocessing stuff and naming
ProcessorsJobs<-trace$UsedProcessors
RuntimeJobs<-trace$JobSimFinishTime-trace$JobSimStartTime


names(ProcessorsJobs)<-trace$JobId
names(RuntimeJobs)<-trace$JobId


###########################################
### LOADS PER HOURS #######################
###########################################

loadPerHour<-rep(0,times=as.integer(lastArrival/3600)) 
names(loadPerHour)=names(arrivalsPerHour);
#Time to create the plots of the load per day
for(job in trace$JobId)
{
  loadPerHour[jobHourArrival[job]] <- loadPerHour[jobHourArrival[job]]+ProcessorsJobs[job]*RuntimeJobs[job]
}

#Now we have to normalize the value depending on the amount of computational power available each hour

loadPerHour <- loadPerHour/(MachineProcessors*3600);


barplot(loadPerHour,
     main="Load of the Machine",
     sub="per hours" ,
     xlab="Hour in the simulation",
     ylab="Load");

###########################################
### LOADS PER DAYS #######################
###########################################

loadPerDay<-rep(0,times=as.integer(lastArrival/(3600*24))) 
names(loadPerDay)=names(arrivalsPerDay);
#Time to create the plots of the load per day
for(job in trace$JobId)
{
  loadPerDay[jobDayArrival[job]] <- loadPerDay[jobDayArrival[job]]+ProcessorsJobs[job]*RuntimeJobs[job]
}

#Now we have to normalize the value depending on the amount of computational power available each hour

loadPerDay <- loadPerDay/(MachineProcessors*3600*24);


barplot(loadPerDay,
     main="Load of the Machine",
     sub="per days" ,
     xlab="Day in the simulation",
     ylab="Load");

