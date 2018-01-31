#include "PNMwriter.h"
#include <stdio.h>
#include <stdlib.h>
#include <logging.h>

PNMwriter::PNMwriter(){
	//this->SetInput(new Image);
	//this->SetInput2(new Image);
	return;
}

PNMwriter::~PNMwriter(){
	
	return;
}

void PNMwriter::Write(char* filename){
	
	if(filename == NULL){
		char msg[1024];
		sprintf(msg, "(Sink): Output file name not specified.");
		DataFlowException e("", msg);
		throw e;
	}
	FILE* outfile = fopen(filename, "w");

	if(outfile == NULL){
		char msg[1024];
		sprintf(msg, "(Sink): Unable to open output file.");
		DataFlowException e("", msg);
		throw e;
	}

	//Write header to the output PNM file
	fprintf(outfile, "P6\n");
	fprintf(outfile, "%d %d\n", this->getInput()->getWidth(), this->getInput()->getHeight());
	fprintf(outfile, "%d\n", this->getInput()->getMaxval());

	//Write image data to output PNM file
	fwrite(this->getInput()->getPixels(), sizeof(Pixel), this->getInput()->getWidth() * this->getInput()->getHeight(), outfile);
	fclose(outfile);
	return;
}

char* PNMwriter::getSinkName(){
	return this->sinkName;
}
