#ifndef METADATA_H
#define METADATA_H


#include <chunk.hxx>
#include <chunktype.hxx>
class MetadataFactory;
#include <metadatafiletype.hxx>
#include <metadataiterator.hxx>
#include <metadatatag.hxx>

#include <fstream>
#include <iterator>
#include <iostream>
#include <list>
#include <string>


class Metadata {
public:
private:
	MetadataFileType type;

	// Hack, but allows ChunkIterator to have a
	//   MetadataIterator*, breaking the circular dependancy
	MetadataIterator* bRef = NULL;
	MetadataIterator* rRef = NULL;

	friend MetadataFactory;
	friend MetadataIterator;

protected:
	std::list< MetadataTag > tags;

	Metadata() = default;

	        void create(const std::string&);
	        void create(std::istream& file) { read(file); };
	virtual void read(std::istream&) = 0;

public:
	Metadata(const Metadata&);
	Metadata(Metadata&&);

	virtual ~Metadata();

	Metadata& operator=(Metadata&& rhs);

	virtual void              write(const std::string&) = 0;
	        bool              remove(unsigned int);

	        size_t            size()   const;
	        bool              empty()  const;

	        MetadataIterator  begin()  noexcept { return   MetadataIterator(this, false); };
	        MetadataIterator  end()    noexcept { return ++MetadataIterator(this, true);  };
	        MetadataIterator  rbegin() noexcept { return   MetadataIterator(this, true);  };
	        MetadataIterator  rend()   noexcept { return --MetadataIterator(this, false); };

	        MetadataIterator* beginReference();
	        MetadataIterator* rbeginReference();

// protected:
// int loopChunks< T >(std::istream&, ...);
#include <metadata.tcc>
};


#define METADATA_CONSTRUCTORS(_SUBTYPE_)             using Metadata::Metadata; \
                                            private: _SUBTYPE_() = default; \
                                                     friend MetadataFactory; \
                                            public:  _SUBTYPE_(const std::string& path) { create(path); } \
                                                     _SUBTYPE_(std::istream& file)      { create(file); }


#endif
