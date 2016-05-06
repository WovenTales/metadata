#ifndef PNG_CHUNK_H
#define PNG_CHUNK_H


#include <chunk.hxx>

#include <istream>
#include <ostream>
#include <string>


namespace metadata {


//! Specialize Chunk for tags in PNG images
class PNGChunk : public Chunk {
	CHUNK_CONSTRUCTORS(PNGChunk)

protected:
	//! Save the four-byte checksum for proper writing
	char crc[4];

	//! \copybrief Chunk::data
	virtual std::string data(Type)                           const override;
	//! \copybrief Chunk::name
	virtual std::string name(Type, const std::string&)       const override;

	//! \copybrief Chunk::printableTypeCode
	virtual std::string printableTypeCode()                  const override;
	//! \copybrief Chunk::defaultChunkName
	virtual std::string defaultChunkName(const std::string&) const override;

public:
	//! Parse the next tag in the stream, following the PNG specification
	PNGChunk(std::istream&);
	//! Standard copy constructor
	PNGChunk(const PNGChunk&);
	//! Standard move constructor
	PNGChunk(PNGChunk&&);

	//! \copybrief Chunk::required
	virtual bool        required()           const override;
	//! \copybrief Chunk::write
	virtual void        write(std::ostream&) const override;
};


}
#endif
