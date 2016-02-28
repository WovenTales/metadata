#include <exififdchunk.hxx>

#include <exifchunk.hxx>

#include <cstring>

#include <algorithm>
#include <iomanip>
#include <locale>
#include <map>
#include <string>
#include <sstream>
#include <utility>


// This entire thing is essentially a TIFF file
// Basic Exif spec implemented, but not all original TIFF tags
//     Once image format split off (no QPixmap support by default),
//       continue working from page 33 of the latter spec.

//        typeCode                name         type
std::map< std::string, std::pair< std::string, ChunkType > > ExifIFDTypeMap = {
	// TODO For next three, follow offset to print values
	{ "\x87\x69", { "EXIF-specific IFD",              ChunkType::NONE   } },
	{ "\x88\x25", { "GPS IFD",                        ChunkType::NONE   } },
	{ "\xA0\x05", { "Interoperability IFD",           ChunkType::NONE   } },

	{ "\x01\x00", { "Image width (pixels)",           ChunkType::DIGIT  } },
	{ "\x01\x01", { "Image width (pixels)",           ChunkType::DIGIT  } },
	{ "\x01\x02", { "Bits per sample",                ChunkType::CUSTOM } },
	{ "\x01\x03", { "Compression scheme",             ChunkType::CUSTOM } },
	{ "\x01\x06", { "Pixel composition",              ChunkType::CUSTOM } },
	{ "\x01\x08", { "Halftone cell width",            ChunkType::DIGIT  } },
	{ "\x01\x09", { "Halftone cell height",           ChunkType::DIGIT  } },
	{ "\x01\x0A", { "Bit fill order",                 ChunkType::CUSTOM } },
	{ "\x01\x12", { "Image orientation",              ChunkType::CUSTOM } },
	{ "\x01\x15", { "Samples per pixel",              ChunkType::DIGIT  } },
	{ "\x01\x1A", { "Pixel resolution (width)",       ChunkType::DIGIT  } },
	{ "\x01\x1B", { "Pixel resolution (height)",      ChunkType::DIGIT  } },
	{ "\x01\x1C", { "Unit to measure resolution",     ChunkType::CUSTOM } },
	{ "\x01\x52", { "Meaning of extra pixel samples", ChunkType::CUSTOM } },
	{ "\x02\x12", { "Y:C subsampling ratio",          ChunkType::CUSTOM } },
	{ "\x02\x13", { "Y and C positioning",            ChunkType::CUSTOM } },

	{ "\x01\x11", { "Image data location(s)",         ChunkType::CUSTOM } },
	{ "\x01\x16", { "Rows per strip",                 ChunkType::DIGIT  } },
	{ "\x01\x17", { "Bytes per strip (compressed)",   ChunkType::CUSTOM } },
	{ "\x02\x01", { "Start of image",                 ChunkType::CUSTOM } },
	{ "\x02\x02", { "Size of image",                  ChunkType::DIGIT  } },

	{ "\x01\x2D", { "Transfer function",              ChunkType::HEX    } },
	{ "\x01\x3E", { "White point",                    ChunkType::CUSTOM } },
	{ "\x01\x3F", { "Primary chromaticities",         ChunkType::CUSTOM } },
	{ "\x01\x40", { "Color palette",                  ChunkType::HEX    } },
	{ "\x02\x11", { "Color space transform",          ChunkType::CUSTOM } },
	{ "\x02\x14", { "Black and white reference",      ChunkType::CUSTOM } },

	{ "\x01\x0F", { "Equipment make",                 ChunkType::TEXT   } },
	{ "\x01\x10", { "Equipment model",                ChunkType::TEXT   } },
	{ "\x01\x31", { "Software used",                  ChunkType::TEXT   } },
	{ "\x01\x32", { "Last file change",               ChunkType::TEXT   } },  // "YYYY:MM:DD HH:MM:SS"
	{ "\x01\x3B", { "Artist/editor",                  ChunkType::TEXT   } },
	{ "\x82\x98", { "Copyright",                      ChunkType::TEXT   } },
};


ExifIFDChunk::ExifIFDChunk(std::istream& file, bool b) : Chunk(file, ExifChunk::convertMap(ExifIFDTypeMap, b)) {
	char bytes[12];

	bigEndian = b;

	// Get chunk type
	file.read(bytes, 12);
	if (file.fail()) {
		throw 'H';
	}
	typeCode  = ExifChunk::toBigEndian(std::string({ bytes[0], bytes[1] }), bigEndian);

	exifType  = readBytes((bytes + 2), 2, bigEndian);
	exifCount = readBytes((bytes + 4), 4, bigEndian);

	length = (exifCount * dataTypeLength());
	raw = new char[length];

	if (length > 4) {
		exifOffset = readBytes((bytes + 8), 4, bigEndian);

		unsigned int pos = file.tellg();
		file.seekg(exifOffset);
		file.read(raw, length);
		if (file.fail()) {
			throw 'D';
		}
		file.seekg(pos);
	} else {
		std::copy((bytes + 8), (bytes + length), raw);

		if (bigEndian == false) {
			std::reverse(raw, (raw + length));
		}
	}
}


