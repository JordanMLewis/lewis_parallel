
#ifndef  LOGGING_H
#define  LOGGING_H

#include <exception>
#include <stdio.h>
#include <sys/time.h>

using std::exception;


class DataFlowException : public exception
{
  public:
                         DataFlowException(const char *type, const char *error);
    virtual const char  *what() const throw() { return msg; };

  protected:
    char        msg[1024];
};


class Logger
{
  public:
	  ~Logger();

    static void     LogEvent(const char *event);
    static void     Finalize();
    
    void	    start(void);//{ startTime = };
    void	    stop(void);// { finishTime = };
   
    double 	    getStartTime(void);//   {return startTime; }
    double 	    getFinishTime(void);//  {return finishTime; }
    double          getElapsedTime(void);// {return finishTime - startTime; }	    



  private:
    static   FILE  *logger;
    struct timeval   startTime;
    struct timeval   finishTime;
};

#endif
