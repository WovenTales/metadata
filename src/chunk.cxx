#include <chunk.hxx>

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <ios>
#include <locale>
#include <sstream>


std::string Chunk::sanitize(std::string str) {
	size_t i;
	while ((i = str.find("\r\n")) != std::string::npos) {
		str.replace(i, 1, "<br>");
	}
	while ((i = str.find('\n')) != std::string::npos) {
		str.replace(i, 1, "<br>");
	}

	return str;
};


Chunk::Chunk(const Chunk& c) : typeMap(c.typeMap) {
	length = c.length;
	typeCode = c.typeCode;
	std::memcpy(crc, c.crc, 4);

	raw = new char[length];
	std::memcpy(raw, c.raw, length);
}

Chunk::Chunk(Chunk&& c) : typeMap(c.typeMap) {
	length = c.length;
	typeCode = c.typeCode;
	raw = c.raw;
	std::memcpy(crc, c.crc, 4);

	c.raw = NULL;
}

Chunk::Chunk(std::istream& file, const std::map< std::string, std::pair< std::string, Type > >& tm) : typeMap(tm) {
	file.clear();
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
