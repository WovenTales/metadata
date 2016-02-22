#include <metadata.hxx>

#include <cstring>
#include <iostream>


Metadata::Tag::Tag(const Chunk* r) : ref(1, r) {
	type = r->type();
	label = r->name();
	data = r->data();
}

Metadata::Tag::~Tag() {
	while (ref.empty() == false) {
		delete ref.front();
		ref.pop_front();
	}
}

void Metadata::Tag::addChunk(const Chunk* c) {
	ref.push_back(c);
}

bool Metadata::Tag::required() const {
	if (ref.empty()) {
		return true;
	} else {
		for (auto i = ref.begin(); i != ref.end(); ++i) {
			if ((*i)->required() == true) {
				return true;
			}
		}
		return false;
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
	if (header != std::string("\x89PNG\r\n\x1A\n")) {
		std::cout << "ERROR: wrong file header" << std::endl;
		return false;
	}

	while (file.peek() != EOF) {
		Chunk* c = NULL;
		try {
			c = new Chunk(file);
		} catch (char e) {
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

			// TODO: Check behaviour for exception in constructor with new (need delete?)
			return false;
		}

		std::string n = c->name();
		auto i = tags.begin(), e = tags.end();
		switch (c->type()) {
			case Chunk::Type::COUNT:
				for (; i != e; ++i) {
					if (i->label == n) {
						i->addChunk(c);
						i->data = (std::to_string(i->ref.size()) + " occurences");

						// Need to break out of both for and switch blocks
						goto end_switch;
					}
				}

				tags.emplace_back(c);
				tags.back().data = "1 occurence";
				break;
			default:
				tags.emplace_back(c);
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

	for (auto t = tags.begin(); t != tags.end(); ++t) {
		for (auto c = t->ref.begin(); c != t->ref.end(); ++c) {
			(*c)->write(out);
		}
	}

	out.close();
}

bool Metadata::remove(unsigned int index) {
	// Access to index longer than vector, but removal doesn't trigger copying
	auto iTag = tags.begin();
	auto eTag = tags.end();
	for (unsigned int i = 0; i < index; ++i, ++iTag) {
		if (iTag == eTag) {
			std::cout << "ERROR: removal index out of bounds" << std::endl;
			return false;
		}
	}

	if (iTag->ref.empty()) {
		std::cout << "ERROR: no chunk associated with tag for removal" << std::endl;
		return false;
	}

	tags.erase(iTag);
	return true;
}
