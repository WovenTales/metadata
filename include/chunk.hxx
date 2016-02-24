#ifndef CHUNK_H
#define CHUNK_H


#include <cstddef>

#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <utility>


#define CHUNK_CONSTRUCTORS(_SUBTYPE_)    using Chunk::Chunk;


struct Chunk {
public:
	enum struct Type {
		OTHER,   // autodetect text, otherwise hex
		NONE,    // do not display contents
		HIDE,    // do not display contents or name
		CUSTOM,  // specific to format of subclass

		HEX,     // raw hex value

		DIGIT,   // numerical value (big endian, unsigned)
		TEXT,    // text string

		COLOR,   // color swatch
		COUNT,   // count and display occurrences of tag
		TIME,    // timestamp
	};

private:
	const std::map< std::string, std::pair< std::string, Type > >& typeMap;

	static std::string sanitize(std::string);

protected:
	      unsigned int   length   = 0;
	      std::string    typeCode;
	      char*          raw      = NULL;

	Chunk(std::istream&, const std::map< std::string, std::pair< std::string, Type > >&);

	       std::string hexString(unsigned int* = NULL, unsigned int* = NULL, unsigned int = 0) const;

	virtual std::string data(Type)                           const;
	virtual std::string name(Type, const std::string&)       const;

	virtual std::string printableTypeCode()                  const = 0;
	virtual std::string defaultChunkName(const std::string&) const = 0;

public:
	Chunk(const Chunk&);
	Chunk(Chunk&&);

	virtual ~Chunk();

	        std::string data()               const;
	        std::string name()               const;
	        Type        type()               const;
	virtual bool        required()           const = 0;

	virtual void        write(std::ostream&) const = 0;
};


#endif
