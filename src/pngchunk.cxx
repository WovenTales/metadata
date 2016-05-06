#include <pngchunk.hxx>

#include <algorithm>
#include <locale>
#include <string>
#include <sstream>


namespace metadata {


// Types


/*! \class PNGChunk
 *  \sa PNGTypeMap
 */


// Variables


//! Global reference for Chunk::typeMap
/*! The PNG spec very nicely defines the valid type codes:
 *    * All four characters must be A-Z or a-z
 *    * If and only if the first letter is uppercase, it's a required tag
 *    * If the second character is lowercase, it's defined by and for a given
 *      program and anything else can safely ignore it
 *    * The third character is always uppercase for potential future use
 *    * A lowercase fourth character means it doesn't special handling once we
 *      start editing chunks; if it's uppercase, it probably can't be copied if
 *      any of the critical chunks (first letter) have been changed
 *
 *  \sa PNGChunk
 */
static const Chunk::ChunkTypeMap PNGTypeMap = {
	{ "IHDR", { "Header",                         Chunk::Type::CUSTOM } },
	{ "PLTE", { "Palette",                        Chunk::Type::CUSTOM } },
	{ "IDAT", { "Image",                          Chunk::Type::COUNT  } },
	{ "IEND", { "End of file",                    Chunk::Type::NONE   } },

	{ "tRNS", { "Transparency color",             Chunk::Type::COLOR  } },

	{ "cHRM", { "Chromaticity",                   Chunk::Type::CUSTOM } },
	{ "gAMA", { "Gamma",                          Chunk::Type::CUSTOM } },
	{ "iCCP", { "Color profile",                  Chunk::Type::NONE   } },
	{ "sBIT", { "Significant bits in color data", Chunk::Type::DIGIT  } },
	{ "sRGB", { "RGB color space",                Chunk::Type::CUSTOM } },

	{ "tEXt", { "Text",                           Chunk::Type::TEXT   } },
	{ "zTXt", { "Compressed text",                Chunk::Type::CUSTOM } },
	{ "iTXt", { "Unicode text",                   Chunk::Type::CUSTOM } },

	{ "bKGD", { "Background color",               Chunk::Type::COLOR  } },
	{ "hIST", { "Histogram",                      Chunk::Type::CUSTOM } },
	{ "pHYs", { "Pixel dimensions",               Chunk::Type::CUSTOM } },
	{ "sPLT", { "Suggested palette",              Chunk::Type::CUSTOM } },

	{ "tIME", { "Last modified",                  Chunk::Type::TIME   } },
};


/*! \var PNGChunk::crc
 *
 *  \todo We don't have to worry about this while we still only remove tags,
 *        but it will have to be recalculated when we implement tag editing
 */



// Constructors, destructors, and operators


PNGChunk::PNGChunk(std::istream& file) : Chunk(file, PNGTypeMap) {
	char bytes[4];

	// Get data length
	file.read(bytes, 4);
	if (file.fail()) {
		throw 'L';
	}
	// PNG is strictly big-endian, so we can read this directly
	length = readBytes(bytes, 4);

	// Get chunk type
	file.read(bytes, 4);
	if (file.fail()) {
		throw 'T';
	}
	typeCode = std::string(bytes, 4);

	// Get chunk data
	raw = new char[length];
	file.read(raw, length);
	if (file.fail()) {
		delete[] raw;
		throw 'D';
	}

	// Swallow CRC (don't need to validate here)
	file.read(crc, 4);
	if (file.fail()) {
		throw 'C';
	}
}

PNGChunk::PNGChunk(const PNGChunk& c) : Chunk(c) {
	std::copy(c.crc, (c.crc + 4), crc);
}

PNGChunk::PNGChunk(PNGChunk&& c) : Chunk(c) {
	std::move(c.crc, (c.crc + 4), crc);
}



// Functions


/*! \todo Implement Chunk::Type::CUSTOM entries; most are tables or otherwise
 *        require some processing beyond simply reading a value
 */
std::string PNGChunk::data(Chunk::Type type) const {
	size_t n;
	std::string str;

	switch (type) {
		case Type::CUSTOM:
			return "TODO<br><br>" + hexString();
		case Type::TEXT:
			// All PNG tEXt tags have a label separated from the rest of the
			// - contents by a \0 character
			str = std::string(raw, length);
			n = str.find('\0');

			if (n == std::string::npos) {
				// Not proper tEXt tag, but included to handle malformed case
				return str;
			} else {
				return str.substr(n + 1);
			}
		default:
			return Chunk::data(type);
	}
}


std::string PNGChunk::defaultChunkName(const std::string& typeCode) const {
	// The PNG spec allows anyone to create their own tag codes, as long as
	// - the second character in it (index 1) is lowercase
	return (std::string(isupper(typeCode[1], std::locale("C")) ? "Unrecognized" : "Private-use")
			+ " chunk &lt;" + typeCode + "&gt;");
}


std::string PNGChunk::name(Chunk::Type type, const std::string& title) const {
	size_t n;
	std::string str;

	switch (type) {
		case Type::TEXT:
			// All PNG tEXt tags have a label separated from the rest of the
			// - contents by a \0 character
			str = std::string(raw, length);
			n = str.find('\0');

			if (n == std::string::npos) {
				// Not proper tEXt tag, but included to handle malformed case
				return title;
			} else {
				return (title + " &lt;" + str.substr(0, n) + "&gt;");
			}
		default:
			return Chunk::name(type, title);
	}
}


std::string PNGChunk::printableTypeCode() const {
	return typeCode;
}


bool PNGChunk::required() const {
	return isupper(typeCode[0], std::locale("C"));
}


void PNGChunk::write(std::ostream& out) const {
	// Copy byte-by-byte to ensure proper order
	// Automatically truncates to single byte as put() expects a char
	out.put(length >> (8 * 3));
	out.put(length >> (8 * 2));
	out.put(length >> 8);
	out.put(length);

	out.write(typeCode.c_str(), 4);
	out.write(raw, length);
	out.write(crc, 4);
}


}
