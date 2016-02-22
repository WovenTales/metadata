#include <jpegchunk.hxx>

#include <algorithm>
#include <iomanip>
#include <locale>
#include <map>
#include <string>
#include <sstream>
#include <utility>

//debug
#include <iostream>


//        typeCode                name         type
std::map< std::string, std::pair< std::string, Chunk::Type > > JPEGTypeMap = {
	// TODO: Try to reduce amount of Chunk::Type::HEX entries via proper formatting
	{ "\xD8", { "Start of file", Chunk::Type::NONE } },
	{ "\xD9", { "End of file", Chunk::Type::NONE } },

	{ "\xDA", { "Image", Chunk::Type::COUNT } },
	{ "\xFE", { "Comment", Chunk::Type::COUNT } },

	{ "\xC0", { "Start of frame (baseline DCT)", Chunk::Type::NONE } },
	{ "\xC2", { "Start of frame (progressive DCT)", Chunk::Type::NONE } },

	{ "\xC4", { "Define Huffman table(s)", Chunk::Type::NONE } },
	{ "\xDB", { "Define quantization table(s)", Chunk::Type::NONE } },

	{ "\xDD", { "Define restart interval", Chunk::Type::NONE } },
	{ "\xD0", { "Restart", Chunk::Type::NONE } },
	{ "\xD1", { "Restart", Chunk::Type::NONE } },
	{ "\xD2", { "Restart", Chunk::Type::NONE } },
	{ "\xD3", { "Restart", Chunk::Type::NONE } },
	{ "\xD4", { "Restart", Chunk::Type::NONE } },
	{ "\xD5", { "Restart", Chunk::Type::NONE } },
	{ "\xD6", { "Restart", Chunk::Type::NONE } },
	{ "\xD7", { "Restart", Chunk::Type::NONE } },

	{ "\xE0", { "Application specific 0", Chunk::Type::NONE } },
	{ "\xE1", { "Application specific 1", Chunk::Type::NONE } },
	{ "\xE2", { "Application specific 2", Chunk::Type::NONE } },
	{ "\xE3", { "Application specific 3", Chunk::Type::NONE } },
	{ "\xE4", { "Application specific 4", Chunk::Type::NONE } },
	{ "\xE5", { "Application specific 5", Chunk::Type::NONE } },
	{ "\xE6", { "Application specific 6", Chunk::Type::NONE } },
	{ "\xE7", { "Application specific 7", Chunk::Type::NONE } },
};


JPEGChunk::JPEGChunk(std::istream& file) : Chunk(file, JPEGTypeMap) {
	char bytes[2];

	// Get chunk type
	file.read(bytes, 2);
std::cout << "typeCode: 0x" << std::hex << std::setw(2) << std::setfill('0') << ((unsigned int)bytes[0] & 0xFF) << ((unsigned int)bytes[1] & 0xFF) << std::endl;
	if (file.fail() || ((unsigned char)bytes[0] != 0xFF)) {
		throw 'T';
	}
	// Handle marker padding (NOTE: won't be saved in new file)
	while (((unsigned char)bytes[1] == 0xFF) && (file.eof() == false)) {
		bytes[1] = file.get();
	}
	// Assumes raw code not 0xFF00, but that is invalid anyway
	typeCode = std::string({ bytes[1] });

	// Break on tags without further data
	if (dataFreeTag(typeCode[0])) {
	    	length = 0;
	    	return;
	} else if (typeCode == "\xDA") {
		// Swallow image
		std::vector< unsigned char > data;
		
		unsigned char c = file.get();
		// Read a byte at a time
		while (file.eof() == false) {
			// May be end-of-image tag or a legitimate 0xFF in the image
			if (c == 0xFF) {
				// Only peek at next char so easier to undo if it /is/ a tag
				unsigned char p = file.peek();

				// dataFreeTag() consolidates the 'Repeat' tags, but EOF needs to be managed
				if ((p == 0x00) || (dataFreeTag(p) && (p != 0xD9))) {
					data.push_back(c);
					data.push_back(file.get());
				} else {
					file.unget();
					break;
				}
			}

			c = file.get();
		}

		length = data.size();

		raw = new char[length];
		std::copy(data.data(), (data.data() + length), raw);
	} else {
		// Get data length
		file.read(bytes, 2);
		if (file.fail()) {
			throw 'L';
		}
		// JPEG is strictly big-endian, but includes length bytes in count
		length = ((256 * (unsigned char)bytes[0]) + (unsigned char)bytes[1] - 2);

		// Get chunk data
		raw = new char[length];
		file.read(raw, length);
		if (file.fail()) {
			delete[] raw;
			throw 'D';
		}
	}
}


std::string JPEGChunk::printableTypeCode() const {
	std::ostringstream ss;

	ss << std::hex << std::setw(2) << std::setfill('0');
	ss << "0xFF" << ((unsigned int)typeCode[0] & 0xFF);

	return ss.str();
}
std::string JPEGChunk::defaultChunkName(const std::string& typeCode) const {
	std::ostringstream ss;

	ss << std::hex << std::setw(2) << std::setfill('0');
	ss << "Unrecognized chunk <0xFF" << ((unsigned int)typeCode[0] & 0xFF) << ">";

	return ss.str();
}


bool JPEGChunk::dataFreeTag(unsigned char c) {
	if ((c == 0x01) || ((c >= 0xD0) && (c <= 0xD9))) {
	    	return true;
	} else {
		return false;
	}
}


bool JPEGChunk::required() const {
	return false;
}


void JPEGChunk::write(std::ostream& out) const {
	out.put(0xFF);
	out.put(typeCode[0]);

	// Automatically truncates to single byte
	// Copy byte-by-byte to ensure proper order
	out.put((length + 2) >> 8);
	out.put(length + 2);

	out.write(raw, length);
}
