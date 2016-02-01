#include <chunk.hxx>

#include <cstdint>
#include <iomanip>
#include <ios>
#include <locale>
#include <sstream>


std::map< std::string, Chunk::Type > Chunk::typeMap = {};


std::string sanitize(std::string str) {
	size_t i;
	while ((i = str.find('\0')) != std::string::npos) {
		str.replace(i, 1, "//");
	}
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

Chunk::Chunk(std::istream& file) {
	unsigned char bytes[4];

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

	// Get chunk data
	raw = new unsigned char[length];
	file.read((char*)raw, length);
	if (!file.good()) {
		delete[] raw;
		throw 'D';
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


std::string Chunk::data() const {
	if (type() == Type::TEXT) {
		return sanitize(std::string((char*)raw, length));

	} else {
		std::stringstream ss;
		unsigned int print = 0, hex = 0;

		ss << std::hex;

		ss << "<code>";
		for (unsigned int i = 0; i < length; ++i) {
			if (i > 0) {
				ss << " ";
			}

			ss << std::setw(2) << std::setfill('0');
			ss << (unsigned int)raw[i];

			if (std::isprint(raw[i])) {
				++print;
			} else {
				++hex;
			}
		}
		ss << "</code>";

		// If it seems likely this is a text field, interpret it as such
		if (print > hex) {
			return (sanitize(std::string((char*)raw, length)) + "<br><br>" + ss.str());
		} else {
			return ss.str();
		}
	}
}

std::string Chunk::name() const {
	return typeCode;
}

Chunk::Type Chunk::type() const {
	return Type::OTHER;
}
