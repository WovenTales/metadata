#include <jpegchunk.hxx>

#include <algorithm>
#include <iomanip>
#include <locale>
#include <string>
#include <sstream>


//        typeCode                name         type
Chunk::ChunkTypeMap JPEGTypeMap = {
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
		length = (readBytes(bytes, 2) - 2);

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

	ss << "Unrecognized chunk &lt;0xFF";
	ss << std::hex << std::setw(2) << std::setfill('0') << ((unsigned int)typeCode[0] & 0xFF) << "&gt;";

	return ss.str();
}


unsigned int JPEGChunk::exifRational(const char* offset, bool bigEndian) const {
	float out = readBytes(offset, 4, bigEndian);
	out /= readBytes((offset + 4), 4, bigEndian);

	return (unsigned int)out;
}

int JPEGChunk::exifSRational(const char* offset, bool bigEndian) const {
	float out = (int)readBytes(offset, 4, bigEndian);
	out /= (int)readBytes((offset + 4), 4, bigEndian);

	return (int)out;
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

					ss << "Pixel " << ((unsigned char)raw[7] == 0 ? "aspect ratio" : "density") <<
						": " << readBytes((raw + 8), 2) << "x" << readBytes((raw + 10), 2);
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

					// Image (3-byte pixels: R0, G0, B0, R1, G1, B1, ...)
					ss << "Thumbnail: " << ((unsigned int)raw[12] & 0xFF) << "x" <<
						((unsigned int)raw[13] & 0xFF) << hexString(true, 14);


				} else if (id == "JFXX") {
					std::string str;

					switch ((unsigned char)raw[5]) {
						case 0x10:
							// Standard JPEG encoding
							ss << "JPEG thumbnail: " << hexString(true, 6);
						case 0x11:
							// Palette (3*256 bytes), then image (byte index)
							ss << "Thumbnail (1 byte per pixel): " <<
								((unsigned int)raw[6] & 0xFF) << "x" <<
								((unsigned int)raw[7] & 0xFF) << hexString(true, 8);
						case 0x13:
							// Image (R0, G0, B0, R1, G1, B1, ...)
							ss << "Thumbnail (3 bytes per pixel): " <<
								((unsigned int)raw[6] & 0xFF) << "x" <<
								((unsigned int)raw[7] & 0xFF) << hexString(true, 8);
						default:
							return ("(Unrecognized extension) " + hexString(true, 6));
					}

				// This entire thing except 'Exif\0\0' is essentially a TIFF file
				// Basic Exif spec implemented, but not all original TIFF tags
				//     Once image format split off (no QPixmap support by default),
				//       continue working from page 33 of the latter spec.
				} else if (id == "Exif") {
					// ID encoded with two-NULL termination rather than single

					// Just as easy to access as 0x49/0x4D in 6-7
					bool bigEndian = ((unsigned char)raw[9] == 0x2A);

					// Will almost always be immediately after header,
					//   but should check for support of edge cases
					unsigned int offset = (readBytes((raw + 10), 4, bigEndian) + 6);

					unsigned int tagCount;
					char* data = NULL;
					unsigned int tag, type, count;
					unsigned int value, valueOffset;
					while (offset != 0) {
						tagCount = readBytes((raw + offset), 2, bigEndian);
						offset += 2;

						for (unsigned int i = 0; i < tagCount; ++i) {
							data = (raw + offset);

							tag   = readBytes(data, 2, bigEndian);
							data += 2;
							type  = readBytes(data, 2, bigEndian);
							data += 2;
							count = readBytes(data, 4, bigEndian);
							data += 4;

							value = 0;
							valueOffset = (readBytes(data, 4, bigEndian) + 6);
							switch (type) {
								case EXIF_BYTE:
									value = ((unsigned int)data[0] & 0xFF);
									break;
								case EXIF_SHORT:
									value = readBytes(data, 2, bigEndian);
									break;
								case EXIF_LONG:
									value = (valueOffset - 6);
									break;
								case EXIF_SLONG:
									value = (int)readBytes(data, 4, bigEndian);
									break;
								case EXIF_RATIONAL:
									value = exifRational((raw + valueOffset), bigEndian);
									break;
								case EXIF_SRATIONAL:
									value = exifSRational((raw + valueOffset), bigEndian);
									break;
							}

							// TODO Perfect candidate for splitting into subtags
							switch (tag) {
								case 256:
									ss << "Image width: " << value << " pixels";
									break;
								case 257:
									ss << "Image height: " << value << " pixels";
									break;
								case 258:
									ss << "Bits per sample: ";
									for (unsigned int j = 0; j < count; ++j) {
										if (j > 0) {
											ss << ", ";
										}
										ss << readBytes((raw + valueOffset + (2 * j)), 2, bigEndian);
									}
									break;
								case 259:
									ss << "Compression scheme: ";
									switch (value) {
										case 1:
											ss << "Uncompressed";
											break;
										case 2:
											ss << "1D modified Huffman";
											break;
										case 6:
											ss << "JPEG";
											break;
										case 32773:
											ss << "PackBits";
											break;
										default:
											ss << "Unknown";
											break;
									}
									break;
								case 262:
									ss << "Pixel composition: ";
									switch (value) {
										case 2:
											ss << "RGB";
											break;
										case 6:
											ss << "YCbCr";
											break;
										default:
											ss << "Unknown";
											break;
									}
									break;
								case 264:
									ss << "Halftone cell width: " << value;
									break;
								case 265:
									ss << "Halftone cell height: " << value;
									break;
								case 266:
									ss << "Bit fill order: ";
									switch (value) {
										case 1:
											ss << "Left-to-right";
											break;
										case 2:
											ss << "Low-to-high";
											break;
										default:
											ss << "Unknown";
											break;
									}
									break;
								case 270:
									ss << "Image title: ";
									goto ascii_goto;
								case 271:
									ss << "Equipment make: ";
									goto ascii_goto;
								case 272:
									ss << "Equipment model: ";
									goto ascii_goto;
								// TODO See about displaying strips, if possible
								case 273:
									ss << "Image data location(s): ";
									ss << std::hex;
									if (count == 1) {
										ss << "0x" << std::setw(8) << std::setfill('0');
										ss << valueOffset << std::dec;
										break;
									}
									for (unsigned int j = 0; j < count; ++j) {
										if (j > 0) {
											ss << ", ";
										}
										ss << "0x" << std::setw(8) << std::setfill('0');
										ss << readBytes((raw + valueOffset + (j * 4) + 6), 4, bigEndian);
									}
									ss << std::dec;
									break;
								case 274:
									ss << "Image orientation: ";
									switch (value) {
										case 1:
											ss << "Left to right, top to bottom";
											break;
										case 2:
											ss << "Right to left, top to bottom";
											break;
										case 3:
											ss << "Right to left, bottom to top";
											break;
										case 4:
											ss << "Left to right, bottom to top";
											break;
										case 5:
											ss << "Top to bottom, left to right";
											break;
										case 6:
											ss << "Top to bottom, right to left";
											break;
										case 7:
											ss << "Bottom to top, right to left";
											break;
										case 8:
											ss << "Bottom to top, left to right";
											break;
										default:
											ss << "Unknown";
											break;
									}
									break;
								case 277:
									ss << "Samples per pixel: " << value;
									break;
								case 278:
									ss << "Rows per strip: " << value;
									break;
								case 279:
									ss << "Bytes per strip: ";
									{
										char* bytes;
										unsigned int size = (type == EXIF_SHORT ? 2 : 4);
										if ((count * size) > 4) {
											bytes = (raw + valueOffset);
										} else {
											bytes = data;
										}
										for (unsigned int j = 0; j < count; ++j) {
											if (j > 0) {
												ss << ", ";
											}
											ss << readBytes(bytes, size, bigEndian);
											bytes += size;
										}
									}
									break;
								case 282:
									ss << "Pixels per unit (width): " << value;
									break;
								case 283:
									ss << "Pixels per unit (height): " << value;
									break;
								case 284:
									ss << "Image data arrangement: ";
									switch (value) {
										case 1:
											ss << "Chunky format";
											break;
										case 2:
											ss << "Planar format";
											break;
										default:
											ss << "Unknown";
											break;
									}
									break;
								case 296:
									ss << "Unit to determine resolution: ";
									switch (value) {
										case 1:
											ss << "Inches";
											break;
										case 2:
											ss << "Centimeters";
											break;
										default:
											ss << "Unknown";
											break;
									}
									break;
								case 301:
									ss << "Transfer function: ";
									ss << hexString(true, (offset + 8), count) << std::dec;
									break;
								case 305:
									ss << "Software used: ";
									goto ascii_goto;
								case 306:
									ss << "Last file change: ";
									// "YYYY:MM:DD HH:MM:SS"
									goto ascii_goto;
								case 315:
									ss << "Artist/editor: ";
									goto ascii_goto;
								case 318:
									ss << "White point: (";
									ss << exifRational((raw + valueOffset),      bigEndian) << ",&nbsp;";
									ss << exifRational((raw + valueOffset + 8),  bigEndian) << ")";
									break;
								case 319:
									ss << "Primary chromaticities: [";
									ss << exifRational((raw + valueOffset),      bigEndian) << ",&nbsp;";
									ss << exifRational((raw + valueOffset + 8),  bigEndian) << "], [";
									ss << exifRational((raw + valueOffset + 16), bigEndian) << ",&nbsp;";
									ss << exifRational((raw + valueOffset + 24), bigEndian) << "], [";
									ss << exifRational((raw + valueOffset + 32), bigEndian) << ",&nbsp;";
									ss << exifRational((raw + valueOffset + 40), bigEndian) << "]";
									break;
								case 320:
									ss << "Color palette: ";
									ss << hexString(true, (offset + 8), count) << std::dec;
									break;
								case 338:
									ss << "Meaning of extra pixel samples: ";
									switch (value) {
										case 0:
											ss << "Unspecified data";
											break;
										case 1:
											ss << "Associated alpha";
											break;
										case 2:
											ss << "Unassociated alpha";
											break;
										default:
											ss << "Unknown";
											break;
									}
									break;
								case 513:
									ss << "Start of JPEG image: ";
									ss << "0x" << std::setw(8) << std::setfill('0');
									ss << std::hex << valueOffset << std::dec;
									break;
								case 514:
									ss << "Size of JPEG image: " << value;
									break;
								case 529:
									ss << "Color space transform: ";
									ss << exifRational((raw + valueOffset),      bigEndian) << ", ";
									ss << exifRational((raw + valueOffset + 8),  bigEndian) << ", ";
									ss << exifRational((raw + valueOffset + 16), bigEndian);
									break;
								case 530:
									ss << "Y:C subsampling ratio: ";
									if (value == 2) {
										switch (readBytes((data + 2), 2, bigEndian)) {
											case 1:
												ss << "YCbCr 4:2:2";
												break;
											case 2:
												ss << "YCbCr 4:2:0";
												break;
											default:
												ss << "Unknown";
												break;
										}
									} else {
										ss << "Unknown";
									}
									break;
								case 531:
									ss << "Y and C positioning: ";
									switch (value) {
										case 1:
											ss << "Centered";
											break;
										case 2:
											ss << "Co-sited";
											break;
										default:
											ss << "Unknown";
											break;
									}
									break;
								case 532:
									ss << "Black and white reference: [";
									ss << exifRational((raw + valueOffset),      bigEndian) << ",&nbsp;";
									ss << exifRational((raw + valueOffset + 8),  bigEndian) << "], [";
									ss << exifRational((raw + valueOffset + 16), bigEndian) << ",&nbsp;";
									ss << exifRational((raw + valueOffset + 24), bigEndian) << "], [";
									ss << exifRational((raw + valueOffset + 32), bigEndian) << ",&nbsp;";
									ss << exifRational((raw + valueOffset + 40), bigEndian) << "]";
									break;
								case 33432:
									ss << "Copyright: ";
									goto ascii_goto;
								// TODO For next three, follow offset to print values
								case 34665:
									ss << "EXIF IFD location: ";
									ss << "0x" << std::setw(8) << std::setfill('0');
									ss << std::hex << valueOffset << std::dec;
									break;
								case 34853:
									ss << "GPS IFD location: ";
									ss << "0x" << std::setw(8) << std::setfill('0');
									ss << std::hex << valueOffset << std::dec;
									break;
								case 40965:
									ss << "Interoperability IFD location: ";
									ss << "0x" << std::setw(8) << std::setfill('0');
									ss << std::hex << valueOffset << std::dec;
									break;
								default:
									ss << "Unknown tag " << tag << ": ";
									// TODO Make use of type/count to properly render
									ss << hexString(true, (offset + 8), count) << std::dec;
									break;
							ascii_goto:
									// NULL-termination included in count
									{
										char* bytes = (count > 4 ? (raw + valueOffset) : data);
										std::string str;
										unsigned int len = 0;
										while (len < count) {
											if (len != 0) {
												ss << " // ";
											}
											str = std::string(bytes);
											ss << "'" << str << "'";
											len += (str.length() + 1);
										}
									}
									break;
							}

							if ((i + 1) == tagCount) {
								break;
							}

							ss << "<br>";

							offset += 12;
						}

						// TODO With offset to next tag, can easily leave parts
						//   of file unread and containing hidden data. Print these.

						offset = (readBytes((data + 4), 4, bigEndian) + 6);

						if (offset == 6) {
							// offset is 0, plus Exif header
							break;
						} else {
							ss << "<br><br>";
						}
					}
				}

				if (ss.tellp() == 0) {
					return ("(Unrecognized application)" + hexString(true, id.length() + 1));
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
				return (std::string(title).insert(21, ("&lt;" + std::string(raw) + ", ")) + "&gt;");
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
