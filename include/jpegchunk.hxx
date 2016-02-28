#ifndef JPEG_CHUNK_H
#define JPEG_CHUNK_H


#include <chunk.hxx>
#include <chunktype.hxx>

#include <istream>
#include <ostream>
#include <string>


class JPEGChunk : public Chunk {
	CHUNK_CONSTRUCTORS(JPEGChunk);

protected:
	virtual std::string  data(ChunkType)                      const override;
	virtual std::string  name(ChunkType, const std::string&)  const override;
	virtual ChunkType    type()                               const override;

	virtual std::string  printableTypeCode()                  const override;

	static  bool         dataFreeTag(unsigned char);
	static  std::string  hexByte(unsigned char);
	static  unsigned int charValue(unsigned char);

public:
	JPEGChunk(std::istream&);

	virtual bool         required()           const override;
	virtual void         write(std::ostream&) const override;
};


#endif
