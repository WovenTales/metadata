#ifndef PNG_CHUNK_H
#define PNG_CHUNK_H


#include <chunk.hxx>
#include <chunktype.hxx>

#include <istream>
#include <ostream>
#include <string>


class PNGChunk : public Chunk {
	CHUNK_CONSTRUCTORS(PNGChunk);

protected:
	char crc[4];

	virtual std::string data(ChunkType)                     const override;
	virtual std::string name(ChunkType, const std::string&) const override;

	virtual std::string printableTypeCode()                 const override;
	virtual std::string defaultChunkName()                  const override;

public:
	PNGChunk(const PNGChunk&);
	PNGChunk(PNGChunk&&);
	PNGChunk(std::istream&);

	virtual bool        required()           const override;
	virtual void        write(std::ostream&) const override;
};


#endif
