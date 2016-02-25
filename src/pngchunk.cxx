#include <pngchunk.hxx>

#include <algorithm>
#include <locale>
#include <map>
#include <string>
#include <sstream>
#include <utility>


std::map< std::string, std::pair< std::string, Chunk::Type > > PNGTypeMap = {
	// TODO: Implement Chunk::Type::CUSTOM entries
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



PNGChunk::PNGChunk(const PNGChunk& c) : Chunk(c) {
	std::copy(c.crc, (c.crc + 4), crc);
}

PNGChunk::PNGChunk(PNGChunk&& c) : Chunk(c) {
	std::move(c.crc, (c.crc + 4), crc);
}

PNGChunk::PNGChunk(std::istream& file) : Chunk(file, PNGTypeMap) {
	char bytes[4];

	// Get data length
	file.read(bytes, 4);
	if (file.fail()) {
		throw 'L';
	}
	// PNG is strictly big-endian
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

	// Swallow CRC (don't need to validate)
	file.read(crc, 4);
	if (file.fail()) {
		throw 'C';
	}
}


std::string PNGChunk::printableTypeCode() const {
	return typeCode;
}


std::string PNGChunk::defaultChunkName(const std::string& typeCode) const {
	return (std::string(isupper(typeCode[1], std::locale("C")) ? "Unrecognized" : "Private-use")
			+ " chunk <" + typeCode + ">");
}


std::string PNGChunk::data(Chunk::Type type) const {
	size_t n;
	std::string str;

	switch (type) {
		case Type::CUSTOM:
			return "TODO<br><br>" + hexString();
		case Type::TEXT:
			str = std::string(raw, length);
			n = str.find('\0');

			if (n == std::string::npos) {
				// Not proper Text tag, but included to handle malformed case
				return str;
			} else {
				return str.substr(n + 1);
			}
		default:
			return Chunk::data(type);
	}
}

std::string PNGChunk::name(Chunk::Type type, const std::string& title) const {
	size_t n;
	std::string str;

	switch (type) {
		case Type::TEXT:
			str = std::string(raw, length);
			n = str.find('\0');

			if (n == std::string::npos) {
				// Not proper Text tag, but included to handle malformed case
				return title;
			} else {
				return (title + " <" + str.substr(0, n) + ">");
			}
		default:
			return Chunk::name(type, title);
	}
}


bool PNGChunk::required() const {
	return isupper(typeCode[0], std::locale("C"));
}


void PNGChunk::write(std::ostream& out) const {
	// Automatically truncates to single byte
	// Copy byte-by-byte to ensure proper order
	out.put(length >> (8 * 3));
	out.put(length >> (8 * 2));
	out.put(length >> 8);
	out.put(length);

	out.write(typeCode.c_str(), 4);
	out.write(raw, length);
	out.write(crc, 4);
}
