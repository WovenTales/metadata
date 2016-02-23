#ifndef PNG_CHUNK_H
#define PNG_CHUNK_H


#include <chunk.hxx>

#include <istream>
#include <ostream>
#include <string>


class PNGChunk : public Chunk {
	// Inherit constructors
	using Chunk::Chunk;

protected:
	char crc[4];

	virtual std::string printableTypeCode()                  const override;
	virtual std::string defaultChunkName(const std::string&) const override;

public:
	PNGChunk(const PNGChunk&);
	PNGChunk(PNGChunk&&);
	PNGChunk(std::istream&);

	virtual std::string data()               const override;
	virtual bool        required()           const override;
	virtual void        write(std::ostream&) const override;
};


#endif
