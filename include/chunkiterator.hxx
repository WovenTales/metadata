#ifndef CHUNKITERATOR_H
#define CHUNKITERATOR_H


class Chunk;
class Metadata;
class MetadataTag;
class MetadataIterator;

#include <iterator>
#include <list>


// TODO Good idea to consolidate with MetadataIterator into template/supertype TagIterator
class ChunkIterator : public std::iterator< std::bidirectional_iterator_tag, MetadataTag > {
private:
	enum struct State {
		BEGIN,
		VALID,
		END
	};

	Chunk*                           chunk = NULL;
	std::list< Metadata* >::iterator top;
	MetadataIterator*                inner = NULL;

	State                            state;

	// Save endpoints to avoid repeated calculations
	std::list< Metadata* >::iterator tStart;
	std::list< Metadata* >::iterator tEnd;

	void initialize(bool);
	void step(bool);

	void updateToMetadata(bool);

public:
	ChunkIterator(Chunk* = NULL, bool = false);

	ChunkIterator& operator++();
	ChunkIterator& operator--();
	MetadataTag*   operator->();
	MetadataTag&   operator*() { return *(operator->()); };
	bool           operator==(const ChunkIterator&)     noexcept;
	bool           operator!=(const ChunkIterator& rhs) noexcept { return !(operator==(rhs)); };

	bool           atStart() const;
	bool           atEnd()   const;
};



#endif
