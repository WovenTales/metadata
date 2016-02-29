#include <metadata.hxx>

#include <fstream>
#include <iostream>


Metadata::Metadata(const Metadata& m) {
	tags = m.tags;
}

Metadata::Metadata(Metadata&& m) {
	tags.swap(m.tags);
}


Metadata::~Metadata() {
	if (bRef != NULL) {
		delete bRef;
	}
	if (rRef != NULL) {
		delete rRef;
	}
}


void Metadata::create(const std::string& path) {
	std::ifstream file(path, std::ifstream::binary);

	try {
		read(file);
	} catch (char e) {
		std::cout << "ERROR: file read failed" << std::endl;
		file.close();
		throw e;
	}

	file.close();
}


Metadata& Metadata::operator=(Metadata&& m) {
	tags.swap(m.tags);

	return (*this);
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


size_t Metadata::size() const {
	size_t out = 0;

	if (tags.empty() == false) {
		auto e = tags.end();
		for (auto i = tags.begin(); i != e; ++i) {
			out += i->size();
		}
	}

	return out;
}


bool Metadata::empty() const {
	if (tags.empty() == false) {
		auto e = tags.end();
		for (auto i = tags.begin(); i != e; ++i) {
			if (i->size() != 0) {
				return false;
			}
		}
	}

	return true;
}


MetadataIterator* Metadata::beginReference() {
	if (bRef == NULL) {
		bRef = new MetadataIterator(this, false);
	}

	return bRef;
}

MetadataIterator* Metadata::rbeginReference() {
	if (rRef == NULL) {
		rRef = new MetadataIterator(this, true);
	}

	return rRef;
}
