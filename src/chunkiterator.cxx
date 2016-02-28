#include <chunkiterator.hxx>

#include <chunk.hxx>
#include <metadata.hxx>
#include <metadatatag.hxx>
#include <metadataiterator.hxx>

#include <stdexcept>


ChunkIterator::ChunkIterator(Chunk* c, bool reverse) {
	chunk = c;
	if (chunk == NULL) {
		return;
	}

	tStart = --(chunk->subChunks.begin());
	tEnd   = chunk->subChunks.end();

	// Delegate to *crement operators to avoid duplicating code
	if (reverse == false) {
		state = State::BEGIN;
		operator++();
	} else {
		state = State::END;
		operator--();
	}
}


ChunkIterator& ChunkIterator::operator++() {
	switch (state) {
		case State::BEGIN:
			if ((chunk == NULL) || (chunk->empty())) {
				state = State::END;
			} else {
				top = tStart;
				++top;

				if (top == tEnd) {
					state = State::END;
					break;
				}

				updateToMetadata(false);
				state = State::VALID;
			}
			break;
		case State::VALID:
			++(*inner);
			if (inner->atEnd()) {
				++top;

				if (top == tEnd) {
					state = State::END;
					break;
				}

				updateToMetadata(false);
			}
			break;
		default:
			break;
	}

	return *this;
}

ChunkIterator& ChunkIterator::operator--() {
	switch (state) {
		case State::VALID:
			--(*inner);
			if (inner->atStart()) {
				--top;

				if (top == tStart) {
					state = State::BEGIN;
					break;
				}

				updateToMetadata(true);
			}
			break;
		case State::END:
			if ((chunk == NULL) || (chunk->empty())) {
				state = State::BEGIN;
			} else {
				top = tEnd;
				--top;

				if (top == tStart) {
					state = State::BEGIN;
					break;
				}

				updateToMetadata(true);
				state = State::VALID;
			}
			break;
		default:
			break;
	}

	return *this;
}

MetadataTag* ChunkIterator::operator->() {
	if (chunk == NULL) {
		throw std::out_of_range("NULL-pointing Chunk* attempted to be dereferenced");
	}

	switch (state) {
		case State::BEGIN:
		case State::END:
			throw std::out_of_range("Chunk index out of bounds");
		case State::VALID:
		default:
			return &**inner;
	}
}

bool ChunkIterator::operator==(const ChunkIterator& rhs) noexcept {
	if (chunk != rhs.chunk) {
		return false;
	}

	if ((chunk == NULL) || (chunk->empty())) {
		return true;
	}

	if (state != rhs.state) {
		return false;
	}

	return ((top == rhs.top) && (inner == rhs.inner));
}

bool ChunkIterator::atStart() const {
	if (chunk == NULL) {
		return true;
	}

	return (state == State::BEGIN);
}

bool ChunkIterator::atEnd() const {
	if (chunk == NULL) {
		return true;
	}

	return (state == State::END);
}


void ChunkIterator::updateToMetadata(bool reverse) {
	if ((*top)->empty()) {
		// Point inner into the right chunk to avoid any potential odd behaviour
		inner = (*top)->beginReference();
		return;
	}

	if (reverse) {
		inner = (*top)->rbeginReference();

		try {
			while ((inner->atStart() == false) && (*inner)->empty()) {
				--(*inner);
			}
		} catch (std::out_of_range) {}
	} else {
		inner = (*top)->beginReference();

		try {
			while ((inner->atEnd() == false) && (*inner)->empty()) {
				++(*inner);
			}
		} catch (std::out_of_range) {}
	}
}
