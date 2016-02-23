#ifndef CHUNK_H
#define CHUNK_H


#include <cstddef>

#include <istream>
#include <map>
#include <ostream>
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
		HEADER,  // separate and label fields according to relevant spec
		TIME,    // timestamp
	};

protected:
	      unsigned int   length   = 0;
	      std::string    typeCode;
	      char*          raw      = NULL;

	const std::map< std::string, std::pair< std::string, Type > >& typeMap;

	Chunk(std::istream&, const std::map< std::string, std::pair< std::string, Type > >&);

	       std::string hexString(unsigned int* = NULL, unsigned int* = NULL) const;
	static std::string sanitize(std::string);

	virtual std::string printableTypeCode()                  const = 0;
	virtual std::string defaultChunkName(const std::string&) const = 0;

public:
	Chunk(const Chunk&);
	Chunk(Chunk&&);

	virtual ~Chunk();

	virtual std::string data()               const;
	virtual std::string name()               const;
	        Type        type()               const;
	virtual bool        required()           const = 0;

	virtual void        write(std::ostream&) const = 0;
};


#endif
