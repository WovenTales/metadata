#include <metadata.hxx>

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