std::string ExifIFDChunk::printableTypeCode() const {
	std::ostringstream ss;

	ss << std::hex;
	if (bigEndian) {
		ss << std::setw(2) << std::setfill('0') << ((unsigned int)typeCode[0] & 0xFF);
		ss << std::setw(2) << std::setfill('0') << ((unsigned int)typeCode[1] & 0xFF);
	} else {
		ss << std::setw(2) << std::setfill('0') << ((unsigned int)typeCode[1] & 0xFF);
		ss << std::setw(2) << std::setfill('0') << ((unsigned int)typeCode[0] & 0xFF);
	}

	return ss.str();
}

unsigned int ExifIFDChunk::intTypeCode() const {
	if (bigEndian) {
		return ((((unsigned int)typeCode[0] & 0xFF) << 8) + ((unsigned int)typeCode[1] & 0xFF));
	} else {
		return (((unsigned int)typeCode[0] & 0xFF) + (((unsigned int)typeCode[1] & 0xFF) << 8));
	}
}

unsigned int ExifIFDChunk::dataTypeLength() const {
	switch (exifType) {
		case EXIF_BYTE:
		case EXIF_ASCII:
		case EXIF_UNDEFINED:
			return 1;
		case EXIF_SHORT:
			return 2;
		case EXIF_LONG:
		case EXIF_SLONG:
			return 4;
		case EXIF_RATIONAL:
		case EXIF_SRATIONAL:
			return 8;
	}

	return 0;
}


