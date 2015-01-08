
## Time to read the CSV file 

trace<-read.csv(CSVFilePath, header = TRUE);

pdf(thePDFFile,encoding="ISOLatin1", family="URWHelvetica",onefile=TRUE);
par(pty="s");

## Plots with general information (or derviate)

source(paste(ScriptsBaseDir,"/WaitTimeSimPlot.r", sep = ""))
source(paste(ScriptsBaseDir,"/SlowdownSimPlot.r", sep = ""))
source(paste(ScriptsBaseDir,"/ArrivalsPerHourDayMonth.r", sep = ""))
source(paste(ScriptsBaseDir,"/LoadPerHourDay.r", sep = ""))

## Statistical information, such as quartiles and so on..


#Closing the stream
dev.off();