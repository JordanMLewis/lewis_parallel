#ifndef SOURCE_H
#define SOURCE_H

#include "image.h"

class Source{

	public:
		Source();
		~Source();
	
		Image* GetOutput();
		
		virtual void Update();
		const virtual char* getSourceName() = 0;

	protected:
		virtual void Execute() = 0;
		Image* img;
		char* sourceName = NULL;

};

#endif
