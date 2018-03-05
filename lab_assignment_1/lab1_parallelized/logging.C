#include <logging.h>
#include <sys/time.h>

FILE* Logger::logger = fopen("program_log", "w");

Logger::~Logger(){
	Finalize();
}

DataFlowException::DataFlowException(const char* type, const char* error)
{
	//Print to log file
	sprintf(this->msg, "Throwing exception: %s: %s", type, error);
	Logger::LogEvent(this->msg);	
}

void Logger::LogEvent(const char* event)
{
	//print event to log file
	fprintf(logger, "%s\n", event);	
}

void Logger::Finalize(void)
{
	fclose(logger);
}

void Logger::start(void){
	gettimeofday(&(this->startTime), 0); 
}
void Logger::stop(void){
	gettimeofday(&(this->finishTime), 0);
}
double Logger::getElapsedTime(void){
	return double(this->finishTime.tv_sec - this->startTime.tv_sec) + double(this->finishTime.tv_usec - this->startTime.tv_usec) / 1000000;
}

