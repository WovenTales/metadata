#include <metadatatag.hxx>

#include <chunk.hxx>
#include <metadataiterator.hxx>


MetadataTag::MetadataTag(Chunk* r) : ref(1, r) {
	type = r->type();
	label = r->name();
	data = r->data();
}

MetadataTag::~MetadataTag() {
	while (ref.empty() == false) {
		delete ref.front();
		ref.pop_front();
	}
}


void MetadataTag::addChunk(Chunk* c) {
	ref.push_back(c);
}


bool MetadataTag::required() const {
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


void MetadataTag::write(std::ofstream& out) {
	for (auto c = ref.begin(); c != ref.end(); ++c) {
		(*c)->write(out);
	}
}


size_t MetadataTag::size() const {
	if (ref.empty()) {
		return 1;
	}

	size_t out = 1;
	for (Chunk* c : ref) {
		if (c == NULL) {
			continue;
		}

		out += c->size();
	}

	return out;
}


bool MetadataTag::empty() const {
	if (ref.empty()) {
		return true;
	}

	for (Chunk* c : ref) {
		if (c == NULL) {
			continue;
		}

		if (c->empty() == false) {
			return false;
		}
	}

	return true;
}
