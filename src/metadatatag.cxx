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


size_t MetadataTag::size() noexcept {
	if (ref.empty()) {
		return 0;
	}

	size_t out = 0;
	auto e = ref.end();
	for (auto i = ref.begin(); i != e; ++i) {
		out += (*i)->size();
	}

	return out;
}


bool MetadataTag::empty() noexcept {
	if (ref.empty()) {
		return true;
	}

	auto e = ref.end();
	for (auto i = ref.begin(); i != e; ++i) {
		if ((*i)->empty() == false) {
			return false;
		}
	}

	return true;
}
