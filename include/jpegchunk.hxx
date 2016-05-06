#ifndef JPEG_CHUNK_H
#define JPEG_CHUNK_H


#include <chunk.hxx>

#include <istream>
#include <ostream>
#include <string>


#define EXIF_BYTE       1
#define EXIF_ASCII      2
#define EXIF_SHORT      3
#define EXIF_LONG       4
#define EXIF_RATIONAL   5
#define EXIF_UNDEFINED  7
#define EXIF_SLONG      9
#define EXIF_SRATIONAL  10


namespace metadata {


//! Specialize Chunk for tags in JPEG images
class JPEGChunk : public Chunk {
	CHUNK_CONSTRUCTORS(JPEGChunk)

protected:
	//! \copybrief Chunk::data
	virtual std::string  data(Type)                           const override;
	//! \copybrief Chunk::name
	virtual std::string  name(Type, const std::string&)       const override;

	//! \copybrief Chunk::printableTypeCode
	virtual std::string  printableTypeCode()                  const override;
	//! \copybrief Chunk::defaultChunkName
	virtual std::string  defaultChunkName(const std::string&) const override;

	//! Encapsulate calculations to determine whether a tag has associated data
	static  bool         dataFreeTag(unsigned char);
	//! Read the eight bytes at the pointer as representing an EXIF "float"
	        unsigned int exifRational(const char*, bool)      const;
	//! Read the eight bytes at the pointer as representing a signed EXIF "float"
	                 int exifSRational(const char*, bool)     const;

public:
	//! Parse the next tag in the stream, following the JPEG specification
	JPEGChunk(std::istream&);

	//! \copybrief Chunk::required
	virtual bool         required()                           const override;
	//! \copybrief Chunk::write
	virtual void         write(std::ostream&)                 const override;
};


}
#endif
