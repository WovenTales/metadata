#include <pngchunk.hxx>

#include <map>
#include <string>
#include <sstream>
#include <utility>


std::map< std::string, std::pair< std::string, Chunk::Type > > PNGTypeMap = {
	// TODO: Try to reduce amount of Chunk::Type::HEX entries via proper formatting
	{ "IHDR", { "Header",                         Chunk::Type::HEADER } },
	{ "PLTE", { "Palette",                        Chunk::Type::HEX    } },
	{ "IDAT", { "Image",                          Chunk::Type::COUNT  } },
	{ "IEND", { "End of file",                    Chunk::Type::HIDE   } },

	{ "tRNS", { "Transparency color",             Chunk::Type::COLOR  } },

	{ "cHRM", { "Chromaticity",                   Chunk::Type::HEX    } },
	{ "gAMA", { "Gamma",                          Chunk::Type::HEX    } },
	{ "iCCP", { "Color profile",                  Chunk::Type::NONE   } },
	{ "sBIT", { "Significant bits in color data", Chunk::Type::HEX } },
	{ "sRGB", { "RGB color space",                Chunk::Type::HEX    } },

	{ "tEXt", { "Text",                           Chunk::Type::TEXT   } },
	{ "zTXt", { "Compressed text",                Chunk::Type::CTEXT  } },
	{ "iTXt", { "Unicode text",                   Chunk::Type::ITEXT  } },

	{ "bKGD", { "Background color",               Chunk::Type::COLOR  } },
	{ "hIST", { "Histogram",                      Chunk::Type::HEX    } },
	{ "pHYs", { "Pixel dimensions",               Chunk::Type::HEX    } },
	{ "sPLT", { "Suggested palette",              Chunk::Type::HEX    } },

	{ "tIME", { "Last modified",                  Chunk::Type::TIME   } },
};


PNGChunk::PNGChunk(std::istream& file) : Chunk(file, PNGTypeMap) {
	char bytes[4];

	// Get data length
	file.read(bytes, 4);
	if (!file.good()) {
		throw 'L';
	}
	// PNG is strictly big-endian
	for (int i = 0; i < 4; ++i) {
		length += ((unsigned int)bytes[i] << ((3 - i) * 8));
	}

	// Get chunk type
	file.read(bytes, 4);
	if (!file.good()) {
		throw 'T';
	}
	typeCode = std::string(bytes, 4);

	// Get chunk data
	raw = new char[length];
	file.read(raw, length);
	if (!file.good()) {
		delete[] raw;
		throw 'D';
	}

	// Swallow CRC (don't need to validate)
	file.read(crc, 4);
	if (!file.good()) {
		throw 'C';
	}
}


std::string PNGChunk::data() const {
	unsigned int print = 0, hex = 0;
	size_t n;
	std::stringstream out;
	std::string str;

	switch (type()) {
		case Type::OTHER:
			out << hexString(&print, &hex);

			// If it seems likely this is a text field, interpret it as such
			if (print > hex) {
				out.seekp(0);
				out << sanitize(std::string(raw, length)) << "<br><br>";
			}

			return out.str();
		case Type::NONE:
			return "";
		case Type::HIDE:
			return "";


		case Type::HEX:
			return hexString();


		case Type::TEXT:
			str = sanitize(std::string(raw, length));
			n = str.find('\0');

			if (n == std::string::npos) {
				return str;
			} else {
				return str.substr(n + 1);
			}
			return (str.substr(0, n) + ": " + str.substr(n + 1));
		case Type::CTEXT:
			return "TODO";
		case Type::ITEXT:
			return "TODO";


		case Type::COLOR:
			return "TODO";
		case Type::COUNT:
			return "";
		case Type::HEADER:
			return "TODO";
		case Type::TIME:
			return "TODO";
	}

	return "ERROR";
}

std::string PNGChunk::name() const {
	auto i = typeMap.find(typeCode);
	if (i != typeMap.end()) {
		if (i->second.second == Type::TEXT) {
			std::string str = sanitize(std::string(raw, length));
			size_t n = str.find('\0');

			if (n != std::string::npos) {
				return (i->second.first + " <" + str.substr(0, n) + ">");
			}
		}
		return i->second.first;
	} else {
		return (std::string(isupper(typeCode[1], std::locale("C")) ? "Unrecognized" : "Private-use")
				+ " chunk <" + typeCode + ">");
	}
}

Chunk::Type PNGChunk::type() const {
	auto i = typeMap.find(typeCode);
	if (i != typeMap.end()) {
		return i->second.second;
	} else {
		return Type::OTHER;
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
