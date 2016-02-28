#ifndef EXIF_CHUNK_H
#define EXIF_CHUNK_H


#include <chunk.hxx>
#include <chunktype.hxx>

#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <utility>


#define EXIF_BYTE       1
#define EXIF_ASCII      2
#define EXIF_SHORT      3
#define EXIF_LONG       4
#define EXIF_RATIONAL   5
#define EXIF_UNDEFINED  7
#define EXIF_SLONG      9
#define EXIF_SRATIONAL  10


class ExifChunk : public Chunk {
	CHUNK_CONSTRUCTORS(ExifChunk);

private:
	bool bigEndian;
	unsigned int exifIndex;

	unsigned int nextOffset = 0;

protected:
	virtual std::string  data(ChunkType)      const override;

	virtual std::string  printableTypeCode()  const override;
	virtual std::string  defaultChunkName()   const override;

public:
	ExifChunk(std::istream&, bool, unsigned int&);

	virtual bool         required()           const override;
	virtual ChunkType    type()               const override;
	virtual void         write(std::ostream&) const override;

	        unsigned int getNextOffset()      const { return nextOffset; };

	static  std::map< std::string, std::pair< std::string, ChunkType > > convertMap(
			std::map< std::string, std::pair< std::string, ChunkType > >, bool);
	static  std::string  toBigEndian(std::string, bool);

	static  float        exifRational(const char*, bool);
	static  float        exifSRational(const char*, bool);
};


#endif
