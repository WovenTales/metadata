#ifndef METADATAITERATOR_H
#define METADATAITERATOR_H


class Chunk;
#include <chunkiterator.hxx>
class Metadata;
#include <metadatatag.hxx>

#include <iterator>
#include <list>


class MetadataIterator : public std::iterator< std::bidirectional_iterator_tag, MetadataTag > {
private:
	enum struct State {
		BEGIN,
		TOP,
		INNER,
		END
	};

	Metadata*                          data = NULL;
	std::list< MetadataTag >::iterator top;
	std::list< Chunk* >::iterator      chunk;
	ChunkIterator                      inner;

	State                              state;

	// Save endpoints to avoid repeated calculations
	std::list< MetadataTag >::iterator tStart;
	std::list< MetadataTag >::iterator tEnd;
	std::list< Chunk* >::iterator      cStart;
	std::list< Chunk* >::iterator      cEnd;

	void initialize(bool);
	void step(bool);
	void stepInner(bool);
	void stepChunk(bool);

	void updateToTag(bool);
	void updateToChunk(bool);

public:
	MetadataIterator(Metadata* = NULL, bool = false);

	MetadataIterator& operator++();
	MetadataIterator& operator--();
	MetadataTag*      operator->();
	MetadataTag&      operator*() { return *(operator->()); };
	bool              operator==(const MetadataIterator&)     noexcept;
	bool              operator!=(const MetadataIterator& rhs) noexcept { return !(operator==(rhs)); };

	bool              atStart() const;
	bool              atEnd()   const;
};



#endif
