#include <jpegmetadata.hxx>

#include <jpegchunk.hxx>

#include <iostream>


void JPEGMetadata::read(std::ifstream& file) {
	while (file.peek() != EOF) {
		Chunk* c = NULL;
		try {
			c = new JPEGChunk(file);
		} catch (char e) {
			std::cout << "ERROR: couldn't read chunk ";
			switch (e) {
				case 'T':
					std::cout << "type code";
					break;
				case 'L':
					std::cout << "length";
					break;
				case 'D':
					std::cout << "data";
					break;
			}
			std::cout << std::endl;

			delete c;
			throw 'C';
		}

		std::string n = c->name();
		auto i = tags.begin(), e = tags.end();
		switch (c->type()) {
			case Chunk::Type::COUNT:
				for (; i != e; ++i) {
					if (i->label == n) {
						i->addChunk(c);
						i->data = (std::to_string(i->size()) + " occurences");

						// Need to break out of both for and switch blocks
						goto end_switch;
					}
				}

				tags.emplace_back(c);
				tags.back().data = "1 occurence";
				break;
			default:
				tags.emplace_back(c);
				break;
		end_switch:
			;
		}
	}
}

void JPEGMetadata::write(const std::string& path) const {
	std::ofstream out(path, std::ofstream::binary);

	for (auto t = tags.begin(); t != tags.end(); ++t) {
		for (auto c = t->begin(); c != t->end(); ++c) {
			(*c)->write(out);
		}
	}

	out.close();
}
