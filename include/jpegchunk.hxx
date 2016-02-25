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


class JPEGChunk : public Chunk {
	CHUNK_CONSTRUCTORS(JPEGChunk);

protected:
	virtual std::string data(Type)                           const override;
	virtual std::string name(Type, const std::string&)       const override;

	virtual std::string printableTypeCode()                  const override;
	virtual std::string defaultChunkName(const std::string&) const override;

	static bool         dataFreeTag(unsigned char);
	       unsigned int exifRational(const char*, bool)      const;
	                int exifSRational(const char*, bool)     const;

public:
	JPEGChunk(std::istream&);

	virtual bool        required()           const override;
	virtual void        write(std::ostream&) const override;
};


#endif
