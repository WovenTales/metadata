#include <jpegchunk.hxx>

#include <algorithm>
#include <iomanip>
#include <locale>
#include <map>
#include <string>
#include <sstream>
#include <utility>


//        typeCode                name         type
std::map< std::string, std::pair< std::string, Chunk::Type > > JPEGTypeMap = {
	// TODO: Implement Chunk::Type::CUSTOM entries
	{ "\xD8", { "Start of file",                                   Chunk::Type::NONE   } },
	{ "\xD9", { "End of file",                                     Chunk::Type::NONE   } },
	{ "\xDA", { "Image",                                           Chunk::Type::NONE   } },
	{ "\xFE", { "Comment",                                         Chunk::Type::TEXT   } },

	{ "\xC0", { "Huffman frame (baseline DCT)",                    Chunk::Type::CUSTOM } },
	{ "\xC1", { "Huffman frame (extended sequential DCT)",         Chunk::Type::CUSTOM } },
	{ "\xC2", { "Huffman frame (progressive DCT)",                 Chunk::Type::CUSTOM } },
	{ "\xC3", { "Huffman frame (lossless)",                        Chunk::Type::CUSTOM } },
	{ "\xC5", { "Huffman frame (differential sequential DCT)",     Chunk::Type::CUSTOM } },
	{ "\xC6", { "Huffman frame (differential progressive DCT)",    Chunk::Type::CUSTOM } },
	{ "\xC7", { "Huffman frame (differential lossless)",           Chunk::Type::CUSTOM } },
	{ "\xC9", { "Arithmatic frame (extended sequential DCT)",      Chunk::Type::CUSTOM } },
	{ "\xCA", { "Arithmatic frame (progressive DCT)",              Chunk::Type::CUSTOM } },
	{ "\xCB", { "Arithmatic frame (lossless)",                     Chunk::Type::CUSTOM } },
	{ "\xCD", { "Arithmatic frame (differential sequential DCT)",  Chunk::Type::CUSTOM } },
	{ "\xCE", { "Arithmatic frame (differential progressive DCT)", Chunk::Type::CUSTOM } },
	{ "\xCF", { "Arithmatic frame (differential lossless)",        Chunk::Type::CUSTOM } },

	{ "\xC4", { "Huffman table(s)",                                Chunk::Type::HEX    } },
	{ "\xCC", { "Arithmatic coding conditioning(s)",               Chunk::Type::HEX    } },
	{ "\xDB", { "Quantization table(s)",                           Chunk::Type::HEX    } },
	{ "\xF1", { "Temporary private use",                           Chunk::Type::HEX    } },

	{ "\xDC", { "Number of lines",                                 Chunk::Type::DIGIT  } },
	{ "\xDE", { "Hiearchical progression",                         Chunk::Type::CUSTOM } },
	{ "\xDF", { "Reference component(s)",                          Chunk::Type::HEX    } },

	{ "\xDD", { "Restart interval",                                Chunk::Type::DIGIT  } },
	{ "\xD0", { "Restart marker",                                  Chunk::Type::NONE   } },
	{ "\xD1", { "Restart marker",                                  Chunk::Type::NONE   } },
	{ "\xD2", { "Restart marker",                                  Chunk::Type::NONE   } },
	{ "\xD3", { "Restart marker",                                  Chunk::Type::NONE   } },
	{ "\xD4", { "Restart marker",                                  Chunk::Type::NONE   } },
	{ "\xD5", { "Restart marker",                                  Chunk::Type::NONE   } },
	{ "\xD6", { "Restart marker",                                  Chunk::Type::NONE   } },
	{ "\xD7", { "Restart marker",                                  Chunk::Type::NONE   } },

	{ "\xE0", { "Application specific 0",                          Chunk::Type::HEX    } },
	{ "\xE1", { "Application specific 1",                          Chunk::Type::HEX    } },
	{ "\xE2", { "Application specific 2",                          Chunk::Type::HEX    } },
	{ "\xE3", { "Application specific 3",                          Chunk::Type::HEX    } },
	{ "\xE4", { "Application specific 4",                          Chunk::Type::HEX    } },
	{ "\xE5", { "Application specific 5",                          Chunk::Type::HEX    } },
	{ "\xE6", { "Application specific 6",                          Chunk::Type::HEX    } },
	{ "\xE7", { "Application specific 7",                          Chunk::Type::HEX    } },
	{ "\xE8", { "Application specific 8",                          Chunk::Type::HEX    } },
	{ "\xE9", { "Application specific 9",                          Chunk::Type::HEX    } },
	{ "\xEA", { "Application specific 10",                         Chunk::Type::HEX    } },
	{ "\xEB", { "Application specific 11",                         Chunk::Type::HEX    } },
	{ "\xEC", { "Application specific 12",                         Chunk::Type::HEX    } },
	{ "\xED", { "Application specific 13",                         Chunk::Type::HEX    } },
	{ "\xEE", { "Application specific 14",                         Chunk::Type::HEX    } },
	{ "\xEF", { "Application specific 15",                         Chunk::Type::HEX    } },

	{ "\xC8", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xF0", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xF1", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xF2", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xF3", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xF4", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xF5", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xF6", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xF7", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xF8", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xF9", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xFA", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xFB", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xFC", { "JPEG extension",                                  Chunk::Type::NONE   } },
	{ "\xFD", { "JPEG extension",                                  Chunk::Type::NONE   } },
};


JPEGChunk::JPEGChunk(std::istream& file) : Chunk(file, JPEGTypeMap) {
	char bytes[2];

	// Get chunk type
	file.read(bytes, 2);
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
			} else {
				data.push_back(c);
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

	ss << "Unrecognized chunk <0xFF";
	ss << std::hex << std::setw(2) << std::setfill('0') << ((unsigned int)typeCode[0] & 0xFF) << ">";

	return ss.str();
}

std::string JPEGChunk::data(Chunk::Type type) const {
	switch (type) {
		case Type::CUSTOM:
			return "TODO<br><br>" + hexString();
		default:
			return Chunk::data(type);
	}
}



bool JPEGChunk::dataFreeTag(unsigned char c) {
	if ((c == 0x01) || ((c >= 0xD0) && (c <= 0xD9))) {
	    	return true;
	} else {
		return false;
	}
}


bool JPEGChunk::required() const {
	unsigned char t = (unsigned char)typeCode[0];

	if (t == 0xC8) {  // JPEG extension
		return false;
	}

	if ((t >= 0xC0) && (t <= 0xDF)) {  // Frames and related tags
		// 0xC0 - 0xCF  Frames and related tags (except 0xC8, above)
		// 0xD0 - 0xD7  Restart markers
		// 0xD8         Start of file
		// 0xD9         End of file
		// 0xDA         Image
		// 0xDB         Quantization tables
		// 0xDC         Number of lines
		// 0xDD         Restart interval
		// 0xDE         Hiearchical progression
		// 0xDF         Reference components
		return true;
	}

	return false;
}


void JPEGChunk::write(std::ostream& out) const {
	out.put(0xFF);
	out.put(typeCode[0]);

	if (typeCode == "\xDA") {
		out.write(raw, length);
	} else if (length != 0) {
		// Automatically truncates to single byte
		// Copy byte-by-byte to ensure proper order
		out.put((length + 2) >> 8);
		out.put(length + 2);

		out.write(raw, length);
	}
}
