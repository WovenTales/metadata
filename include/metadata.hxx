#ifndef METADATA_H
#define METADATA_H


class Chunk;
#include <chunktype.hxx>
class MetadataFactory;
#include <metadatafiletype.hxx>
#include <metadataiterator.hxx>
#include <metadatatag.hxx>

#include <fstream>
#include <iterator>
#include <list>
#include <string>


#define METADATA_CONSTRUCTORS(_SUBTYPE_)               using Metadata::Metadata; \
                                            private:   _SUBTYPE_() = default; \
                                                       friend MetadataFactory; \
                                            protected: _SUBTYPE_(const std::string& path) : Metadata(path) { create(); }


class Metadata {
public:
private:
	std::ifstream file;
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
	Metadata(const std::string&);

	        void create();
	virtual void read(std::ifstream&) = 0;

public:
	Metadata(const Metadata&);
	Metadata(Metadata&&);

	virtual ~Metadata();

	Metadata& operator=(Metadata&& rhs);

	virtual void              write(const std::string&) = 0;
	        bool              remove(unsigned int);

	        size_t            size()                          noexcept;
	        bool              empty()                         noexcept;

	        MetadataIterator  begin()                         noexcept { return   MetadataIterator(this, false); };
	        MetadataIterator  end()                           noexcept { return ++MetadataIterator(this, true);  };
	        MetadataIterator  rbegin()                        noexcept { return   MetadataIterator(this, true);  };
	        MetadataIterator  rend()                          noexcept { return --MetadataIterator(this, false); };

	        MetadataIterator* beginReference();
	        MetadataIterator* rbeginReference();
};


#endif
