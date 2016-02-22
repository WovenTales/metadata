#include <pngmetadata.hxx>

#include <pngchunk.hxx>

#include <fstream>
#include <iostream>


PNGMetadata::PNGMetadata(const std::string& path) {
	std::ifstream file(path, std::ifstream::binary);

	char header[9];
	// NULL-terminate to emulate C-string
	header[8] = 0x00;

	file.clear();
	file.read(header, 8);  // Not using get() as that would break on newline
	if (!file.good()) {
		std::cout << "ERROR: bad file" << std::endl;
		file.close();
		throw 'F';
	}

	// Check against PNG header
	if (header != std::string("\x89PNG\r\n\x1A\n")) {
		std::cout << "ERROR: wrong file header" << std::endl;
		file.close();
		throw 'H';
	}

	while (file.peek() != EOF) {
		Chunk* c = NULL;
		try {
			c = new PNGChunk(file);
		} catch (char e) {
			std::cout << "ERROR: couldn't read chunk ";
			switch (e) {
				case 'L':
					std::cout << "length";
					break;
				case 'T':
					std::cout << "type code";
					break;
				case 'D':
					std::cout << "data";
					break;
				case 'C':
					std::cout << "CRC";
					break;
			}
			std::cout << std::endl;

			// TODO: Check behaviour for exception in constructor with new (need delete?)
			file.close();
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

	file.close();
}

void PNGMetadata::write(const std::string& path) const {
	std::ofstream out(path);

	out.write("\x89PNG\r\n\x1A\n", 8);

	for (auto t = tags.begin(); t != tags.end(); ++t) {
		for (auto c = t->begin(); c != t->end(); ++c) {
			(*c)->write(out);
		}
	}

	out.close();
}
