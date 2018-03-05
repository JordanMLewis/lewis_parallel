#include <PNMreader.h>
#include <PNMwriter.h>
#include <filters.h>
#include <logging.h>

int main(int argc, char *argv[])
{
    Logger log;
    PNMreader reader(argv[1]);

    //Filters go here
    Invert inv;

    inv.SetInput(reader.GetOutput());

    log.start();
    inv.GetOutput()->Update();
    log.stop();
    char msg1[1024]; sprintf(msg1, "Time elapsed: %f\n", log.getElapsedTime()); log.LogEvent(msg1);
    
    PNMwriter writer;
    writer.SetInput(inv.GetOutput());
    writer.Write(argv[2]);
    return 0; 
}
