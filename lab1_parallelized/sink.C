#include <image.h>
#include <sink.h>
#include <stdlib.h>

Sink::Sink(){
	this->input1 = NULL;
	this->input2 = NULL;
	return;
}

Sink::~Sink(){

}

void Sink::SetInput(const Image* img){
	//Allow setting iff input1 = NULL
	if(this->input1 == NULL){
		this->input1 = img;
	}
}

void Sink::SetInput2(const Image* img){
	//Allow setting iff input2 = NULL
	if(this->input2 == NULL){
		this->input2 = img;
	}
}

const Image* Sink::getInput(void){
	return (this->input1);
}

const Image* Sink::getInput2(void){
	return (this->input2);
}
