#ifndef JPEG_CHUNK_H
#define JPEG_CHUNK_H


#include <chunk.hxx>

#include <istream>
#include <ostream>
#include <string>


class JPEGChunk : public Chunk {
	CHUNK_CONSTRUCTORS(JPEGChunk);

protected:
	virtual std::string data(Type)                           const override;

	virtual std::string printableTypeCode()  const override;
	virtual std::string defaultChunkName(const std::string&) const override;

	static bool dataFreeTag(unsigned char);

public:
	JPEGChunk(std::istream&);

	virtual bool        required()           const override;
	virtual void        write(std::ostream&) const override;
};


#endif
