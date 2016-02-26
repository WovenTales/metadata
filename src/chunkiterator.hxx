#include <chunkiterator.hxx>

#include <stdexcept>


MetadataIterator::MetadataIterator(Metadata& m, bool reverse) : data(m) {
	tStart = data.tags.rend();
	tEnd   = data.tags.end();

	if (reverse == false) {
		if (data.empty()) {
			state = State::BEGIN;
		} else {
			top = data.tags.begin();
			updateToTag(false);

			state = State::TOP;
		}
	} else {
		if (data.empty()) {
			state = State::END;
		} else {
			top = data.tags.rbegin();
			updateToTag(true);

			if (tHasChildren) {
				state = State::INNER;
			} else {
				state = State::TOP;
			}
		}
	}
}


MetadataIterator& MetadataIterator::operator++() {
	switch (state) {
		case State::BEGIN:
			if (data.empty()) {
				state = State::END;
			} else {
				top = data.tags.begin();
				updateToTag(false);

				state = State::TOP;
			}
			break;
		case State::TOP:
			if (tHasChildren) {
				// tHasChildren is set in last updateToTag() call:
				//   if uTT(false), inner points to first child
				//   if uTT(true), to have reached TOP will have traversed all children
				state = State::INNER;
			} else {
				++top;
				updateToTag(false);
			}
			break;
		case State::INNER:
			++inner;
			if (inner == iEnd) {
				++top;
				updateToTag(false);

				state = State::TOP;
			}
			break;
	}

	return *this;
}

MetadataIterator& MetadataIterator::operator--() {
	switch (state) {
		case State::INNER:
			--inner;
			if (inner == iStart) {
				state = State::TOP;
				// fall through
			} else {
				break;
			}
		case State::TOP:
			--top;
			updateToTag(true);

			if (tHasChildren) {
				state = State::INNER;
			}
			break;
		case State::END:
			if (data.empty()) {
				state = State::BEGIN;
			} else {
				tag = data.tags.rbegin();
				updateToTag(true);

				if (tHasChildren) {
					state = State::INNER;
				} else {
					state = State::TOP;
				}
			}
			break;
	}

	return *this;
}

Metadata::Tag& MetadataIterator::operator*() {
	switch (state) {
		case State::BEGIN:
		case State::END:
			throw std::out_of_range;
		case State::TOP:
			return *top;
		case State::INNER:
			return *inner;
	}
}

bool MetadataIterator::operator(MetadataIterator& rhs) {
	if (data != rhs.data) {
		return false;
	}

	if (data.empty()) {
		// Beginning and ending equal iff there's no tags between them
		if ((state == State::BEGIN) || (state == State::END)) {
			return ((rhs.state == State::BEGIN) || (rhs.state == State::END));
		}
	}

	if (state != rhs.state) {
		return false;
	}

	return ((top == rhs.top) && (inner == rhs.inner));
}


void MetadataIterator::updateToTag(bool reverse) {
	chunk  = (reverse ? tag->rbegin() : tag->begin());
	cStart = tag->rend();
	cEnd   = tag->end();

	if ((chunk == cStart) || (inner == cEnd)) {
		tHasChildren = false;
		return;
	}

	// TODO Update to storing children, not returning
	if (chunk->empty()) {
		return nextChunk(reverse);
	} else {
		updateToChunk(reverse);
		return true;
	}
}

// Returns whether the chunk has any children
bool MetadataIterator::updateToChunk(bool reverse) {
	inner  = (reverse ? chunk->rbegin() : chunk->begin());
	iStart = tag->rend();
	iEnd   = tag->end();

	// Chunks should never have empty Metadata, but handle just in case
	if ((inner == iStart) || (inner == iEnd)) {
		return false;
	}

	if (inner->empty()) {
		return nextInner(reverse);
	} else {
		return true;
	}
}


// Returns whether a non-empty Chunk exists in direction
bool MetadataIterator::nextChunk(bool reverse) {
	if (reverse) {
		--chunk;
		if (chunk == cStart) {
			return false;
		}
	} else {
		++chunk;
		if (chunk == cEnd) {
			return false;
		}
	}

	if (chunk->empty()) {
		return nextChunk(reverse);
	} else {
		updateToChunk(reverse);
		return true;
	}
}

// Returns whether a non-empty Chunk exists in direction
bool MetadataIterator::nextInner(bool reverse) {
	if (reverse) {
		--inner;
		if (inner == iStart) {
			return false;
		}
	} else {
		++inner;
		if (inner == iEnd) {
			return false;
		}
	}

	if (inner->empty()) {
		return nextInner(reverse);
	} else {
		return true;
	}
}
