#include <image.h>
#include <stdlib.h>
#include <stdio.h>
#include <filters.h>
#include <PNMreader.h>
#include <logging.h>

Image::Image(void){
	this->width = 0;
	this->height = 0;
	this->maxVal = 0;
	this->magicNum = NULL;
	this->pixels = NULL;
}

Image::Image(char* magic, int w, int h, int max){
	this->magicNum = magic;
	this->width = w;
	this->height = h;
	this->maxVal = max;
}

Image::Image(Image &i){	
	this->magicNum = i.magicNum;
	this->width = i.width;
	this->height = i.height;
	this->maxVal = i.maxVal;
}

Image::~Image(){
	if(this->pixels != NULL){
		free(this->pixels);
	}
}

void Image::ResetSize(int w, int h){
	this->width = w;
	this->height = h;

	//Free memory before allocating new memory  
	if(this->pixels != NULL){
		free(this->pixels);
		this->pixels = (Pixel*) malloc(sizeof(Pixel) * w * h);
	} else {	
		this->pixels = (Pixel*) malloc(sizeof(Pixel) * w * h);
	}
}
void Image::setMaxval(int val){ this->maxVal = val;}
void Image::setMagicnum(char* num){ this->magicNum = num;}

int    Image::getWidth(void)   const { return (this->width);}
int    Image::getHeight(void)  const { return (this->height);}
int    Image::getMaxval(void)  const { return (this->maxVal);}
char*  Image::getMagicnum(void)const { return (this->magicNum);}
Pixel* Image::getPixels(void)  const { return (this->pixels);}

void Image::Update(void) const{

	if(this->hasNotBeenUpdated()){	
		if(myFilter != NULL){
			myFilter->Update();
		} else if(color != NULL){
			color->Update();
		} else if(reader != NULL){
			reader->Update();
		}
	}
	this->updated = true;
}
