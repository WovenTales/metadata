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

	initialize(reverse);
}


MetadataIterator& MetadataIterator::operator++() {
	switch (state) {
		case State::BEGIN:
			initialize(false);
			break;
		case State::TOP:
		case State::INNER:
			step(false);
			break;
		default:
			break;
	}

	return *this;
}

MetadataIterator& MetadataIterator::operator--() {
	switch (state) {
		case State::INNER:
		case State::TOP:
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


void MetadataIterator::initialize(bool reverse) {
	if ((data == NULL) || (data->empty())) {
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

	updateToTag(reverse);
}

void MetadataIterator::step(bool reverse) {
	if ((data == NULL) || (data->empty())) {
		state = (reverse ? State::END : State::BEGIN);
		return;
	}

	if (state == State::TOP) {
		if ((reverse == false) && (top->empty() == false)) {
			chunk = cStart;
			++chunk;
			updateToChunk(true);
		}

		if (reverse) {
			--top;
		} else {
			++top;
		}

		updateToTag(reverse);

	} else if (state == State::INNER) {
		stepInner(reverse);
	}
}

void MetadataIterator::stepInner(bool reverse) {
	if (reverse) {
		--inner;
	} else {
		++inner;
	}

	if (inner.atStart() || inner.atEnd()) {
		stepChunk(reverse);
	} else if (inner->empty()) {
		stepInner(reverse);
	}
}

void MetadataIterator::stepChunk(bool reverse) {
	if (reverse) {
		--chunk;
	} else {
		++chunk;
	}

	updateToChunk(reverse);
}


void MetadataIterator::updateToTag(bool reverse) {
	if ((data == NULL) || (top == tStart) || (top == tEnd)) {
		state = (reverse ? State::BEGIN : State::END);
		return;
	} else if (top->empty()) {
		state = State::TOP;
		return;
	}

	cStart = top->begin();
	cEnd   = top->end();
	if (reverse) {
		chunk = cEnd;
		--chunk;

		state = State::INNER;
	} else {
		chunk = cStart;

		state = State::TOP;
	}
	--cStart;

	updateToChunk(reverse);
}

void MetadataIterator::updateToChunk(bool reverse) {
	if (reverse && (chunk == cStart)) {
		state = State::TOP;
		return;
	} else if ((reverse == false) && (chunk == cEnd)) {
		++top;
		updateToTag(false);
		return;
	}

	if ((*chunk)->empty()) {
		stepChunk(reverse);
		return;
	}

	if (reverse) {
		inner = (*chunk)->rbegin();
	} else {
		inner = (*chunk)->begin();
	}

	if ((inner.atStart() || inner.atEnd()) == false) {
		if (inner->empty()) {
			stepInner(reverse);
		}
	}
}
