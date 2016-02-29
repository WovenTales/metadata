#ifndef METADATATAG_H
#define METADATATAG_H


class Chunk;
#include <chunktype.hxx>
class Metadata;
class MetadataIterator;

#include <cstddef>

#include <fstream>
#include <list>
#include <string>


struct MetadataTag {
private:
	std::list< Chunk* > ref;

	friend Metadata;
	friend MetadataIterator;

public:
	ChunkType   type;
	std::string label;
	std::string data;

	MetadataTag(Chunk*);
	virtual ~MetadataTag();

	void addChunk(Chunk*);

	bool   required() const;
	void   write(std::ofstream&);

	size_t size()     const;
	bool   empty()    const;

	std::list< Chunk* >::iterator begin() noexcept { return ref.begin(); };
	std::list< Chunk* >::iterator end()   noexcept { return ref.end();   };
};


#endif
