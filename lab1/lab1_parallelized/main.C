#include <PNMreader.h>
#include <PNMwriter.h>
#include <filters.h>
#include <logging.h>

#define UNUSED __attribute__((unused))

int main(UNUSED int argc, UNUSED char *argv[])
{
    PNMreader reader(argv[1]);
    Logger log;
    
    //Filters go here
    Mirror mr;

    mr.SetInput(reader.GetOutput());

    log.start();
    mr.GetOutput()->Update();
    log.stop();
    char msg1[1024]; sprintf(msg1, "Time elapsed: %f\n", log.getElapsedTime()); log.LogEvent(msg1);
    
    PNMwriter writer;
    writer.SetInput(mr.GetOutput());
    writer.Write(argv[2]);
    return 0; 
}
