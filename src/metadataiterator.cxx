#include <metadataiterator.hxx>

#include <chunk.hxx>
#include <chunkiterator.hxx>
#include <metadata.hxx>

#include <stdexcept>


MetadataIterator::MetadataIterator(Metadata* m, bool reverse) {
	data   = m;
	if (data == NULL) {
		return;
	}

	tStart = --(data->tags.begin());
	tEnd   = data->tags.end();

	// Delegate to *crement operators to avoid duplicating code
	if (reverse == false) {
		state = State::BEGIN;
		operator++();
	} else {
		state = State::END;
		operator--();
	}
}


MetadataIterator& MetadataIterator::operator++() noexcept {
	switch (state) {
		case State::BEGIN:
			if ((data == NULL) || (data->empty())) {
				state = State::END;
			} else {
				top = tStart;
				++top;
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
			while ((inner->empty()) && (inner.atEnd() == false)) {
				++inner;
			}
			if (inner.atEnd()) {
				++chunk;
				while (((*chunk)->empty()) && (chunk != cEnd)) {
					++chunk;
				}
				if (chunk == cEnd) {
					++top;
					updateToTag(false);

					state = State::TOP;
				} else {
					updateToChunk(false);
				}
			}
			break;
		default:
			break;
	}

	return *this;
}

MetadataIterator& MetadataIterator::operator--() noexcept {
	switch (state) {
		case State::INNER:
			--inner;
			while ((inner->empty()) && (inner.atStart() == false)) {
				--inner;
			}
			if (inner.atStart()) {
				--chunk;
				while (((*chunk)->empty()) && (chunk != cStart)) {
					--chunk;
				}
				if (chunk == cStart) {
					state = State::TOP;
					// fall through
				} else {
					updateToChunk(true);
					break;
				}
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
			if ((data == NULL) || (data->empty())) {
				state = State::BEGIN;
			} else {
				top = tEnd;
				--top;
				updateToTag(true);

				if (tHasChildren) {
					state = State::INNER;
				} else {
					state = State::TOP;
				}
			}
			break;
		default:
			break;
	}

	return *this;
}

MetadataTag* MetadataIterator::operator->() {
	if (data == NULL) {
		throw std::out_of_range("NULL-pointing Metadata* attempted to be dereferenced");
	}

	switch (state) {
		case State::BEGIN:
		case State::END:
			throw std::out_of_range("Tag index out of bounds");
		case State::TOP:
			return &*top;
		case State::INNER:
			return &*inner;
	}

	return &*top;
}

bool MetadataIterator::operator==(const MetadataIterator& rhs) noexcept {
	if (data != rhs.data) {
		return false;
	}

	if ((data == NULL) || (data->empty())) {
		return true;
	}

	if (state != rhs.state) {
		return false;
	}

	return ((top == rhs.top) && (inner == rhs.inner));
}


void MetadataIterator::updateToTag(bool reverse) {
	if ((data == NULL) || (top == tStart)) {
		tHasChildren = false;
		state = State::BEGIN;
		return;
	} else if (top == tEnd) {
		tHasChildren = false;
		state = State::END;
		return;
	} else if (top->empty()) {
		tHasChildren = false;
		state = State::TOP;
		return;
	}

	tHasChildren = true;

	if (reverse == false) {
		cStart = top->begin();
		chunk = cStart;
		--cStart;

		cEnd   = top->end();

		while ((*chunk)->empty()) {
			++chunk;
		}
	} else {
		cStart = top->begin();
		--cStart;

		cEnd   = top->end();
		chunk = cEnd;
		--chunk;

		while ((*chunk)->empty()) {
			--chunk;
		}
	}

	updateToChunk(reverse);
}

// Assumes *chunk is not empty
void MetadataIterator::updateToChunk(bool reverse) {
	if (reverse == false) {
		inner = (*chunk)->begin();

		while (inner->empty()) {
			++inner;
		}
	} else {
		inner = (*chunk)->rbegin();

		while (inner->empty()) {
			--inner;
		}
	}
}
