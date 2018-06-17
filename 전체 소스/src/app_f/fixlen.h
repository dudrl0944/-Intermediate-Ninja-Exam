// fixlen.h
#ifndef FIXLEN_H
#define FIXLEN_H

#include <stdlib.h>
#include <iostream.h>
#include "iobuffer.h"

class FixedLengthBuffer: public IOBuffer{
   public:
	FixedLengthBuffer (int recordSize = 1000);
	FixedLengthBuffer (const FixedLengthBuffer & buffer); 

	void Clear ();
	int Read (istream &);
	int Write (ostream &) const;
	int ReadHeader (istream &); 
	int WriteHeader (ostream &) const; 
	void Print (ostream &) const;
	int SizeOfBuffer () const;
 protected:
 	int Init (int recordSize);
	int ChangeRecordSize (int recordSize);
};

inline FixedLengthBuffer :: FixedLengthBuffer (const FixedLengthBuffer & buffer)
: IOBuffer (buffer)
{
	Init (buffer . BufferSize);
}

#endif
