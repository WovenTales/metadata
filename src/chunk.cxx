#include <chunk.hxx>

#include <metadata.hxx>

#include <algorithm>
#include <iomanip>
#include <sstream>


Chunk::Chunk(const Chunk& c) : typeMap(c.typeMap) {
	depth = c.depth;
	length = c.length;
	subChunks = c.subChunks;
	typeCode = c.typeCode;

	raw = new char[length];
	std::copy_n(c.raw, length, raw);
}

Chunk::Chunk(Chunk&& c) : typeMap(c.typeMap) {
	depth = c.depth;
	length = c.length;
	subChunks = c.subChunks;
	typeCode = c.typeCode;

	raw = c.raw;
	c.raw = NULL;
}

std::map< std::string, std::pair< std::string, ChunkType > > emptyMap = {};
Chunk::Chunk(std::istream&, unsigned int d) : typeMap(emptyMap) {
	depth = d;
}

Chunk::Chunk(std::istream&,
		const std::map< std::string, std::pair< std::string, ChunkType > >& tm,
		unsigned int d) : typeMap(tm) {
	depth = d;
}


Chunk::~Chunk() {
	if (raw != NULL) {
		delete[] raw;
	}

	while (subChunks.empty() == false) {
		delete subChunks.front();
		subChunks.pop_front();
	}
}


std::string Chunk::sanitize(std::string str) {
	size_t i;
	while ((i = str.find("\r\n")) != std::string::npos) {
		str.replace(i, 1, "<br>");
	}
	while ((i = str.find('\n')) != std::string::npos) {
		str.replace(i, 1, "<br>");
	}

	return str;
}


std::string Chunk::hexString(bool line, unsigned int offset, unsigned int span,
		unsigned int* print, unsigned int* hex) const {
	std::ostringstream ss;

	unsigned int i = offset;
	if (span == 0) {
		span = length;
	} else {
		span += offset;

		if (span > length) {
			span = length;
		}
	}

	unsigned int size = (span - offset);
	if ((raw == NULL) || (size == 0)) {
		return "";
	}

	if (line) {
	    ss << "<br>";
	}

	if (size > 256) {
		// Long QLabels cause a very noticable slowdown
		ss << size << " bytes";
	} else {
		ss << std::hex << "<code>";

		while (i < span) {
			std::ostringstream str;

			// i is incremented here
			for (unsigned int j = 0; j < 8; ++i, ++j) {
				if (i == span) {
					for (; j < 8; ++j) {
						// Should never occur on j == 0
						ss << "&nbsp;&nbsp;&nbsp;";
					}
					break;
				}

				// Adding padding at start helps set off from non-hex text
				ss << "&nbsp;";

				ss << std::setw(2) << std::setfill('0');
				ss << ((unsigned int)raw[i] & 0xFF);

				if (std::isprint(raw[i])) {
					if (print != NULL) {
						++print;
					}

					if (raw[i] == '&') {
						str << "&amp;";
					} else if (raw[i] == '<') {
						str << "&lt;";
					} else if (raw[i] == '>') {
						str << "&gt;";
					} else {
						str << raw[i];
					}
				} else {
					if (hex != NULL) {
						++hex;
					}

					str << '.';
				}
			}

			// &nbsp; before <br> is a hack to prevent the scroll bar from overlapping text
			ss << "&nbsp;&nbsp;" << str.str() << (i != span ? "&nbsp;&nbsp;&nbsp;<br>" : "");
		}

		ss << "</code>";
	}

	return ss.str();
}


std::string Chunk::data() const {
	return sanitize(data(type()));
}

std::string Chunk::data(ChunkType t) const {
	unsigned int print = 0, hex = 0;
	unsigned long sum = 0;
	std::ostringstream out;

	if (raw == NULL) {
		return "";
	}

	switch (t) {
		case ChunkType::OTHER:
			out << hexString(false, 0, 0, &print, &hex);

			// If it seems likely this is a text field, interpret it as such
			if (print > hex) {
				out.seekp(0);
				out << std::string(raw, length);
			}

			return out.str();
		case ChunkType::NONE:
			return "Should not be shown";
		case ChunkType::HIDE:
			return "Should not be shown";
		case ChunkType::WRAPPER:
			return "Should not be shown";
		case ChunkType::CUSTOM:
			return "SUBCLASS-DEFINED";


		case ChunkType::HEX:
			return hexString();


		case ChunkType::DIGIT:
			for (unsigned int i = 0; i < length; ++i) {
				sum = sum << 8;
				sum += (unsigned char)raw[i];
			}
			return std::to_string(sum);
		case ChunkType::TEXT:
			return std::string(raw, length);


		case ChunkType::COLOR:
			return "TODO";
		case ChunkType::COUNT:
			return "";
	}

	return "ERROR";
}


std::string Chunk::name() const {
	std::ostringstream ss;

	for (unsigned int i = 0; i < depth; ++i) {
		ss << "{ ";
	}

	if (typeMap.empty()) {
		ss << defaultChunkName();
	} else {
		auto m = typeMap.find(typeCode);
		if (m != typeMap.end()) {
			ss << name(m->second.second, m->second.first);
		} else {
			ss << defaultChunkName();
		}
	}

	for (unsigned int i = 0; i < depth; ++i) {
		ss << " }";
	}

	return ss.str();
}

// TODO Are both parameters necessary (look at subtypes)
std::string Chunk::name(ChunkType, const std::string&) const {
	if (typeMap.empty() == false) {
		auto i = typeMap.find(typeCode);
		if (i != typeMap.end()) {
			return i->second.first;
		}
	}
	return defaultChunkName();
}


ChunkType Chunk::type() const {
	if (typeMap.empty() == false) {
		auto i = typeMap.find(typeCode);
		if (i != typeMap.end()) {
			return i->second.second;
		}
	}
	return ChunkType::OTHER;
}


std::string Chunk::defaultChunkName() const {
	std::ostringstream ss;

	ss << "Unrecognized chunk <" << printableTypeCode() << ">";

	return ss.str();
}


unsigned int Chunk::readBytes(const char* data, unsigned char length, bool bigEndian) {
	unsigned int out   = 0;
	         int shift = (bigEndian ? ((length - 1) * 8) : 0);

	for (unsigned char i = 0; i < length; ++i, ++data) {
		out += (((unsigned int)(*data) & 0xFF) << shift);

		if (bigEndian) {
			// Will underflow, but loop exits before next use
			shift -= 8;
		} else {
			shift += 8;
		}
	}

	return out;
}


size_t Chunk::size() const {
	size_t out = 1;

	if (subChunks.empty() == false) {
		auto e = subChunks.cend();
		for (auto i = subChunks.cbegin(); i != e; ++i) {
			if (*i == NULL) {
				continue;
			}

			out += (*i)->size();
		}
	}

	return out;
}


bool Chunk::empty() const {
	if (subChunks.empty() == false) {
		auto e = subChunks.cend();
		for (auto i = subChunks.cbegin(); i != e; ++i) {
			if (*i == NULL) {
				continue;
			}

			if ((*i)->empty() == false) {
				return false;
			}
		}
	}

	return true;
}
