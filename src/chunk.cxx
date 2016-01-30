#include <chunk.hxx>

#include <iomanip>
#include <ios>
#include <sstream>


Chunk::Chunk(Chunk&& c) {
	length = c.length;
	typeCode = c.typeCode;
	raw = c.raw;

	c.raw = NULL;
}

Chunk::Chunk(std::istream& file) {
	char bytes[4];
	raw = NULL;

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
	file.read(bytes, 4);
	if (!file.good()) {
		throw 'C';
	}
}


Chunk::~Chunk() {
	if (raw != NULL) {
		delete[] raw;
	}
}


std::string Chunk::type() const {
	return typeCode;
}

std::string Chunk::data() const {
	std::stringstream ss;
	ss << std::hex;

	for (unsigned int i = 0; i < length; ++i) {
		ss << std::setw(3);
		ss << raw[i];
	}

	return ss.str();
}
