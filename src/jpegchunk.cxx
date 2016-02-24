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
	{ "\xD8", { "Start of file",                                         Chunk::Type::NONE   } },
	{ "\xD9", { "End of file",                                           Chunk::Type::NONE   } },
	{ "\xDA", { "Image",                                                 Chunk::Type::NONE   } },
	{ "\xFE", { "Comment",                                               Chunk::Type::TEXT   } },

	{ "\xC0", { "Start Huffman frame (baseline DCT)",                    Chunk::Type::CUSTOM } },
	{ "\xC1", { "Start Huffman frame (extended sequential DCT)",         Chunk::Type::CUSTOM } },
	{ "\xC2", { "Start Huffman frame (progressive DCT)",                 Chunk::Type::CUSTOM } },
	{ "\xC3", { "Start Huffman frame (lossless)",                        Chunk::Type::CUSTOM } },
	{ "\xC5", { "Start Huffman frame (differential sequential DCT)",     Chunk::Type::CUSTOM } },
	{ "\xC6", { "Start Huffman frame (differential progressive DCT)",    Chunk::Type::CUSTOM } },
	{ "\xC7", { "Start Huffman frame (differential lossless)",           Chunk::Type::CUSTOM } },
	{ "\xC9", { "Start arithmatic frame (extended sequential DCT)",      Chunk::Type::CUSTOM } },
	{ "\xCA", { "Start arithmatic frame (progressive DCT)",              Chunk::Type::CUSTOM } },
	{ "\xCB", { "Start arithmatic frame (lossless)",                     Chunk::Type::CUSTOM } },
	{ "\xCD", { "Start arithmatic frame (differential sequential DCT)",  Chunk::Type::CUSTOM } },
	{ "\xCE", { "Start arithmatic frame (differential progressive DCT)", Chunk::Type::CUSTOM } },
	{ "\xCF", { "Start arithmatic frame (differential lossless)",        Chunk::Type::CUSTOM } },

	{ "\xC4", { "Huffman table(s)",                                      Chunk::Type::HEX    } },
	{ "\xCC", { "Arithmatic coding conditioning(s)",                     Chunk::Type::HEX    } },
	{ "\xDB", { "Quantization table(s)",                                 Chunk::Type::HEX    } },
	{ "\xF1", { "Temporary private use",                                 Chunk::Type::HEX    } },

	{ "\xDC", { "Number of lines",                                       Chunk::Type::DIGIT  } },
	{ "\xDE", { "Hiearchical progression",                               Chunk::Type::CUSTOM } },
	{ "\xDF", { "Reference component(s)",                                Chunk::Type::HEX    } },

	{ "\xDD", { "Restart interval",                                      Chunk::Type::DIGIT  } },
	{ "\xD0", { "Restart marker",                                        Chunk::Type::NONE   } },
	{ "\xD1", { "Restart marker",                                        Chunk::Type::NONE   } },
	{ "\xD2", { "Restart marker",                                        Chunk::Type::NONE   } },
	{ "\xD3", { "Restart marker",                                        Chunk::Type::NONE   } },
	{ "\xD4", { "Restart marker",                                        Chunk::Type::NONE   } },
	{ "\xD5", { "Restart marker",                                        Chunk::Type::NONE   } },
	{ "\xD6", { "Restart marker",                                        Chunk::Type::NONE   } },
	{ "\xD7", { "Restart marker",                                        Chunk::Type::NONE   } },

	{ "\xE0", { "Application specific 0",                                Chunk::Type::CUSTOM } },
	{ "\xE1", { "Application specific 1",                                Chunk::Type::CUSTOM } },
	{ "\xE2", { "Application specific 2",                                Chunk::Type::CUSTOM } },
	{ "\xE3", { "Application specific 3",                                Chunk::Type::CUSTOM } },
	{ "\xE4", { "Application specific 4",                                Chunk::Type::CUSTOM } },
	{ "\xE5", { "Application specific 5",                                Chunk::Type::CUSTOM } },
	{ "\xE6", { "Application specific 6",                                Chunk::Type::CUSTOM } },
	{ "\xE7", { "Application specific 7",                                Chunk::Type::CUSTOM } },
	{ "\xE8", { "Application specific 8",                                Chunk::Type::CUSTOM } },
	{ "\xE9", { "Application specific 9",                                Chunk::Type::CUSTOM } },
	{ "\xEA", { "Application specific 10",                               Chunk::Type::CUSTOM } },
	{ "\xEB", { "Application specific 11",                               Chunk::Type::CUSTOM } },
	{ "\xEC", { "Application specific 12",                               Chunk::Type::CUSTOM } },
	{ "\xED", { "Application specific 13",                               Chunk::Type::CUSTOM } },
	{ "\xEE", { "Application specific 14",                               Chunk::Type::CUSTOM } },
	{ "\xEF", { "Application specific 15",                               Chunk::Type::CUSTOM } },

	{ "\xC8", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xF0", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xF1", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xF2", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xF3", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xF4", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xF5", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xF6", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xF7", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xF8", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xF9", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xFA", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xFB", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xFC", { "JPEG extension",                                        Chunk::Type::NONE   } },
	{ "\xFD", { "JPEG extension",                                        Chunk::Type::NONE   } },
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
	unsigned char t = typeCode[0];
	std::ostringstream ss;

	switch (type) {
		case Type::CUSTOM:
			if ((t >= 0xE0) && (t <= 0xEF)) {  // Application-specific
				// All APP tags should have 0x00-terminated ID at beginning
				std::string id(raw);

				// Most have specific tag number that must be used
				//   (JFIF must be in APP0) but all still have ID string
				if (id == "JFIF") {
					ss << std::dec;
					ss << "Version: " << ((unsigned int)raw[5] & 0xFF) << "." <<
						((unsigned int)raw[6] & 0xFF) << "<br>";

					ss << "Pixel " << ((unsigned char)raw[7] == 0 ?
							"aspect ratio" : "density") << ": " <<
						((((unsigned int)raw[8] & 0xFF) << 8) + ((unsigned int)raw[9] & 0xFF)) <<
						"x" <<
						((((unsigned int)raw[10] & 0xFF) << 8) + ((unsigned int)raw[11] & 0xFF));
					if ((unsigned char)raw[7] != 0) {
						ss << " per ";
						switch ((unsigned char)raw[7]) {
							case 1:
								ss << "inch";
								break;
							case 2:
								ss << "cm";
								break;
							default:
								ss << "unknown unit";
								break;
						}
					}
					ss << "<br>";

					ss << "Thumbnail: " << ((unsigned int)raw[12] & 0xFF) << "x" <<
						((unsigned int)raw[13] & 0xFF) << "<br>" << hexString(NULL, NULL, 14);
				}

				if (ss.tellp() == 0) {
					return ("(Unrecognized application)<br><br>" +
							hexString(NULL, NULL, (id.length() + 1)));
				} else {
					return ss.str();
				}
			} else {
				return ("TODO<br><br>" + hexString());
			}
		default:
			return Chunk::data(type);
	}
}

std::string JPEGChunk::name(Chunk::Type type, const std::string& title) const {
	unsigned char t = typeCode[0];

	switch (type) {
		case Type::CUSTOM:
			if ((t >= 0xE0) && (t <= 0xEF)) {  // Application-specific
				// Display tag ID in title
				return (std::string(title).insert(21, ("<" + std::string(raw) + ", ")) + ">");
			}
		default:
			return Chunk::name(type, title);
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
		// 0xDE         Hierarchical progression
		// 0xDF         Reference components
		return true;
	}

	// Application-specific tags do not seem necessary through experimentation

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
