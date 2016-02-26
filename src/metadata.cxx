#include <metadata.hxx>

#include <fstream>
#include <iostream>


Metadata::Metadata(const std::string& path) : file(path, std::ifstream::binary) {
	// TODO Check validity
}

Metadata::Metadata(const Metadata& m) {
	tags = m.tags;
	// file will be closed by this point and doesn't need copying
}

Metadata::Metadata(Metadata&& m) {
	tags.swap(m.tags);
	// file will be closed by this point and doesn't need copying
}


Metadata::~Metadata() {
	if (bRef != NULL) {
		delete bRef;
	}
	if (rRef != NULL) {
		delete rRef;
	}
}


void Metadata::create() {
	try {
		read(file);
	} catch (char e) {
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


size_t Metadata::size() noexcept {
	if (tags.empty()) {
		return 0;
	}

	size_t out = 0;
	auto e = end();
	for (auto i = begin(); i != e; ++i) {
		out += i->size();
	}

	return out;
}


bool Metadata::empty() noexcept {
	if (tags.empty()) {
		return true;
	}

	auto e = end();
	for (auto i = begin(); i != e; ++i) {
		if (i->empty() == false) {
			return false;
		}
	}

	return true;
}


MetadataIterator* Metadata::beginReference() {
	if (bRef != NULL) {
		delete bRef;
	}

	bRef = new MetadataIterator(this, false);
	return bRef;
}

MetadataIterator* Metadata::rbeginReference() {
	if (rRef != NULL) {
		delete rRef;
	}

	rRef = new MetadataIterator(this, true);
	return rRef;
}
