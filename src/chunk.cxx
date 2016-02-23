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
	return sanitize(data(type()));
}

std::string Chunk::data(Chunk::Type t) const {
	unsigned int print = 0, hex = 0;
	std::ostringstream out;

	switch (t) {
		case Type::OTHER:
			out << hexString(&print, &hex);

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


		case Type::HEX:
			return hexString();


		case Type::TEXT:
			return std::string(raw, length);
		case Type::CTEXT:
			return "TODO";
		case Type::ITEXT:
			return "TODO";


		case Type::COLOR:
			return "TODO";
		case Type::COUNT:
			return "";
		case Type::HEADER:
			return hexString();
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

std::string Chunk::name(Chunk::Type type, const std::string& title) const {
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
