#include <chunk.hxx>

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <ios>
#include <locale>
#include <sstream>


std::map< std::string, std::pair< std::string, Chunk::Type > > Chunk::typeMap = {
	// TODO: Try to reduce amount of Type::HEX entries via proper formatting
	{ "IHDR", { "Header",             Type::HEADER } },
	{ "PLTE", { "Palette",            Type::HEX    } },
	{ "IDAT", { "Image",              Type::COUNT  } },
	{ "IEND", { "End of file",        Type::HIDE   } },

	{ "tRNS", { "Transparency color", Type::COLOR  } },

	{ "cHRM", { "Chromaticity",       Type::HEX    } },
	{ "gAMA", { "Gamma",              Type::HEX    } },
	{ "iCCP", { "Color profile",      Type::NONE   } },
	{ "sBIT", { "Significant bits in color data", Type::HEX } },
	{ "sRGB", { "RGB color space",    Type::HEX    } },

	{ "tEXt", { "Text",               Type::TEXT   } },
	{ "zTXt", { "Compressed text",    Type::CTEXT  } },
	{ "iTXt", { "Unicode text",       Type::ITEXT  } },

	{ "bKGD", { "Background color",   Type::COLOR  } },
	{ "hIST", { "Histogram",          Type::HEX    } },
	{ "pHYs", { "Pixel dimensions",   Type::HEX    } },
	{ "sPLT", { "Suggested palette",  Type::HEX    } },

	{ "tIME", { "Last modified",      Type::TIME   } },
};


std::string sanitize(std::string str) {
	size_t i;
	while ((i = str.find("\r\n")) != std::string::npos) {
		str.replace(i, 1, "<br>");
	}
	while ((i = str.find('\n')) != std::string::npos) {
		str.replace(i, 1, "<br>");
	}

	return str;
};


Chunk::Chunk(const Chunk& c) {
	length = c.length;
	typeCode = c.typeCode;
	std::memcpy(crc, c.crc, 4);

	raw = new char[length];
	std::memcpy(raw, c.raw, length);
}

Chunk::Chunk(Chunk&& c) {
	length = c.length;
	typeCode = c.typeCode;
	raw = c.raw;
	std::memcpy(crc, c.crc, 4);

	c.raw = NULL;
}

Chunk::Chunk(std::istream& file) {
	char bytes[4];

	file.clear();

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


Chunk::~Chunk() {
	if (raw != NULL) {
		delete[] raw;
	}
}


std::string Chunk::hexString(unsigned int* print, unsigned int* hex) const {
	std::stringstream ss;

	ss << std::hex;

	ss << "<code>";
	for (unsigned int i = 0; i < length; ++i) {
		if (i > 0) {
			ss << " ";
		}

		ss << std::setw(2) << std::setfill('0');
		ss << ((unsigned int)raw[i] & 0xFF);

		if ((print != NULL) || (hex != NULL)) {
			if (std::isprint(raw[i])) {
				++print;
			} else {
				++hex;
			}
		}
	}
	ss << "</code>";

	return ss.str();
}
std::string Chunk::data() const {
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

std::string Chunk::name() const {
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


void Chunk::write(std::ostream& out) const {
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

bool Chunk::required() const {
	return isupper(typeCode[0], std::locale("C"));
}
