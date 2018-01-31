//#include <source.h>
#include <sink.h>
#include <stdio.h>

class Filter : public Source, public Sink {

	public:
		 Filter(){};
		~Filter(){};

		virtual void Execute() = 0;
		int getRowAndColumn(int, int);

	protected:
		char* filterName = NULL;
};

class Invert : public Filter {

	public:
		Invert(){this->GetOutput()->setFilter(this);};
		~Invert(){};
		virtual void Execute(void);
		const char* getSourceName();
};

class Shrinker : public Filter {

	public:
		Shrinker(){this->GetOutput()->setFilter(this);};
		~Shrinker(){};
		virtual void Execute(void);
		const char* getSourceName();

};

class LRConcat : public Filter {

	public:
		LRConcat(){this->GetOutput()->setFilter(this);};
		~LRConcat(){};
		virtual void Execute(void);
		const char* getSourceName();

};

class TBConcat : public Filter {

	public:
		TBConcat(){this->GetOutput()->setFilter(this);};
		~TBConcat(){};
		virtual void Execute(void);
		const char* getSourceName();

};

class Blender : public Filter {

	public:
		Blender(){this->GetOutput()->setFilter(this);};
		~Blender(){};
		virtual void Execute(void);
		float getFactor(void){return this->factor;};
		void SetFactor(float);
		const char* getSourceName();

	private:
		float factor;
};

class Mirror : public Filter {

	public:
		Mirror(){this->GetOutput()->setFilter(this);};
		~Mirror(){};
		virtual void Execute(void);
		const char* getSourceName();

	private:
};

class Rotate : public Filter {

	public:
		Rotate(){this->GetOutput()->setFilter(this);};
		~Rotate(){};
		virtual void Execute(void);
		const char* getSourceName();

	private:
};

class Subtract : public Filter {

	public:
		Subtract(){this->GetOutput()->setFilter(this);};
		~Subtract(){};
		virtual void Execute(void);
		const char* getSourceName();

	private:
};

class Grayscale : public Filter {

	public:
		Grayscale(){this->GetOutput()->setFilter(this);};
		~Grayscale(){};
		virtual void Execute(void);
		const char* getSourceName();

	private:
};

class Blur : public Filter {

	public:
		Blur(){this->GetOutput()->setFilter(this);};
		~Blur(){};
		virtual void Execute(void);
		const char* getSourceName();

	private:
};

class Color : public Source {

	public:
		Color(){this->GetOutput()->setColor((this));};
		Color(int, int, unsigned char, unsigned char, unsigned char);
		
		virtual void Execute(void);
		virtual void Update(void);
		const char* getSourceName();

		int  getWidth(void)  { return colorWidth; };
		int  getHeight(void) { return colorHeight; };
		unsigned char getRed(void)    { return rVal; };
		unsigned char getGreen(void)  { return gVal; };
		unsigned char getBlue(void)   { return bVal; };

	private:
		int colorWidth = 0;
		int colorHeight = 0;
		unsigned char rVal = 0;
		unsigned char gVal = 0;
		unsigned char bVal = 0;
};

class CheckSum : public Sink {

	public:
		void OutputCheckSum(char const* str);
		virtual void Execute(void);
		const char* getSourceName();

	private:
};
