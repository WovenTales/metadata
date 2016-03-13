#ifndef EXIF_IFD_CHUNK_H
#define EXIF_IFD_CHUNK_H


#include <chunk.hxx>
#include <chunktype.hxx>

#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <utility>


class ExifIFDChunk : public Chunk {
	CHUNK_CONSTRUCTORS(ExifIFDChunk);

private:
	bool bigEndian;
	unsigned int exifType;
	unsigned int exifCount;
	unsigned int exifOffset = 0;

	        unsigned int intTypeCode()                   const;
	        unsigned int dataTypeLength()                const;
	                 int simpleValue(const char* = NULL) const;

protected:
	virtual std::string  data(ChunkType)                 const override;
	virtual std::string  printableTypeCode()             const override;

public:
	ExifIFDChunk(std::istream&, bool);
	ExifIFDChunk(const ExifIFDChunk&);

	virtual bool         required()           const override;
	virtual void         write(std::ostream&) const override;
};


#endif
