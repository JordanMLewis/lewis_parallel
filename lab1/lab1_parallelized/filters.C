#include <filters.h>
#include <stdio.h>
#include <logging.h>
#include "parallelized.h"

void Blender::SetFactor(float f){
	if(0.0 <= f && f <= 1.0){
		this->factor = f;
	}
}
const char* Shrinker::getSourceName() { return "(Shrinker)";  }
const char* LRConcat::getSourceName() { return "(LRConcat)"; }
const char* TBConcat::getSourceName() { return "(TBConcat)";  }
const char* Blender::getSourceName()  { return "(Blender)";  }
const char* Mirror::getSourceName()   { return "(Mirror)";  }
const char* Rotate::getSourceName()   { return "(Rotate)";  }
const char* Subtract::getSourceName() { return "(Subtract)";  }
const char* Grayscale::getSourceName(){ return "(Grayscale)";  }
const char* Blur::getSourceName()     { return "(Blur)";  }
const char* Color::getSourceName()    { return "(Color)";  }
const char* CheckSum::getSourceName() { return "(CheckSum)";  }

Color::Color(int w, int h, unsigned char r, unsigned char g, unsigned char b){	
	//Check input parameters for validity
	if(w > 0 && h > 0){
	       	this->colorWidth = w;
		this->colorHeight = h;
		this->rVal = r;
		this->gVal = g;
		this->bVal = b;
		this->GetOutput()->setColor(this);
       	} else { 
		char msg[1024];
		sprintf(msg, "%s: Invalid color instantiated", getSourceName());
		DataFlowException e(getSourceName(), msg);
		throw e;
	}
}

void Color::Update(void) { this->Execute(); }

