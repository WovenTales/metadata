#include <metadata.hxx>

#include <cstring>
#include <iostream>


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
	chunks.swap(m.chunks);

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
			chunks.push_back(Chunk(file, &chunks));
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

		if (chunks.back().type() == Chunk::Type::HIDE) {
			chunks.pop_back();
		}
	}

	return true;
}
