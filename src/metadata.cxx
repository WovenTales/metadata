#include <metadata.hxx>

#include <fstream>
#include <iostream>


Metadata::Metadata(const Metadata& m) {
	type = m.type;
	tags = m.tags;
}

Metadata::Metadata(Metadata&& m) {
	type = m.type;
	tags.swap(m.tags);
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
	type = m.type;
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
		for (MetadataTag t : tags) {
			out += t.size();
		}
	}

	return out;
}


bool Metadata::empty() const {
	if (tags.empty() == false) {
		// For some reason, segfaults here and only here if not reference
		for (const MetadataTag& t : tags) {
			if (t.size() != 0) {
				return false;
			}
		}
	}

	return true;
}
