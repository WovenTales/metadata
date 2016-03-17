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

	std::cout << "constructor" << std::endl;
	initialize(reverse);
}


ChunkIterator& ChunkIterator::operator++() {
	std::cout << "increment" << std::endl;
	switch (state) {
		case State::BEGIN:
			initialize(false);
			break;
		case State::VALID:
			step(false);
			break;
		default:
			break;
	}

	return *this;
}

ChunkIterator& ChunkIterator::operator--() {
	switch (state) {
		case State::VALID:
			step(true);
			break;
		case State::END:
			initialize(true);
			break;
		default:
			break;
	}

	return *this;
}

MetadataTag* ChunkIterator::operator->() {
	if (chunk == NULL) {
		throw std::out_of_range("NULL-pointing Chunk* attempted to be dereferenced");
	} else if (inner == NULL) {
		throw std::out_of_range("NULL-pointing MetadataIterator* attempted to be dereferenced");
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
	if ((chunk == NULL) || inner == NULL) {
		return true;
	}

	return ((state == State::BEGIN) || inner->atStart());
}

bool ChunkIterator::atEnd() const {
	if ((chunk == NULL) || (inner == NULL)) {
		return true;
	}

	return ((state == State::END) || inner->atEnd());
}


void ChunkIterator::initialize(bool reverse) {
	if ((chunk == NULL) || (chunk->empty())) {
		state = (reverse ? State::BEGIN : State::END);
		return;
	}

	if (reverse) {
		top = tEnd;
		--top;
	} else {
		top = tStart;
		++top;
	}

	std::cout << "init" << std::endl;
	updateToMetadata(reverse);
}

void ChunkIterator::step(bool reverse) {
	if ((reverse && atStart()) || ((reverse == false) && atEnd()) ||
			(chunk == NULL) || (chunk->empty())) {
		state = (reverse ? State::BEGIN : State::END);
		return;
	}

	auto test = [reverse, this](){ return (reverse ? inner->atStart() : inner->atEnd()); };
	if (test() == false) {
		if (reverse) {
			--(*inner);
		} else {
			++(*inner);
		}

		if (test()) {
			std::cout << "step" << std::endl;
			if (reverse) {
				--top;
			} else {
				++top;
			}

			updateToMetadata(reverse);

		} else if ((*inner)->empty()) {
			step(reverse);
		}
	}
}


void ChunkIterator::updateToMetadata(bool reverse) {
	if ((chunk == NULL) || (top == tStart) || (top == tEnd)) {
		std::cout << "end" << std::endl;
		state = (reverse ? State::BEGIN : State::END);
		return;
	}

	state = State::VALID;

	if (reverse) {
		inner = (*top)->rbeginReference();
	} else {
		inner = (*top)->beginReference();
	}

	std::cout << "update" << std::endl;
	if (((inner->atStart() || inner->atEnd()) == false) && (*inner)->empty()) {
		step(reverse);
	}
}
