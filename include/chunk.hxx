#ifndef CHUNK_H
#define CHUNK_H


#include <chunkiterator.hxx>
#include <chunktype.hxx>
class Metadata;

#include <cstddef>

#include <istream>
#include <list>
#include <map>
#include <ostream>
#include <string>
#include <utility>


#define CHUNK_CONSTRUCTORS(_SUBTYPE_)    using Chunk::Chunk;


struct Chunk {
private:
	const std::map< std::string, std::pair< std::string, ChunkType > >& typeMap;

	std::list< Metadata* > subChunks;

	static std::string sanitize(std::string);

	friend ChunkIterator;

protected:
	unsigned int   length   = 0;
	std::string    typeCode;
	char*          raw      = NULL;

	Chunk(std::istream&, const std::map< std::string, std::pair< std::string, ChunkType > >&);

	        std::string hexString(bool = false, unsigned int = 0, unsigned int = 0,
				unsigned int* = NULL, unsigned int* = NULL) const;

	virtual std::string data(ChunkType)                      const;
	virtual std::string name(ChunkType, const std::string&)  const;

	virtual std::string printableTypeCode()                  const = 0;
	virtual std::string defaultChunkName(const std::string&) const = 0;

public:
	Chunk(const Chunk&);
	Chunk(Chunk&&);

	virtual ~Chunk();

	        std::string  data()               const;
	        std::string  name()               const;
	        ChunkType    type()               const;
	virtual bool         required()           const = 0;

	virtual void         write(std::ostream&) const = 0;

	static  unsigned int readBytes(const char*, unsigned char, bool = true);

	        size_t        size()               const noexcept;
	        bool          empty()              const noexcept;

	        ChunkIterator begin()                    noexcept { return ChunkIterator(this, false);   };
	        ChunkIterator end()                      noexcept { return ++ChunkIterator(this, true);  };
	        ChunkIterator rbegin()                   noexcept { return ChunkIterator(this, true);    };
	        ChunkIterator rend()                     noexcept { return --ChunkIterator(this, false); };
};


#endif
