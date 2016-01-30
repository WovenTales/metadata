#include <metadata.hxx>

#include <cstring>


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
		return false;
	}

	// Check against PNG header
	if(strcmp(header, "\x89PNG\r\n\x1A\n")) {
		return false;
	}

	while (file.peek() != EOF) {
		try {
			chunks.push_back(Chunk(file));
		} catch (char e) {
			// Thrown from Chunk constructor; not added to vector
			// If ever want to try to keep alive, switch over 'L', 'T', 'D', 'C'
			return false;
		}
	}

	return true;
}