std::string ExifIFDChunk::data(ChunkType type) const {
	std::ostringstream ss;
	// TODO Implement ChunkType::CUSTOM entries

	switch (type) {
		case ChunkType::DIGIT:
			if (exifType == EXIF_RATIONAL) {
				return std::to_string(ExifChunk::exifRational((raw + exifOffset), bigEndian));
			} else if (exifType == EXIF_SRATIONAL) {
				return std::to_string(ExifChunk::exifSRational((raw + exifOffset), bigEndian));
			} else {
				return std::to_string(simpleValue());
			}
		case ChunkType::TEXT:
			// NULL-termination included in exifCount and therefore in length
			{
				char* str = raw;
				unsigned int sum = 0, len;
				while (sum < length) {
					if (sum != 0) {
						ss << "<br>";
					}
					ss << "'" << str << "'";

					len  = (std::strlen(str) + 1);
					sum += len;
					str += len;
				}

				return ss.str();
			}
		case ChunkType::CUSTOM:
			switch (intTypeCode()) {
				case 258:  // Bits per sample
					for (unsigned int i = 0; i < exifCount; ++i) {
						if (i > 0) {
							ss << ", ";
						}
						ss << simpleValue(raw + (2 * i));
					}
					break;
				case 259:  // Compression scheme
					switch (simpleValue()) {
						case 1:
							return "Uncompressed";
						case 2:
							return "1D modified Huffman";
						case 6:
							return "EXIF";
						case 32773:
							return "PackBits";
						default:
							return "Unknown";
					}
				case 262:  // Pixel composition
					switch (simpleValue()) {
						case 2:
							return "RGB";
						case 6:
							return "YCbCr";
						default:
							return "Unknown";
					}
				case 266:  // Bit fill order
					switch (simpleValue()) {
						case 1:
							return "Left-to-right";
						case 2:
							return "Low-to-high";
						default:
							return "Unknown";
					}
				// TODO See about displaying strips, if possible
				case 273:  // Image data location(s)
					{
						char* offset = raw;
						unsigned int step = dataTypeLength();

						ss << std::hex;
						for (unsigned int i = 0; i < exifCount; ++i) {
							if (i > 0) {
								ss << ", ";
							}
							ss << "0x" << std::setw(8) << std::setfill('0') << simpleValue(offset);
							offset += step;
						}
					}
					ss << std::dec;
					break;
				case 274:  // Image orientation
					switch (simpleValue()) {
						case 1:
							return "Left to right, top to bottom";
						case 2:
							return "Right to left, top to bottom";
						case 3:
							return "Right to left, bottom to top";
						case 4:
							return "Left to right, bottom to top";
						case 5:
							return "Top to bottom, left to right";
						case 6:
							return "Top to bottom, right to left";
						case 7:
							return "Bottom to top, right to left";
						case 8:
							return "Bottom to top, left to right";
						default:
							return "Unknown";
					}
				case 279:  // Bytes per strip
					{
						char* offset = raw;
						unsigned int step = dataTypeLength();

						for (unsigned int i = 0; i < exifCount; ++i) {
							if (i > 0) {
								ss << ", ";
							}
							ss << simpleValue(offset);
							offset += step;
						}
					}
					break;
				case 284:  // Image data arrangement
					switch (simpleValue()) {
						case 1:
							return "Chunky format";
						case 2:
							return "Planar format";
						default:
							return "Unknown";
					}
				case 296:  // Unit to determine resolution
					switch (simpleValue()) {
						case 1:
							return "Inches";
						case 2:
							return "Centimeters";
						default:
							return "Unknown";
					}
				case 318:  // White point:
					ss << ExifChunk::exifRational(raw,        bigEndian) << ",&nbsp;";
					ss << ExifChunk::exifRational((raw + 8),  bigEndian) << ")";
					break;
				case 319:  // Primary chromaticities:
					ss << ExifChunk::exifRational(raw,        bigEndian) << ",&nbsp;";
					ss << ExifChunk::exifRational((raw + 8),  bigEndian) << "], [";
					ss << ExifChunk::exifRational((raw + 16), bigEndian) << ",&nbsp;";
					ss << ExifChunk::exifRational((raw + 24), bigEndian) << "], [";
					ss << ExifChunk::exifRational((raw + 32), bigEndian) << ",&nbsp;";
					ss << ExifChunk::exifRational((raw + 40), bigEndian) << "]";
					break;
				case 338:  // Meaning of extra pixel samples
					switch (simpleValue()) {
						case 0:
							return "Unspecified data";
						case 1:
							return "Associated alpha";
						case 2:
							return "Unassociated alpha";
						default:
							return "Unknown";
					}
				case 513:  // Start of EXIF image
					ss << "0x" << std::setw(8) << std::setfill('0');
					ss << std::hex << simpleValue() << std::dec;
					break;
				case 529:  // Color space transform
					ss << ExifChunk::exifRational(raw,        bigEndian) << ", ";
					ss << ExifChunk::exifRational((raw + 8),  bigEndian) << ", ";
					ss << ExifChunk::exifRational((raw + 16), bigEndian);
					break;
				case 530:  // Y:C subsampling ratio
					// Only defined for pairs [2, 1] and [2, 2]
					if (simpleValue() == 2) {
						switch (readBytes((raw + 2), 2, bigEndian)) {
							case 1:
								return "YCbCr 4:2:2";
							case 2:
								return "YCbCr 4:2:0";
							default:
								return "Unknown";
						}
					} else {
						return "Unknown";
					}
				case 531:  // Y and C positioning
					switch (simpleValue()) {
						case 1:
							return "Centered";
						case 2:
							return "Co-sited";
						default:
							return "Unknown";
					}
				case 532:  // Black and white reference:
					ss << ExifChunk::exifRational(raw,        bigEndian) << ",&nbsp;";
					ss << ExifChunk::exifRational((raw + 8),  bigEndian) << "], [";
					ss << ExifChunk::exifRational((raw + 16), bigEndian) << ",&nbsp;";
					ss << ExifChunk::exifRational((raw + 24), bigEndian) << "], [";
					ss << ExifChunk::exifRational((raw + 32), bigEndian) << ",&nbsp;";
					ss << ExifChunk::exifRational((raw + 40), bigEndian) << "]";
					break;
			}

			if (ss.tellp() == 0) {
				return hexString(false, 0, length);
			} else {
				return ss.str();
			}
		default:
			return Chunk::data(type);
	}
}

int ExifIFDChunk::simpleValue(const char* offset) const {
	if (offset == NULL) {
		offset = raw;
	}

	switch (exifType) {
		case EXIF_BYTE:
			return ((unsigned int)offset[0] & 0xFF);
		case EXIF_SHORT:
			return readBytes(offset, 2, bigEndian);
		case EXIF_LONG:
		default:
			return readBytes(offset, 4, bigEndian);
		case EXIF_SLONG:
			return (int)readBytes(offset, 4, bigEndian);
	}
}


bool ExifIFDChunk::required() const {
	// TODO After generalizing to TIFF files, some fields will be required
	return false;
}


void ExifIFDChunk::write(std::ostream& out) const {
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
