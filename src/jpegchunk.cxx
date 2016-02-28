#include <jpegchunk.hxx>

#include <exifmetadata.hxx>

#include <algorithm>
#include <iomanip>
#include <locale>
#include <map>
#include <string>
#include <sstream>
#include <utility>


//        typeCode                name         type
std::map< std::string, std::pair< std::string, ChunkType > > JPEGTypeMap = {
	// TODO: Implement ChunkType::CUSTOM entries
	{ "\xD8", { "Start of file",                                         ChunkType::NONE   } },
	{ "\xD9", { "End of file",                                           ChunkType::NONE   } },
	{ "\xDA", { "Image",                                                 ChunkType::NONE   } },
	{ "\xFE", { "Comment",                                               ChunkType::TEXT   } },

	{ "\xC0", { "Start Huffman frame (baseline DCT)",                    ChunkType::CUSTOM } },
	{ "\xC1", { "Start Huffman frame (extended sequential DCT)",         ChunkType::CUSTOM } },
	{ "\xC2", { "Start Huffman frame (progressive DCT)",                 ChunkType::CUSTOM } },
	{ "\xC3", { "Start Huffman frame (lossless)",                        ChunkType::CUSTOM } },
	{ "\xC5", { "Start Huffman frame (differential sequential DCT)",     ChunkType::CUSTOM } },
	{ "\xC6", { "Start Huffman frame (differential progressive DCT)",    ChunkType::CUSTOM } },
	{ "\xC7", { "Start Huffman frame (differential lossless)",           ChunkType::CUSTOM } },
	{ "\xC9", { "Start arithmatic frame (extended sequential DCT)",      ChunkType::CUSTOM } },
	{ "\xCA", { "Start arithmatic frame (progressive DCT)",              ChunkType::CUSTOM } },
	{ "\xCB", { "Start arithmatic frame (lossless)",                     ChunkType::CUSTOM } },
	{ "\xCD", { "Start arithmatic frame (differential sequential DCT)",  ChunkType::CUSTOM } },
	{ "\xCE", { "Start arithmatic frame (differential progressive DCT)", ChunkType::CUSTOM } },
	{ "\xCF", { "Start arithmatic frame (differential lossless)",        ChunkType::CUSTOM } },

	{ "\xC4", { "Huffman table(s)",                                      ChunkType::HEX    } },
	{ "\xCC", { "Arithmatic coding conditioning(s)",                     ChunkType::HEX    } },
	{ "\xDB", { "Quantization table(s)",                                 ChunkType::HEX    } },
	{ "\xF1", { "Temporary private use",                                 ChunkType::HEX    } },

	{ "\xDC", { "Number of lines",                                       ChunkType::DIGIT  } },
	{ "\xDE", { "Hiearchical progression",                               ChunkType::CUSTOM } },
	{ "\xDF", { "Reference component(s)",                                ChunkType::HEX    } },

	{ "\xDD", { "Restart interval",                                      ChunkType::DIGIT  } },
	{ "\xD0", { "Restart marker",                                        ChunkType::NONE   } },
	{ "\xD1", { "Restart marker",                                        ChunkType::NONE   } },
	{ "\xD2", { "Restart marker",                                        ChunkType::NONE   } },
	{ "\xD3", { "Restart marker",                                        ChunkType::NONE   } },
	{ "\xD4", { "Restart marker",                                        ChunkType::NONE   } },
	{ "\xD5", { "Restart marker",                                        ChunkType::NONE   } },
	{ "\xD6", { "Restart marker",                                        ChunkType::NONE   } },
	{ "\xD7", { "Restart marker",                                        ChunkType::NONE   } },

	{ "\xE0", { "Application specific 0",                                ChunkType::CUSTOM } },
	{ "\xE1", { "Application specific 1",                                ChunkType::CUSTOM } },
	{ "\xE2", { "Application specific 2",                                ChunkType::CUSTOM } },
	{ "\xE3", { "Application specific 3",                                ChunkType::CUSTOM } },
	{ "\xE4", { "Application specific 4",                                ChunkType::CUSTOM } },
	{ "\xE5", { "Application specific 5",                                ChunkType::CUSTOM } },
	{ "\xE6", { "Application specific 6",                                ChunkType::CUSTOM } },
	{ "\xE7", { "Application specific 7",                                ChunkType::CUSTOM } },
	{ "\xE8", { "Application specific 8",                                ChunkType::CUSTOM } },
	{ "\xE9", { "Application specific 9",                                ChunkType::CUSTOM } },
	{ "\xEA", { "Application specific 10",                               ChunkType::CUSTOM } },
	{ "\xEB", { "Application specific 11",                               ChunkType::CUSTOM } },
	{ "\xEC", { "Application specific 12",                               ChunkType::CUSTOM } },
	{ "\xED", { "Application specific 13",                               ChunkType::CUSTOM } },
	{ "\xEE", { "Application specific 14",                               ChunkType::CUSTOM } },
	{ "\xEF", { "Application specific 15",                               ChunkType::CUSTOM } },

	{ "\xC8", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xF0", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xF1", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xF2", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xF3", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xF4", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xF5", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xF6", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xF7", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xF8", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xF9", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xFA", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xFB", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xFC", { "JPEG extension",                                        ChunkType::NONE   } },
	{ "\xFD", { "JPEG extension",                                        ChunkType::NONE   } },
};


JPEGChunk::JPEGChunk(std::istream& file) : Chunk(file, JPEGTypeMap) {
	char bytes[2];

	// Get chunk type
	file.read(bytes, 2);
	if (file.fail() || (charValue(bytes[0]) != 0xFF)) {
		throw 'H';
	}
	// Handle marker padding (NOTE: won't be saved in new file)
	while ((charValue(bytes[1]) == 0xFF) && (file.eof() == false)) {
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
		length = (readBytes(bytes, 2) - 2);

		// Get chunk data
		raw = new char[length];
		file.read(raw, length);
		if (file.fail()) {
			delete[] raw;
			throw 'D';
		}
	}

	if ((charValue(typeCode[0]) >= 0xE0) && (charValue(typeCode[0]) <= 0xEF)) {
		// All APP tags should have 0x00-terminated ID at beginning
		std::string id(raw);

		if (id == "Exif") {
			// ID encoded with two-NULL termination rather than single

			std::istringstream tag(std::string((raw + 6), (length - 6)));
			subChunks.push_back(new ExifMetadata(tag));
		}
	}
}


std::string JPEGChunk::printableTypeCode() const {
	return ("0xFF" + hexByte(typeCode[0]));
}


std::string JPEGChunk::data(ChunkType type) const {
	unsigned char t = typeCode[0];
	std::ostringstream ss;

	switch (type) {
		case ChunkType::CUSTOM:
			if ((t >= 0xE0) && (t <= 0xEF)) {  // Application-specific
				// All APP tags should have 0x00-terminated ID at beginning
				std::string id(raw);

				// Most have specific tag number that must be used
				//   (JFIF must be in APP0) but all still have ID string

				if (id == "JFIF") {
					ss << std::dec;
					ss << "Version: " << charValue(raw[5]) << "." << charValue(raw[6]) << "<br>";

					ss << "Pixel " << (charValue(raw[7]) == 0 ? "aspect ratio" : "density") <<
						": " << readBytes((raw + 8), 2) << "x" << readBytes((raw + 10), 2);
					if (charValue(raw[7]) != 0) {
						ss << " per ";
						switch (charValue(raw[7])) {
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

					// Image (3-byte pixels: R0, G0, B0, R1, G1, B1, ...)
					ss << "Thumbnail: " << charValue(raw[12]) << "x" << charValue(raw[13]) <<
						hexString(true, 14);

				} else if (id == "JFXX") {
					switch (charValue(raw[5])) {
						case 0x10:
							// Standard JPEG encoding
							ss << "JPEG thumbnail: " << hexString(true, 6);
						case 0x11:
							// Palette (3*256 bytes), then image (byte index)
							ss << "Thumbnail (1 byte per pixel): " <<
								charValue(raw[6]) << "x" << charValue(raw[7]) <<
								hexString(true, 8);
						case 0x13:
							// Image (R0, G0, B0, R1, G1, B1, ...)
							ss << "Thumbnail (3 bytes per pixel): " <<
								charValue(raw[6]) << "x" << charValue(raw[7]) <<
								hexString(true, 8);
						default:
							return ("(Unrecognized extension) " + hexString(true, 6));
					}
				}

				if (ss.tellp() == 0) {
					return ("(Unrecognized application) " + hexString(true, id.length() + 1));
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

std::string JPEGChunk::name(ChunkType type, const std::string& title) const {
	unsigned char t = typeCode[0];

	switch (type) {
		case ChunkType::CUSTOM:
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


std::string JPEGChunk::hexByte(unsigned char c) {
	std::ostringstream ss, out;

	ss << std::hex << std::setw(2) << std::setfill('0') << charValue(c);
	for (char i : ss.str()) {
		out << std::toupper(i, std::locale("C"));
	}

	return out.str();
}

unsigned int JPEGChunk::charValue(unsigned char c) {
	return ((unsigned int)c & 0xFF);
}

ChunkType JPEGChunk::type() const {
	unsigned char t = typeCode[0];

	if ((t >= 0xE0) && (t <= 0xEF)) {  // Application-specific
		// All APP tags should have 0x00-terminated ID at beginning
		std::string id(raw);

		if (id == "Exif") {
			return ChunkType::WRAPPER;
		}
	}

	return Chunk::type();
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
