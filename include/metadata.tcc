// In scope Metadata

protected:
template< typename T, class ... Params >
unsigned int loopChunks(std::istream& file, Params... args) {
	unsigned int count = 0;

	while (file.peek() != EOF) {
		Chunk* c = NULL;
		try {
			c = new T(file, args...);
		} catch (char e) {
			std::cout << "ERROR: couldn't read chunk ";
			switch (e) {
				case 'L':
					std::cout << "length";
					break;
				case 'H':
					std::cout << "header";
					break;
				case 'D':
					std::cout << "data";
					break;
				case 'C':
					std::cout << "CRC";
					break;
			}
			std::cout << std::endl;

			delete c;
			throw 'C';
		}

		std::string n = c->name();
		switch (c->type()) {
			case ChunkType::COUNT:
				for (MetadataTag t : tags) {
					if (t.label == n) {
						t.addChunkReference(c);
						t.data = (std::to_string(t.size()) + " occurences");

						/* Need to break out of both for and switch blocks */
						goto end_switch;
					}
				}

				// Note that this has to be emplace_* rather than push_*
				//   as otherwise the destructor will free in-use memory
				// TODO Fix above
				tags.emplace_back(c);
				tags.back().data = "1 occurence";
				break;
			default:
				tags.emplace_back(c);
				break;
		end_switch:
			;
		}

		++count;
	}

	return count;
}
