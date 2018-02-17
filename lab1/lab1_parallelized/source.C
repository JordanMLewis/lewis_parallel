#include <source.h>
#include <filters.h>
#include <stdlib.h>

Source::Source(void){
	this->img = new Image;
}

Source::~Source(void){
	delete this->img;
}

void Source::Update(void){
	if(img->getFilter() != NULL){
		img->getFilter()->Execute();
	}
}

Image* Source::GetOutput(void){
	return (this->img);
}
