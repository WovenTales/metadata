#include <metadata.hxx>

#include <cstring>
#include <iostream>


Metadata::Tag::Tag(Chunk::Type t, const std::string& l, const std::string& d) {
	type = t;
	label = l;
	data = d;
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
	if(strcmp(header, "\x89PNG\r\n\x1A\n")) {
		std::cout << "ERROR: wrong header" << std::endl;
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

		auto c = raw.rbegin();
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
				tags.emplace_back(t, c->name(), c->data());
				break;
		end_switch:
			;
		}
	}

	return true;
}
