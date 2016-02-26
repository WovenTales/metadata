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

	cStart = --(chunk->subChunks.begin());
	cEnd   = chunk->subChunks.end();

	// Delegate to *crement operators to avoid duplicating code
	if (reverse == false) {
		state = State::BEGIN;
		operator++();
	} else {
		state = State::END;
		operator--();
	}
}


ChunkIterator& ChunkIterator::operator++() noexcept {
	switch (state) {
		case State::BEGIN:
			if ((chunk == NULL) || (chunk->empty())) {
				state = State::END;
			} else {
				top = cStart;
				++top;
				updateToChunk(false);

				state = State::VALID;
			}
			break;
		case State::VALID:
			++(*inner);
			if (inner->atEnd()) {
				++top;
				updateToChunk(false);
			}
			break;
		default:
			break;
	}

	return *this;
}

ChunkIterator& ChunkIterator::operator--() noexcept {
	switch (state) {
		case State::VALID:
			--(*inner);
			if (inner->atStart()) {
				--top;
				updateToChunk(true);
			}
			break;
		case State::END:
			if ((chunk == NULL) || (chunk->empty())) {
				state = State::BEGIN;
			} else {
				top = cEnd;
				--top;
				updateToChunk(true);

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
			throw std::out_of_range("Tag index out of bounds");
		case State::VALID:
			return &**inner;
	}

	return &**inner;
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


void ChunkIterator::updateToChunk(bool reverse) {
	if ((chunk == NULL) || (top == cStart)) {
		state = State::BEGIN;
		return;
	} else if (top == cEnd) {
		state = State::END;
		return;
	}

	inner = (reverse ? (*top)->rbeginReference() : (*top)->beginReference());

	while ((*inner)->empty()) {
		if (reverse) {
			--(*inner);
		} else {
			++(*inner);
		}
	}
}
