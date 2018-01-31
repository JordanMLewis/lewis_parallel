#include "source.h"

class PNMreader : public Source {
	
	public:
		PNMreader(char*);
		~PNMreader();
	
		void Execute(void);
		void Update(void);
		void getFilename(void);
		void setFilename(void);
		const char* getSourceName();

	private:
		char* filename;	
};
