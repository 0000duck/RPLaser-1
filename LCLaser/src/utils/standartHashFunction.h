/***************************************************************************
                          standartHashFunction.h  -  description
                             -------------------
    begin                : May Jun. 6 2015
    copyright            : (C) 2015 by XI'AN JIAOTONG UNIVERSITY
    author               : Cui Bin
 ***************************************************************************/
#ifndef STANDARDHASHFUNCTION_INCLUDED
#define STANDARDHASHFUNCTION_INCLUDED

//!Generic hash function:
template <class Source>
class StandardHashFunction
	{
	/* Static methods: */
	public:
	static size_t rawHash(const Source& source)
	{
		/* Standard hasher reinterprets object as bit pattern and runs simple CRC on that: */
		size_t result=0;
		size_t sourceSize=sizeof(source);
		
		/* Process complete words first: */
		const size_t* h1Ptr;
		for(h1Ptr=(const size_t*)&source; sourceSize >= sizeof(size_t); sourceSize-=sizeof(size_t), ++h1Ptr)
			result+=*h1Ptr;
		
		/* Process rest of source byte-wise: */
		size_t lastBytes=0;
		const unsigned char* h2Ptr;
		for(h2Ptr=(const unsigned char*)h1Ptr;sourceSize>0;sourceSize-=sizeof(unsigned char),++h2Ptr)
			lastBytes=(lastBytes<<8)+*h2Ptr;
		result+=lastBytes;
		
		return result;
	}
	static size_t hash(const Source& source,size_t tableSize)
	{
		return rawHash(source)%tableSize;
	}
	};


#endif // STANDARDHASHFUNCTION_INCLUDED
