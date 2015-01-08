#We create the number of jobs per hour
lastArrival<-max(trace$JobSimSubmitTime)
hoursInSeconds<-seq(0, lastArrival, by=3600) 


#We discretize the values
jobHourArrival<- cut(trace$JobSimSubmitTime,breaks=hoursInSeconds,na.rm = TRUE);
names(jobHourArrival)<-trace$JobId
arrivalsPerHour<-table(jobHourArrival)

barplot(arrivalsPerHour,
     main="Arrivals per hour",
     sub="number of jobs" ,
     xlab="Hour in the simulation",
     ylab="Number of arrivals");


#We create the number of jobs per day
daysInSeconds<-seq(0, lastArrival, by=3600*24) 

#We discretize the values
jobDayArrival<- cut(trace$JobSimSubmitTime,breaks=daysInSeconds,na.rm = TRUE);
names(jobDayArrival)<-trace$JobId
arrivalsPerDay<-table(jobDayArrival)

barplot(arrivalsPerDay,
     main="Arrivals per day",
     sub="number of jobs" ,
     xlab="Day in the simulation",
     ylab="Number of arrivals");

#We create the number of jobs per month
monthInSeconds<-seq(0, lastArrival, by=3600*24*30) 

#We discretize the values
jobMonthArrival<- cut(trace$JobSimSubmitTime,breaks=monthInSeconds,na.rm = TRUE);
names(jobMonthArrival)<-trace$JobId
arrivalsPerMonth<-table(jobMonthArrival)

barplot(arrivalsPerMonth,
     main="Arrivals per month",
     sub="number of jobs" ,
     xlab="Month in the simulation",
     ylab="Number of arrivals");

