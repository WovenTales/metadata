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


Chunk::Chunk(Chunk&& c) {
	length = c.length;
	typeCode = c.typeCode;
	raw = c.raw;

	c.raw = NULL;
}

Chunk::Chunk(std::istream& file, std::list< Chunk >* search) {
	unsigned char bytes[4];
	unsigned int l = 0;
	Type t;

	file.clear();

	// Get data length
	file.read((char*)bytes, 4);
	if (!file.good()) {
		throw 'L';
	}
	// PNG is strictly big-endian
	for (int i = 0; i < 4; ++i) {
		length += ((unsigned int)bytes[i] << ((3 - i) * 8));
	}

	// Get chunk type
	file.read((char*)bytes, 4);
	if (!file.good()) {
		throw 'T';
	}
	typeCode = std::string((char*)bytes, 4);
	t = type();
	// Automatically collapse into any pre-existing chunks if counting
	if ((search != NULL) && (t == Type::COUNT)) {
		l = 1;

		for (auto i = search->begin(); i != search->end(); ++i) {
			if (!typeCode.compare(i->typeCode)) {
				l += i->length;
				search->erase(i);

				break;
			}
		}
	}

	// Get chunk data
	raw = new unsigned char[length];
	file.read((char*)raw, length);
	if (!file.good()) {
		delete[] raw;
		throw 'D';
	}

	// Able to use this in non-standard manner as only block expecting length
	//   after this point is hexString, which is not called on Type::COUNT
	if ((t == Type::COUNT) && (l != 0)) {
		length = l;

		delete[] raw;
		raw = new unsigned char[0];
	}

	// Swallow CRC (don't need to validate)
	file.read((char*)bytes, 4);
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
		ss << (unsigned int)raw[i];

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
	unsigned int print = 0, hex = 0, n = 0;
	std::stringstream out;
	std::string str;

	switch (type()) {
		case Type::OTHER:
			out << hexString(&print, &hex);

			// If it seems likely this is a text field, interpret it as such
			if (print > hex) {
				out.seekp(0);
				out << sanitize(std::string((char*)raw, length)) << "<br><br>";
			}

			return out.str();
		case Type::NONE:
			return "";
		case Type::HIDE:
			return "";


		case Type::HEX:
			return hexString();


		case Type::TEXT:
			str = sanitize(std::string((char*)raw, length));
			n = str.find('\0');

			return (str.substr(0, n) + ": " + str.substr(n + 1));
		case Type::CTEXT:
			str = sanitize(std::string((char*)raw, length));
			n = str.find('\0');

			return (str.substr(0, n) + ": <Compressed>");
		case Type::ITEXT:
			return "TODO";


		case Type::COLOR:
			return "TODO";
		case Type::COUNT:
			out << length << " occurence" << (length != 1 ? "s" : "");
			return out.str();
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
		return i->second.first;
	} else {
		return (typeCode + " <" + (isupper(typeCode[1], std::locale("C")) ? "Unrecognized" : "Private-use")
				+ " chunk>");
	}
}

Chunk::Type Chunk::type() const {
	auto i = typeMap.find(typeCode);
	if (i != typeMap.end()) {
		return i->second.second;
	} else {
		return Type::OTHER;
	}
}
