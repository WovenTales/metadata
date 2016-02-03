#ifndef CHUNK_H
#define CHUNK_H


#include <cstddef>

#include <istream>
#include <list>
#include <map>
#include <string>
#include <utility>


struct Chunk {
public:
	enum struct Type {
		OTHER,   // autodetect text, otherwise hex
		NONE,    // do not display contents
		HIDE,    // do not display contents or name

		HEX,     // raw hex value

		TEXT,    // text string
		CTEXT,   // compressed text
		ITEXT,   // unicode text (may be compressed)

		COLOR,   // color swatch
		COUNT,   // count and display occurrences of tag
		HEADER,  // separate and label fields according to spec
		TIME,    // timestamp
	};

private:
	static std::map< std::string, std::pair< std::string, Type > > typeMap;

	std::string hexString(unsigned int* = NULL, unsigned int* = NULL) const;

public:
	unsigned int   length = 0;
	std::string    typeCode;
	unsigned char* raw = NULL;

	Chunk(const Chunk&) = delete;
	Chunk(Chunk&&);
	Chunk(std::istream&);

	virtual ~Chunk();

	std::string data() const;
	std::string name() const;
	Type        type() const;
};


#endif
