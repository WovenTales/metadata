#ifndef PNG_CHUNK_H
#define PNG_CHUNK_H


#include <chunk.hxx>

#include <istream>
#include <ostream>
#include <string>


class PNGChunk : public Chunk {
	// Inherit constructors
	using Chunk::Chunk;

public:
	PNGChunk(std::istream&);

	virtual std::string data()               const override;
	virtual std::string name()               const override;
	virtual Type        type()               const override;
	virtual bool        required()           const override;

	virtual void        write(std::ostream&) const override;
};


#endif
