#include <metadata.hxx>

#include <cstring>
#include <iostream>


Metadata::Tag::Tag(Chunk::Type t, const std::string& l, const std::string& d, const Chunk* r) {
	type = t;
	label = l;
	data = d;
	ref = r;
}

bool Metadata::Tag::required() const {
	if (ref == NULL) {
		return true;
	} else {
		return ref->required();
	}
}


Metadata::Metadata(Metadata&& m) {
	file.swap(m.file);
}
Metadata::Metadata(const std::string& path) {
	file.open(path, std::ifstream::binary);

	// Invalidate metadata if improper file
	if (read() == false) {
		file.close();
	}
}


Metadata& Metadata::operator=(Metadata&& m) {
	file.swap(m.file);
	raw.swap(m.raw);
	tags.swap(m.tags);

	return (*this);
}


Metadata::~Metadata() {
	if (file.is_open()) {
		file.close();
	}
}


bool Metadata::isValid() const {
	return file.is_open();
}


bool Metadata::read() {
	char header[9];
	header[8] = 0x00;

	file.clear();
	file.read(header, 8);  // Not using get() as that would break on newline
	if (!file.good()) {
		std::cout << "ERROR: bad file" << std::endl;
		return false;
	}

	// Check against PNG header
	if(strcmp(header, "\x89PNG\r\n\x1A\n")) {
		std::cout << "ERROR: wrong file header" << std::endl;
		return false;
	}

	while (file.peek() != EOF) {
		try {
			raw.emplace_back(file);
		} catch (char e) {
			// Thrown from Chunk constructor; not added to list

			std::cout << "ERROR: couldn't read chunk ";
			switch (e) {
				case 'L':
					std::cout << "length";
					break;
				case 'T':
					std::cout << "type code";
					break;
				case 'D':
					std::cout << "data";
					break;
				case 'C':
					std::cout << "CRC";
					break;
			}
			std::cout << std::endl;

			return false;
		}

		Chunk* c = &*(raw.rbegin());
		auto i = tags.begin(), e = tags.end();
		unsigned int d;
		size_t n;
		Chunk::Type t = c->type();
		switch (t) {
			case Chunk::Type::HIDE:
				break;
			case Chunk::Type::COUNT:
				for (; i != e; ++i) {
					if (i->type == t) {
						n = i->data.find(' ');
						d = std::stoi(i->data.substr(0, n));
						i->data = (std::to_string(d + 1) + " occurences");
						// Need to break out of both for and switch
						goto end_switch;
					}
				}

				tags.emplace_back(t, c->name(), "1 occurence");
				break;
			default:
				tags.emplace_back(t, c->name(), c->data(), c);
				break;
		end_switch:
			;
		}
	}

	return true;
}

void Metadata::write(const std::string& path) const {
	std::ofstream out(path);

	out.write("\x89PNG\r\n\x1A\n", 8);

	for (auto c = raw.begin(); c != raw.end(); ++c) {
		// Automatically truncates to single byte
		// Copy byte-by-byte to ensure proper order
		out.put(c->length >> (8 * 3));
		out.put(c->length >> (8 * 2));
		out.put(c->length >> 8);
		out.put(c->length);

		out.write(c->typeCode.c_str(), 4);
		out.write(c->raw, c->length);
		out.write(c->crc, 4);
	}

	out.close();
}

bool Metadata::remove(unsigned int index) {
	auto iTag = tags.begin();
	auto eTag = tags.end();
	for (unsigned int i = 0; i < index; ++i, ++iTag) {
		if (iTag == eTag) {
			std::cout << "ERROR: removal index out of bounds" << std::endl;
			return false;
		}
	}

	if (iTag->ref == NULL) {
		std::cout << "ERROR: no chunk associated with tag for removal" << std::endl;
		return false;
	}

	auto iRaw = raw.begin();
	auto eRaw = raw.end();
	while (&*iRaw != iTag->ref) {
		if (iRaw == eRaw) {
			std::cout << "ERROR: couldn't find chunk matching tag for removal" << std::endl;
			return false;
		}
		++iRaw;
	}

	tags.erase(iTag);
	raw.erase(iRaw);
	return true;
}
