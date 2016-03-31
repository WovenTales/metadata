#include <metadatatag.hxx>

#include <chunk.hxx>
#include <metadataiterator.hxx>


MetadataTag::MetadataTag(Chunk* r) : ref(1, r) {
	type = r->type();
	label = r->name();
	data = r->data();
}

MetadataTag::MetadataTag(const MetadataTag& t) {
	operator=(t);
}

MetadataTag::MetadataTag(MetadataTag&& t) {
	operator=(t);
}

MetadataTag::~MetadataTag() {
	while (ref.empty() == false) {
		delete ref.front();
		ref.pop_front();
	}
}


MetadataTag& MetadataTag::operator=(const MetadataTag& rhs) {
	type = rhs.type;
	label = rhs.label;
	data = rhs.data;

	for (const Chunk* c : rhs.ref) {
		ref.push_back(c->clone());
	}

	return *this;
}

MetadataTag& MetadataTag::operator=(MetadataTag&& rhs) {
	type = rhs.type;
	label = rhs.label;
	data = rhs.data;

	ref = std::move(rhs.ref);

	return *this;
}


void MetadataTag::addChunkReference(Chunk* c) {
	ref.push_back(c);
}


bool MetadataTag::required() const {
	if (ref.empty()) {
		return true;
	} else {
		for (const Chunk* c : ref) {
			if (c->required() == true) {
				return true;
			}
		}
		return false;
	}
}


void MetadataTag::write(std::ofstream& out) {
	for (const Chunk* c : ref) {
		c->write(out);
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
