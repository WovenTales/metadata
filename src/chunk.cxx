#include <chunk.hxx>

#include <algorithm>
#include <iomanip>
#include <sstream>


std::string Chunk::sanitize(std::string str) {
	size_t i = -1;
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

	raw = new char[length];
	std::copy(c.raw, (c.raw + length), raw);
}

Chunk::Chunk(Chunk&& c) : typeMap(c.typeMap) {
	length = c.length;
	typeCode = c.typeCode;

	raw = c.raw;
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

std::string Chunk::data(Chunk::Type t) const {
	unsigned int print = 0, hex = 0;
	unsigned long sum = 0;
	std::ostringstream out;

	switch (t) {
		case Type::OTHER:
			out << hexString(false, 0, 0, &print, &hex);

			// If it seems likely this is a text field, interpret it as such
			if (print > hex) {
				out.seekp(0);
				out << std::string(raw, length);
			}

			return out.str();
		case Type::NONE:
			return "Should not be shown";
		case Type::HIDE:
			return "Should not be shown";
		case Type::CUSTOM:
			return "SUBCLASS-DEFINED";


		case Type::HEX:
			return hexString();


		case Type::DIGIT:
			for (unsigned int i = 0; i < length; ++i) {
				sum = sum << 8;
				sum += (unsigned char)raw[i];
			}
			return std::to_string(sum);
		case Type::TEXT:
			return std::string(raw, length);


		case Type::COLOR:
			return "TODO";
		case Type::COUNT:
			return "";
		case Type::TIME:
			return "TODO";
	}

	return "ERROR";
}


std::string Chunk::name() const {
	auto i = typeMap.find(typeCode);
	if (i != typeMap.end()) {
		return name(i->second.second, i->second.first);
	} else {
		return defaultChunkName(typeCode);
	}
}

std::string Chunk::name(Chunk::Type, const std::string& title) const {
	return title;
}


Chunk::Type Chunk::type() const {
	auto i = typeMap.find(typeCode);
	if (i != typeMap.end()) {
		return i->second.second;
	} else {
		return Type::OTHER;
	}
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
