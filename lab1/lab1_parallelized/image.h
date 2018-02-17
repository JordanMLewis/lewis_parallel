/* Header file for image class */
#ifndef IMAGE_H
#define IMAGE_H
#include <stdlib.h>

class PNMreader;
class Filter;
class Color;

class Pixel{
	public:
		unsigned char r;
		unsigned char g;
		unsigned char b;
};

class Image{
	
	public:
		Image(void);
		Image(char* magic, int w, int h, int max);
		Image(Image &i);
		~Image(void);

		void ResetSize(int w, int h);
		void setMaxval(int val);
		void setMagicnum(char* num);

		int getWidth(void) const;
		int getHeight(void)const;
		int getMaxval(void)const;
		char* getMagicnum(void)const;
		Pixel* getPixels(void)const;

		//Demand-driven execution method
		void Update(void) const;
		bool hasNotBeenUpdated() const{ return !updated; };
		
		void setFilter(Filter* fp)    {this->myFilter = fp;};
		Filter* getFilter(void)  const{return myFilter;};
		void setColor(Color* cp)      {this->color = cp;};
		void setReader(PNMreader* r)  {this->reader = r;};

	private:
		Filter* myFilter = NULL;
		PNMreader* reader = NULL;
		Color* color = NULL;
		mutable bool updated = false;
		char* magicNum;
		int width;
		int height;
		int maxVal;
		Pixel* pixels;
};

#endif
