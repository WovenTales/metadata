#include <pngchunk.hxx>

#include <algorithm>
#include <locale>
#include <map>
#include <string>
#include <sstream>
#include <utility>


std::map< std::string, std::pair< std::string, ChunkType > > PNGTypeMap = {
	// TODO Implement ChunkType::CUSTOM entries
	{ "IHDR", { "Header",                         ChunkType::CUSTOM } },
	{ "PLTE", { "Palette",                        ChunkType::CUSTOM } },
	{ "IDAT", { "Image",                          ChunkType::COUNT  } },
	{ "IEND", { "End of file",                    ChunkType::NONE   } },

	{ "tRNS", { "Transparency color",             ChunkType::COLOR  } },

	{ "cHRM", { "Chromaticity",                   ChunkType::CUSTOM } },
	{ "gAMA", { "Gamma",                          ChunkType::CUSTOM } },
	{ "iCCP", { "Color profile",                  ChunkType::NONE   } },
	{ "sBIT", { "Significant bits in color data", ChunkType::DIGIT  } },
	{ "sRGB", { "RGB color space",                ChunkType::CUSTOM } },

	{ "tEXt", { "Text",                           ChunkType::TEXT   } },
	{ "zTXt", { "Compressed text",                ChunkType::CUSTOM } },
	{ "iTXt", { "Unicode text",                   ChunkType::CUSTOM } },

	{ "bKGD", { "Background color",               ChunkType::COLOR  } },
	{ "hIST", { "Histogram",                      ChunkType::CUSTOM } },
	{ "pHYs", { "Pixel dimensions",               ChunkType::CUSTOM } },
	{ "sPLT", { "Suggested palette",              ChunkType::CUSTOM } },

	{ "tIME", { "Last modified",                  ChunkType::CUSTOM } },
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
		throw 'H';
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


std::string PNGChunk::defaultChunkName() const {
	return (std::string(std::isupper(typeCode[1], std::locale("C")) ? "Unrecognized" : "Private-use")
			+ " chunk &lt;" + typeCode + "&lt;");
}


std::string PNGChunk::data(ChunkType type) const {
	size_t n;
	std::string str;

	switch (type) {
		case ChunkType::CUSTOM:
			return "TODO<br><br>" + hexString();
		case ChunkType::TEXT:
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

std::string PNGChunk::name(ChunkType type, const std::string& title) const {
	size_t n;
	std::string str;

	switch (type) {
		case ChunkType::TEXT:
			str = std::string(raw, length);
			n = str.find('\0');

			if (n == std::string::npos) {
				// Not proper Text tag, but included to handle malformed case
				return title;
			} else {
				return (title + " &lt;" + str.substr(0, n) + "&gt;");
			}
		default:
			return Chunk::name(type, title);
	}
}


bool PNGChunk::required() const {
	return std::isupper(typeCode[0], std::locale("C"));
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
