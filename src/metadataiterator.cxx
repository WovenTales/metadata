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


MetadataIterator& MetadataIterator::operator++() {
	switch (state) {
		case State::BEGIN:
			if ((data == NULL) || (data->empty())) {
				state = State::END;
			} else {
				top = tStart;
				++top;

				if (top == tEnd) {
					state = State::END;
					break;
				}

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

				if (top == tEnd) {
					state = State::END;
					break;
				}

				updateToTag(false);
			}
			break;
		case State::INNER:
			{
				bool first = true;
				while (inner.atEnd() == false) {
					if (first || inner->empty()) {
						++inner;
						first = false;
					} else {
						break;
					}
				}

				if (inner.atEnd()) {
					first = true;
					while (chunk != cEnd) {
						if (first || (*chunk)->empty()) {
							++chunk;
							first = false;
						} else {
							break;
						}
					}

					if (chunk == cEnd) {
						++top;
						updateToTag(false);

						state = State::TOP;
					} else {
						updateToChunk(false);
					}
				}
			}
			break;
		default:
			break;
	}

	return *this;
}

MetadataIterator& MetadataIterator::operator--() {
	switch (state) {
		case State::INNER:
			{
				bool first = true;
				while (inner.atStart() == false) {
					if (first || inner->empty()) {
						--inner;
						first = false;
					} else {
						break;
					}
				}

				if (inner.atStart()) {
					first = true;
					while (chunk != cStart) {
						if (first || (*chunk)->empty()) {
							++chunk;
							first = false;
						} else {
							break;
						}
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
			}
		case State::TOP:
			--top;

			if (top == tStart) {
				state = State::BEGIN;
				break;
			}

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

				if (top == tStart) {
					state = State::BEGIN;
					break;
				}

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

	if (data == NULL) {
		return true;
	}

	if (state != rhs.state) {
		return false;
	}

	return ((top == rhs.top) && (inner == rhs.inner));
}

bool MetadataIterator::atStart() const {
	if (data == NULL) {
		return true;
	}

	return (state == State::BEGIN);
}

bool MetadataIterator::atEnd() const {
	if (data == NULL) {
		return true;
	}

	return (state == State::END);
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

		while ((chunk != cEnd) && (*chunk)->empty()) {
			++chunk;
		}
	} else {
		cStart = top->begin();
		--cStart;

		cEnd   = top->end();
		chunk = cEnd;
		--chunk;

		while ((chunk != cStart) && (*chunk)->empty()) {
			--chunk;
		}
	}

	updateToChunk(reverse);
}

void MetadataIterator::updateToChunk(bool reverse) {
	if ((*chunk)->empty()) {
		// Point inner into the right chunk to avoid any potential odd behaviour
		inner = (*chunk)->begin();
		return;
	}

	if (reverse) {
		inner = (*chunk)->rbegin();

		try {
			while ((inner.atStart() == false) && inner->empty()) {
				--inner;
			}
		} catch (std::out_of_range) {}
	} else {
		inner = (*chunk)->begin();

		try {
			while ((inner.atEnd() == false) && inner->empty()) {
				++inner;
			}
		} catch (std::out_of_range) {}
	}
}
