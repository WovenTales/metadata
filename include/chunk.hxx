#ifndef CHUNK_H
#define CHUNK_H


#include <cstddef>

#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <utility>


//! Unify subclass constructors and ensure that everything necessary is included
#define CHUNK_CONSTRUCTORS(_SUBTYPE_)    using Chunk::Chunk;


//! Manage and wrap the raw data comprising a particular metadata tag.
class Chunk {
public:
	//! Format of the prettified data.
	enum struct Type {
		OTHER,   //!< autodetect text, otherwise hex
		NONE,    //!< do not display contents
		HIDE,    //!< do not display contents or name
		CUSTOM,  //!< specific to format of subclass

		HEX,     //!< pure hex data

		DIGIT,   //!< numerical value (big endian, unsigned)
		TEXT,    //!< text string

		COLOR,   //!< color swatch
		COUNT,   //!< count and display occurrences of tag
		TIME,    //!< timestamp
	};

	//! Shorten the (typeCode -> title/format) map used by \ref typeMap
	typedef std::map< std::string, std::pair< std::string, Type > > ChunkTypeMap;

private:
	//! Internal lookup table to allow simplifying subtyping through automation
	const ChunkTypeMap& typeMap;

	//! Render the passed string to HTML-safe encoding
	static std::string sanitize(std::string);

protected:
	//! A code or string uniquely identifying the type of the tag
	std::string    typeCode;
	//! As \ref raw is not null-terminated, we need to save it's size
	unsigned int   length   = 0;
	//! The pure byte array of the tag data
	char*          raw      = NULL;

	//! Construct a Chunk from the tag starting at the current location in the stream
	Chunk(std::istream&, const ChunkTypeMap&);

	//! Format the raw data into a human-readable hex-and-ASCII block
	        std::string hexString(bool = false, unsigned int = 0, unsigned int = 0,
				unsigned int* = NULL, unsigned int* = NULL) const;

	//! Parse the raw data according to the specified \ref Type
	virtual std::string data(Type)                           const;
	//! Print the tag title according to the specified \ref Type
	virtual std::string name(Type, const std::string&)       const;

	//! Generate a printable (probably ASCII) representation of \ref typeCode
	virtual std::string printableTypeCode()                  const = 0;
	//! Generate a title to display if the \ref typeCode was not found in \ref typeMap
	virtual std::string defaultChunkName(const std::string&) const = 0;

public:
	//! Standard copy constructor
	Chunk(const Chunk&);
	//! Standard move constructor
	Chunk(Chunk&&);

	//! Standard destructor
	virtual ~Chunk();

	//! Get the data formatted according to the tag type
	        std::string data()               const;
	//! Get the tag title according to the proper type
	        std::string name()               const;
	//! Get the Type to use to render the tag
	        Type        type()               const;
	//! Get whether this tag is required to correctly display an image
	virtual bool        required()           const = 0;

	//! Write the image data into the specified stream
	virtual void        write(std::ostream&) const = 0;

	//! Read a block of bytes of a given length, in the proper endianness
	static unsigned int readBytes(const char*, unsigned char, bool = true);
};


#endif