void CheckSum::OutputCheckSum(char const* filename){		
	
	Logger::LogEvent("CheckSum: about to execute");
	if(filename == NULL){	
		char msg[1024];
		sprintf(msg, "%s: No filename provided", getSourceName());
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	FILE* outputFile = fopen(filename, "w");
	if(outputFile == NULL){	
		char msg[1024];
		sprintf(msg, "%s: Unable to open checksum file", getSourceName());
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	const Image* input = getInput();
	
	if(input == NULL){	
		char msg[1024];
		sprintf(msg, "%s: No input1!", getSourceName());
		DataFlowException e(getSourceName(), msg);
		throw e;
	}
	
	Pixel* inputPixels = input->getPixels();
	int w = input->getWidth();
	int h = input->getHeight();
	int rowPlusj = 0;

	//Vars for keeping track of checksum
	unsigned char red_total = 0;
	unsigned char green_total = 0;
	unsigned char blue_total = 0;

	//Minimize arithmetic instructions
	int row = 0;

	for(int i = 0; i < h; i++){
		row = i*w;
		for(int j = 0; j < w; j++){
			rowPlusj = row + j;
			red_total += inputPixels[rowPlusj].r;	
			green_total += inputPixels[rowPlusj].g;	
			blue_total += inputPixels[rowPlusj].b;	
		}
	}

	fprintf(outputFile, "CHECKSUM: %d, %d, %d\n", red_total, green_total, blue_total); 
	fclose(outputFile);
	Logger::LogEvent("CheckSum: done executing");
}

void CheckSum::Execute(void) {} 
//void CheckSum::Update(void) {}

void Shrinker::Execute(){

	//Check to see if input1 is up to date
	Logger::LogEvent("Shrinker: about to update input1");
	if(this->getInput() != NULL && this->getInput()->hasNotBeenUpdated()){ 
		this->getInput()->Update();
	}  
	Logger::LogEvent("Shrinker: done updating input1");
	
	//Make sure input is actually valid
	if(this->getInput() == NULL){
		char msg[1024];
		sprintf(msg, "%s: Invalid input 1", getSourceName());
		DataFlowException e(getSourceName(), msg);
		throw e;
	}
	Logger::LogEvent("Shrinker: about to execute");

	//Get references early to clean up code
	const Image* input = getInput();
	Pixel* inputPixels = input->getPixels();
	int w = input->getWidth();
	int h = input->getHeight();
	
	Image* output = GetOutput();
	output->ResetSize(w/2, h/2);
	output->setMaxval(input->getMaxval());
	output->setMagicnum(input->getMagicnum());
	Pixel* outputPixels = output->getPixels();

	//Minimize arithmetic instructions
	int rowCol = 0;
	int w2 = w*2;
	int iw2 = 0;
	int j2 = 0;

	for(int i = 0; i < h/2; i++){
		iw2 = i*w2;
		for(int j = 0; j < w/2; j++){
			rowCol = getRowAndColumn(i, j);
			j2 = j*2; 
			outputPixels[rowCol].r = inputPixels[(iw2) + (j2)].r;
			outputPixels[rowCol].g = inputPixels[(iw2) + (j2)].g;
			outputPixels[rowCol].b = inputPixels[(iw2) + (j2)].b;
		}
	}	
}

void LRConcat::Execute(){
	//Check to see if both inputs are valid
	Logger::LogEvent("LRConcat: about to update input1");
	if(this->getInput() != NULL && this->getInput()->hasNotBeenUpdated()){ 
		this->getInput()->Update();
	} 
	Logger::LogEvent("LRConcat: done updating input1");
	
	Logger::LogEvent("LRConcat: about to update input2");
	if(this->getInput2() != NULL && this->getInput2()->hasNotBeenUpdated()){ 
		this->getInput2()->Update();
	}
	Logger::LogEvent("LRConcat: done updating input2");

	if(this->getInput() == NULL){	
		char msg[1024];
		sprintf(msg, "LRConcat: no input1!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	} else if (this->getInput2() == NULL){
		char msg[1024];
		sprintf(msg, "LRConcat: no input2!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	Logger::LogEvent("LRConcat: about to execute");
	
	const Image* left = getInput();    Pixel* leftPixels = left->getPixels();
	const Image* right = getInput2();  Pixel* rightPixels = right->getPixels();
	Image* output = GetOutput(); 

	//Check to make sure heights match
	if(left->getHeight() != right->getHeight()){
		char msg[1024];
		sprintf(msg, "LRConcat: heights must match: %d, %d", left->getHeight(), right->getHeight());
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	int leftWidth = left->getWidth();
	int rightWidth = right->getWidth();
	//int w = leftWidth;
	int h = left->getHeight();	
	int combinedWidth = leftWidth + rightWidth;

	output->ResetSize(combinedWidth, h);
	output->setMaxval(left->getMaxval());
	output->setMagicnum(left->getMagicnum());
	Pixel* outputPixels = output->getPixels();

	int outWidth = output->getWidth();

	//Variables used to reduce arithmetic instructions
	int iOutWidthPlusj = 0;   
	int iLeftWidthPlusj = 0;   
	int iRightWidthPlusJMinusLeftWidth = 0;

	for(int i = 0; i < h; i++){
		for(int j = 0; j < combinedWidth; j++){
			iOutWidthPlusj = i*outWidth + j; 
			iLeftWidthPlusj = i*leftWidth + j;
			iRightWidthPlusJMinusLeftWidth = i*rightWidth + (j-leftWidth);
			
			if(j < leftWidth){
				outputPixels[iOutWidthPlusj].r = leftPixels[iLeftWidthPlusj].r;
				outputPixels[iOutWidthPlusj].g = leftPixels[iLeftWidthPlusj].g;
				outputPixels[iOutWidthPlusj].b = leftPixels[iLeftWidthPlusj].b;
			} else {	
				outputPixels[iOutWidthPlusj].r = rightPixels[iRightWidthPlusJMinusLeftWidth].r;
				outputPixels[iOutWidthPlusj].g = rightPixels[iRightWidthPlusJMinusLeftWidth].g;
				outputPixels[iOutWidthPlusj].b = rightPixels[iRightWidthPlusJMinusLeftWidth].b;
			}
		}
	}
	Logger::LogEvent("LRConcat: done executing");
}

void TBConcat::Execute(){

	Logger::LogEvent("TBConcat: about to update input1");
	if(this->getInput() != NULL && this->getInput()->hasNotBeenUpdated()){ 
	       	this->getInput()->Update(); 
       	};
	Logger::LogEvent("TBConcat: done updating input1");
	
	Logger::LogEvent("TBConcat: about to update input2");
	if(this->getInput2() != NULL && this->getInput2()->hasNotBeenUpdated()){
	       	this->getInput2()->Update(); 
	};
	Logger::LogEvent("TBConcat: done updating input2");

	//check to make sure both inputs are valid	
	if(this->getInput() == NULL){
		char msg[1024];
		sprintf(msg, "%s: No input1!", getSourceName());
		DataFlowException e(getSourceName(), msg);
		throw e;
	} else if (this->getInput2() == NULL){
		char msg[1024];
		sprintf(msg, "%s: No input2!", getSourceName());
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	const Image* top = getInput();     Pixel* topPixels = top->getPixels();
	const Image* bottom = getInput2(); Pixel* bottomPixels = bottom->getPixels();
	Image* output = GetOutput();

	//Widths of top and bottom image must be the same
	if(top->getWidth() != bottom->getWidth()){
		char msg[1024];
		sprintf(msg, "%s: Widths much match: %d %d", getSourceName(), top->getWidth(), bottom->getWidth());
		DataFlowException e(getSourceName(), msg);
		throw e;
	}
	Logger::LogEvent("TBConcat: about to execute");
	
	int w = top->getWidth();
	int h = top->getHeight();
	int combinedHeight = top->getHeight() + bottom->getHeight();
	int bottomWidth = bottom->getWidth();

	output->ResetSize(w, combinedHeight);
	output->setMaxval(top->getMaxval());
	output->setMagicnum(top->getMagicnum());
	Pixel* outputPixels = output->getPixels();

	//Variables used to reduce arithmetic instructions
	int row = 0;
	int rowPlusj = 0;
	int iMinusHTimesbottomWidthPlusj = 0;

	for(int i = 0; i < combinedHeight; i++){
		row = i*w;
		for(int j = 0; j < w; j++){
			rowPlusj = row + j;
			if(i < h){
				outputPixels[rowPlusj].r = topPixels[rowPlusj].r; 
				outputPixels[rowPlusj].g = topPixels[rowPlusj].g; 
				outputPixels[rowPlusj].b = topPixels[rowPlusj].b; 
			} else {	
				iMinusHTimesbottomWidthPlusj = (i-h)*bottomWidth + j;
				outputPixels[rowPlusj].r = bottomPixels[iMinusHTimesbottomWidthPlusj].r; 
				outputPixels[rowPlusj].g = bottomPixels[iMinusHTimesbottomWidthPlusj].g; 
				outputPixels[rowPlusj].b = bottomPixels[iMinusHTimesbottomWidthPlusj].b; 
			}
		}
	}	
	Logger::LogEvent("TBConcat: done executing");
}

void Blender::Execute(){
	
	Logger::LogEvent("Blender: about to update input1");
	if(this->getInput() != NULL && this->getInput()->hasNotBeenUpdated()){
	       	this->getInput()->Update(); 
	};
	Logger::LogEvent("Blender: done updating input1");
	
	Logger::LogEvent("Blender: about to update input2");
	if(this->getInput2() != NULL && this->getInput2()->hasNotBeenUpdated()){ 
		this->getInput2()->Update();
	};
	Logger::LogEvent("Blender: done updating input2");
	
	const Image* input1 = getInput();  Pixel* input1Pixels = input1->getPixels();
	const Image* input2 = getInput2(); Pixel* input2Pixels = input2->getPixels();
	Image* output = GetOutput();
	
	if(input1 == NULL){
		char msg[1024];
		sprintf(msg, "%s: Invalid input 1!", getSourceName());
		DataFlowException e(getSourceName(), msg);
		throw e;
	} else if (input2 == NULL){
		char msg[1024];
		sprintf(msg, "%s: Invalid input 2!", getSourceName());
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	//Heights and widths of images must be the same	
	if(input1->getHeight() != input2->getHeight()){
		char msg[1024];
		sprintf(msg, "%s: Heights much match: %d %d", getSourceName(), input1->getHeight(), input2->getHeight());
		DataFlowException e(getSourceName(), msg);
		throw e;

	} else if (input1->getWidth() != input2->getWidth()){	
		char msg[1024];
		sprintf(msg, "%s: Widths much match: %d %d", getSourceName(), input1->getWidth(), input2->getWidth());
		DataFlowException e(getSourceName(), msg);
		throw e;
	} else if (factor > 1.0 || factor < 0.0){
		char msg[1024];
		sprintf(msg, "Invalid factor for Blender: %f", factor);
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	Logger::LogEvent("Blender: about to execute");

	int w = input1->getWidth();
	int h = input1->getHeight();

	output->ResetSize(w, h);	
	output->setMaxval(input1->getMaxval());
	output->setMagicnum(input1->getMagicnum());
	Pixel* outputPixels = output->getPixels();

	float factor = this->getFactor();
	float complimentFactor = 1.0 - factor;

	//Variables to reduce arithmetic instructions
	int row = 0; int rowPlusj = 0;

	//Width and height are the same for both images
	for(int i = 0; i < h; i++){
		row = i*w;
		for(int j = 0; j < w; j++){
			rowPlusj = row + j;
			outputPixels[rowPlusj].r = (factor * (input1Pixels[rowPlusj].r)) + ((complimentFactor) * (input2Pixels[rowPlusj].r));
			outputPixels[rowPlusj].g = (factor * (input1Pixels[rowPlusj].g)) + ((complimentFactor) * (input2Pixels[rowPlusj].g));
			outputPixels[rowPlusj].b = (factor * (input1Pixels[rowPlusj].b)) + ((complimentFactor) * (input2Pixels[rowPlusj].b));
		}
	}	
	Logger::LogEvent("Blender: done executing");
}

void Mirror::Execute(void){
	
	Logger::LogEvent("Mirror: about to update input1");
	if(this->getInput() != NULL && this->getInput()->hasNotBeenUpdated()){
	       	this->getInput()->Update(); 
	};
	Logger::LogEvent("Mirror: done updating input1");
	
	if(this->getInput() == NULL){	
		char msg[1024];
		sprintf(msg, "Mirror: no input1!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	Logger::LogEvent("Mirror: about to execute");

	const Image* input = getInput(); Pixel* inputPixels = input->getPixels();
	Image* output = GetOutput();

	int w = input->getWidth();
	int h = input->getHeight();

	output->ResetSize(w, h);
	output->setMaxval(input->getMaxval());
	output->setMagicnum(input->getMagicnum());	
	Pixel* outputPixels = output->getPixels();

	int8_t* inputChars = (int8_t*) malloc(sizeof(int8_t)*w*h*3);
	int8_t* outputChars = (int8_t*) malloc(sizeof(int8_t)*w*h*3);
	
	int i3 = 0;
	//fill in unsigned char array with input pixel's rgb values	
	for(int i = 0; i < w*h; i++){
		i3 = i*3;
		inputChars[(i3)                   ] = inputPixels[i].r;
		inputChars[(i3)+    sizeof(int8_t)] = inputPixels[i].g;
		inputChars[(i3)+(2* sizeof(int8_t))] = inputPixels[i].b;
	}

	//the following code within the loop will be parallelized
	for(int i = 0; i < h*3; i++){
		ispc::invert(inputChars, outputChars, w, i);
	}
	
	//Rewrite to output pixels	
	for(int i = 0; i < w*h; i++){
		i3 = i*3;
		outputPixels[i].r = outputChars[i3  ];
		outputPixels[i].g = outputChars[i3+(sizeof(int8_t))];
		outputPixels[i].b = outputChars[i3+(2*sizeof(int8_t))];
	}
	
	free(inputChars);
	free(outputChars);

	/*
 	//Original code
	for(int i = 0; i < h; i++){
		row = i*w;
		for(int j = 0; j < w; j++){
			rowPlusj = row + j;	
			oppositeRowPlusj = ((i+1)*w) - 1 - j;
			outputPixels[oppositeRowPlusj].r = inputPixels[rowPlusj].r;
			outputPixels[oppositeRowPlusj].g = inputPixels[rowPlusj].g;
			outputPixels[oppositeRowPlusj].b = inputPixels[rowPlusj].b;
		}
	}
	*/

	Logger::LogEvent("Mirror: done executing");
}

void Rotate::Execute(void){
	
	Logger::LogEvent("Rotate: about to update input1");
	if(this->getInput() != NULL && this->getInput()->hasNotBeenUpdated()){ 
	       	this->getInput()->Update(); 
       	};
	Logger::LogEvent("Rotate: done updating input1");
	
	if(this->getInput() == NULL){	
		char msg[1024];
		sprintf(msg, "Rotate: no input1!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	Logger::LogEvent("Rotate: about to execute");
	
	const Image* input = getInput(); Pixel* inputPixels = input->getPixels();
	Image* output = GetOutput(); 

	int w = input->getWidth();
	int h = input->getHeight();

	//Height and width swapped because of rotation
	output->ResetSize(h, w);
	output->setMaxval(input->getMaxval());
	output->setMagicnum(input->getMagicnum());	
	Pixel* outputPixels = output->getPixels();
	
	//Variables to reduce arithmetic instructions
	int row = 0;
	int rowPlusj = 0;
	int outColumn = 0;
	int outputWidth = output->getWidth();
	
	for(int i = 0; i < h; i++){
		row = i*w;
		for(int j = 0; j < w; j++){
			rowPlusj = row + j;
			outColumn = (outputWidth*(j+1)) - 1 - i;
			outputPixels[outColumn].r = inputPixels[rowPlusj].r;
			outputPixels[outColumn].g = inputPixels[rowPlusj].g;
			outputPixels[outColumn].b = inputPixels[rowPlusj].b;
		}
	}	
	Logger::LogEvent("Rotate: done executing");
}

void Subtract::Execute(void){

	Logger::LogEvent("Subtract: about to update input1");	
	if(this->getInput() != NULL && this->getInput()->hasNotBeenUpdated()){ 
	       	this->getInput()->Update(); 
       	};
	Logger::LogEvent("Subtract: done updating input1");
	
	Logger::LogEvent("Subtract: about to update input2");
	if(this->getInput2() != NULL && this->getInput2()->hasNotBeenUpdated()){ 
		this->getInput2()->Update();
	};
	Logger::LogEvent("Subtract: done updating input2");
	
	if(this->getInput() == NULL){	
		char msg[1024];
		sprintf(msg, "Subtract: no input1!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	} else if (this->getInput2() == NULL){
		char msg[1024];
		sprintf(msg, "Subtract: no input2!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	Logger::LogEvent("Subtract: about to execute");
	
	const Image* input1 = getInput();  Pixel* input1Pixels = input1->getPixels();
	const Image* input2 = getInput2(); Pixel* input2Pixels = input2->getPixels();
	Image* output = GetOutput();

	int w = input1->getWidth();
	int h = input2->getHeight();

	if(input1->getWidth() != input2->getWidth()){
		char msg[1024];
		sprintf(msg, "Subtract: Inputs must have the same widths!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	} else if (input1->getHeight() != input2->getHeight()){
		char msg[1024];
		sprintf(msg, "Subtract: Inputs must have the same heights!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	output->ResetSize(w, h);
	output->setMaxval(input1->getMaxval());
	output->setMagicnum(input1->getMagicnum());
	Pixel* outputPixels = output->getPixels();

	//Variables to reduce arithmetic instructions
	int row = 0;
	int rowPlusj = 0;
	unsigned char input1_RedVal = 0;   unsigned char input2_RedVal = 0;
	unsigned char input1_GreenVal = 0; unsigned char input2_GreenVal = 0;
	unsigned char input1_BlueVal = 0;  unsigned char input2_BlueVal = 0;

	for(int i = 0; i < h; i++){
		row = i*w;
		for(int j = 0; j < w; j++){
			rowPlusj = row + j;	

			input1_RedVal = input1Pixels[rowPlusj].r;	
			input1_GreenVal = input1Pixels[rowPlusj].g;	
			input1_BlueVal = input1Pixels[rowPlusj].b;	

			input2_RedVal = input2Pixels[rowPlusj].r;	
			input2_GreenVal = input2Pixels[rowPlusj].g;	
			input2_BlueVal = input2Pixels[rowPlusj].b;

			//If the result of subtracting would be negative, set to zero instead
			if(input1_RedVal > input2_RedVal) {	
				outputPixels[rowPlusj].r = input1_RedVal - input2_RedVal;
			} else {
				outputPixels[rowPlusj].r = 0;
			}

			if(input1_GreenVal > input2_GreenVal) {	
				outputPixels[rowPlusj].g = input1_GreenVal - input2_GreenVal;
			} else {
				outputPixels[rowPlusj].g = 0;
			}

			if(input1_BlueVal > input2_BlueVal) {	
				outputPixels[rowPlusj].b = input1_BlueVal - input2_BlueVal;
			} else {
				outputPixels[rowPlusj].b = 0;
			}
				
		}
	}

	Logger::LogEvent("Subtract: done executing");
}

void Grayscale::Execute(void){
	
	Logger::LogEvent("Grayscale: about to update input1");	
	if(this->getInput() != NULL && this->getInput()->hasNotBeenUpdated()){ 
	       	this->getInput()->Update(); 
       	};
	Logger::LogEvent("Grayscale: done updating input1");

	if(this->getInput() == NULL){	
		char msg[1024];
		sprintf(msg, "Grayscale: no input1!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	} 

	Logger::LogEvent("Grayscale: about to execute");
	
	const Image* input = getInput(); Pixel* inputPixels = input->getPixels();
	Image* output = GetOutput(); 

	int w = input->getWidth();
	int h = input->getHeight();
	
	//Variables to reduce arithmetic instructions
	int row = 0;
	int rowPlusj = 0;
	int grayVal = 0;

	output->ResetSize(w, h);
	output->setMaxval(input->getMaxval());
	output->setMagicnum(input->getMagicnum());	
	Pixel* outputPixels = output->getPixels();

	for(int i = 0; i < h; i++){
		row = i*w;
		for(int j = 0; j < w; j++){
			rowPlusj = row + j;
			grayVal = (inputPixels[rowPlusj].r / 5) + (inputPixels[rowPlusj].g / 2) + (inputPixels[rowPlusj].b / 4);
			outputPixels[rowPlusj].r = grayVal;
			outputPixels[rowPlusj].g = grayVal;
			outputPixels[rowPlusj].b = grayVal;
		}	
	} 
	Logger::LogEvent("Grayscale: done executing");
}

void Blur::Execute(void){

	Logger::LogEvent("Blur: about to update input1");	
	if(this->getInput() != NULL && this->getInput()->hasNotBeenUpdated()){ 
	       	this->getInput()->Update(); 
       	};
	Logger::LogEvent("Blur: done updating input1");

	if(this->getInput() == NULL){	
		char msg[1024];
		sprintf(msg, "Blur: no input1!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	}
	
	Logger::LogEvent("Blur: about to execute");

	const Image* input = getInput(); Pixel* inputPixels = input->getPixels();
	Image* output = GetOutput(); 

	int w = input->getWidth();
	int h = input->getHeight();
	
	//Variables to reduce arithmetic instructions
	int row = 0;
	int rowPlusj = 0;

	//used for indexing adjacent pixels
	int pixelOne, pixelTwo, pixelThree, pixelFour, pixelFive, pixelSix, pixelSeven, pixelEight = 0;

	output->ResetSize(w, h);
	output->setMaxval(input->getMaxval());
	output->setMagicnum(input->getMagicnum());	
	Pixel* outputPixels = output->getPixels();

	//First, copy entire image
	for(int i = 0; i < h; i++){
		row = i*w;
		for(int j = 0; j < w; j++){
			rowPlusj = row + j;
			outputPixels[rowPlusj].r = inputPixels[rowPlusj].r;	
			outputPixels[rowPlusj].g = inputPixels[rowPlusj].g;	
			outputPixels[rowPlusj].b = inputPixels[rowPlusj].b;	
		}
	}

	//Second, modify inner pixels (row+1)(col+1) to (row-1)(col-1)
	for(int i = 1; i < h-1; i++){
		row = i*w;
		for(int j = 1; j < w-1; j++){
			rowPlusj = row + j;
			pixelOne =   rowPlusj - w - 1; 		//left-upper-diagonal
			pixelTwo =   rowPlusj - w;		//above pixel
			pixelThree = rowPlusj - w + 1;		//right-upper-diagonal
			pixelFour =  rowPlusj - 1;		//left pixel
			pixelFive =  rowPlusj + 1;		//right pixel
			pixelSix =   rowPlusj + w - 1;		//left-lower-diagonal
			pixelSeven = rowPlusj + w; 		//below pixel
			pixelEight = rowPlusj + w + 1;		//right-lower-diagonal

			outputPixels[rowPlusj].r = (inputPixels[pixelOne].r / 8)  + (inputPixels[pixelTwo].r / 8) +   (inputPixels[pixelThree].r / 8) +
						   (inputPixels[pixelFour].r / 8) +    /* Original pixel here*/       (inputPixels[pixelFive].r / 8) +
						   (inputPixels[pixelSix].r / 8) +  (inputPixels[pixelSeven].r / 8) + (inputPixels[pixelEight].r / 8);
			
			outputPixels[rowPlusj].g = (inputPixels[pixelOne].g / 8)  + (inputPixels[pixelTwo].g / 8) +   (inputPixels[pixelThree].g / 8) +
						   (inputPixels[pixelFour].g / 8) +    /* Original pixel here*/       (inputPixels[pixelFive].g / 8) +
						   (inputPixels[pixelSix].g / 8) +  (inputPixels[pixelSeven].g / 8) + (inputPixels[pixelEight].g / 8);
			
			outputPixels[rowPlusj].b = (inputPixels[pixelOne].b / 8)  + (inputPixels[pixelTwo].b / 8) +   (inputPixels[pixelThree].b / 8) +
						   (inputPixels[pixelFour].b / 8) +    /* Original pixel here*/       (inputPixels[pixelFive].b / 8) +
						   (inputPixels[pixelSix].b / 8) +  (inputPixels[pixelSeven].b / 8) + (inputPixels[pixelEight].b / 8);
		}
	}
	Logger::LogEvent("Blur: done executing");
}

void Color::Execute(void){
	
	Logger::LogEvent("Color: about to execute");	
	//if(this->colorWidth <= 0 || this->colorHeight <= 0 || ( 255 < this->rVal || this->rVal < 0 )|| (255 < this->gVal || this->gVal < 0) || (255 < this->bVal || this->bVal < 0)){
	if(this->colorWidth <= 0 || this->colorHeight <= 0){
		char msg[1024];
		sprintf(msg, "Color: invalid color parameters!");
		DataFlowException e(getSourceName(), msg);
		throw e;
	}

	int w = this->colorWidth;
	int h = this->colorHeight;
	Image* output = GetOutput(); 	
	
	output->ResetSize(w, h);
	output->setMaxval(255);
	output->setMagicnum((char*)"P6");

	Pixel* outputPixels = output->getPixels();

	//Variables to reduce arithmetic instructions
	int row = 0;
	int rowPlusj = 0;

	for(int i = 0; i < h; i++){
		row = i*w;
		for(int j = 0; j < w; j++){
			rowPlusj = row + j;
			outputPixels[rowPlusj].r = this->rVal;	
			outputPixels[rowPlusj].g = this->gVal;	
			outputPixels[rowPlusj].b = this->bVal;	
		}
	}

	Logger::LogEvent("Color: done executing");	
}

int Filter::getRowAndColumn(int i, int j){
	return (i*this->GetOutput()->getWidth()+j);
}
