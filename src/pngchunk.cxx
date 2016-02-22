#include <pngchunk.hxx>

#include <algorithm>
#include <locale>
#include <map>
#include <string>
#include <sstream>
#include <utility>


std::map< std::string, std::pair< std::string, Chunk::Type > > PNGTypeMap = {
	// TODO: Try to reduce amount of Chunk::Type::HEX entries via proper formatting
	{ "IHDR", { "Header",                         Chunk::Type::PNGHEADER } },
	{ "PLTE", { "Palette",                        Chunk::Type::HEX       } },
	{ "IDAT", { "Image",                          Chunk::Type::COUNT     } },
	{ "IEND", { "End of file",                    Chunk::Type::HIDE      } },

	{ "tRNS", { "Transparency color",             Chunk::Type::COLOR     } },

	{ "cHRM", { "Chromaticity",                   Chunk::Type::HEX       } },
	{ "gAMA", { "Gamma",                          Chunk::Type::HEX       } },
	{ "iCCP", { "Color profile",                  Chunk::Type::NONE      } },
	{ "sBIT", { "Significant bits in color data", Chunk::Type::HEX       } },
	{ "sRGB", { "RGB color space",                Chunk::Type::HEX       } },

	{ "tEXt", { "Text",                           Chunk::Type::TEXT      } },
	{ "zTXt", { "Compressed text",                Chunk::Type::CTEXT     } },
	{ "iTXt", { "Unicode text",                   Chunk::Type::ITEXT     } },

	{ "bKGD", { "Background color",               Chunk::Type::COLOR     } },
	{ "hIST", { "Histogram",                      Chunk::Type::HEX       } },
	{ "pHYs", { "Pixel dimensions",               Chunk::Type::HEX       } },
	{ "sPLT", { "Suggested palette",              Chunk::Type::HEX       } },

	{ "tIME", { "Last modified",                  Chunk::Type::TIME      } },
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
	for (int i = 0; i < 4; ++i) {
		length += ((unsigned int)bytes[i] << ((3 - i) * 8));
	}

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
