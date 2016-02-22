#include <chunk.hxx>

#include <algorithm>
#include <iomanip>
#include <sstream>


std::string Chunk::sanitize(std::string str) {
	size_t i;
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


std::string Chunk::hexString(unsigned int* print, unsigned int* hex) const {
	std::ostringstream ss;

	ss << std::hex;

	ss << "<code>";
	for (unsigned int i = 0; i < length; ++i) {
		if (i > 0) {
			ss << " ";
		}

		ss << std::setw(2) << std::setfill('0');
		ss << ((unsigned int)raw[i] & 0xFF);

		if ((print != NULL) || (hex != NULL)) {
			if (std::isprint(raw[i])) {
				++print;
			} else {
				++hex;
			}
		}
	}
	ss << "</code>";

	return ss.str();
}


std::string Chunk::data() const {
	unsigned int print = 0, hex = 0;
	size_t n;
	std::ostringstream out;
	std::string str;

	switch (type()) {
		case Type::OTHER:
			out << hexString(&print, &hex);

			// If it seems likely this is a text field, interpret it as such
			if (print > hex) {
				out.seekp(0);
				out << sanitize(std::string(raw, length)) << "<br><br>";
			}

			return out.str();
		case Type::NONE:
			return "";
		case Type::HIDE:
			return "";


		case Type::HEX:
			return hexString();


		case Type::TEXT:
			str = sanitize(std::string(raw, length));
			n = str.find('\0');

			if (n == std::string::npos) {
				return str;
			} else {
				return str.substr(n + 1);
			}
			return (str.substr(0, n) + ": " + str.substr(n + 1));
		case Type::CTEXT:
			return "TODO";
		case Type::ITEXT:
			return "TODO";


		case Type::COLOR:
			return "TODO";
		case Type::COUNT:
			return "";
		case Type::PNGHEADER:
			return "TODO";
		case Type::TIME:
			return "TODO";
	}

	return "ERROR";
}

std::string Chunk::name() const {
	auto i = typeMap.find(typeCode);
	if (i != typeMap.end()) {
		if (i->second.second == Type::TEXT) {
			std::string str = sanitize(std::string(raw, length));
			size_t n = str.find('\0');

			if (n != std::string::npos) {
				return (i->second.first + " <" + str.substr(0, n) + ">");
			}
		}
		return i->second.first;
	} else {
		return defaultChunkName(typeCode);
	}
}

Chunk::Type Chunk::type() const {
	auto i = typeMap.find(typeCode);
	if (i != typeMap.end()) {
		return i->second.second;
	} else {
		return Type::OTHER;
	}
}
