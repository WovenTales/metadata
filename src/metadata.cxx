#include <metadata.hxx>

#include <ios>


Metadata::Metadata(Metadata&& m) {
	file.swap(m.file);
}
Metadata::Metadata(const std::string& path) {
	file.open(path, std::ifstream::binary);
}


Metadata& Metadata::operator=(Metadata&& m) {
	file.swap(m.file);

	return (*this);
}


Metadata::~Metadata() {
	if (file.is_open()) file.close();
}


bool Metadata::isValid() const {
	return file.is_open();
}
