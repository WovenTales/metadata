#ifndef CHUNK_H
#define CHUNK_H


#include <cstddef>

#include <istream>
#include <string>


struct Chunk {
public:
	unsigned int length = 0;
	std::string  typeCode;
	char*        raw = NULL;

	Chunk(const Chunk&) = delete;
	Chunk(Chunk&&);
	Chunk(std::istream&);

	virtual ~Chunk();

	std::string type() const;
	std::string data() const;
};


#endif
