#include "PNMreader.h"
#include <stdlib.h>
#include <stdio.h>
#include <logging.h>

PNMreader::PNMreader(char* file){
	this->filename = file;	
	this->img->setReader(this);
	return;
};

PNMreader::~PNMreader(void){};

void PNMreader::Execute(void){
	
	Logger::LogEvent("PNMreader: about to execute");
	char magicNum[100];
	int width, height, maxVal;
	if(filename == NULL){
		Logger::LogEvent("PNMreader: no input file!");
		exit (EXIT_FAILURE);		
	}

	FILE* infile = fopen(filename, "r");

	//fopen was unsuccessful
	if(infile == NULL){	
		char msg[1024];
		sprintf(msg, "PNMreader: invalid file name!!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	Image* output = this->GetOutput();

	fscanf(infile, "%s\n%d %d\n%d\n", magicNum, &width, &height, &maxVal);

	output->ResetSize(width, height);
	output->setMagicnum(magicNum);
	output->setMaxval(maxVal);

	fread(output->getPixels(), sizeof(Pixel), height*width, infile);
	fclose(infile);

	Logger::LogEvent("PNMreader: done executing");
	return;
};

void PNMreader::Update(void){	
	this->Execute();
}

const char* PNMreader::getSourceName(){
	return "(PNMreader)";
}
