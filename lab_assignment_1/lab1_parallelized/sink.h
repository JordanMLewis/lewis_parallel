#ifndef SINK_H
#define SINK_H

#include <source.h>

class Sink {

	public:	
		Sink();
		~Sink();

		void SetInput(const Image* img);
		void SetInput2(const Image* img);
		const Image* getInput(void);
		const Image* getInput2(void);

	protected:
		const Image* input1;
		const Image* input2;
		char* sinkName = NULL;
};
#endif
