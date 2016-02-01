#ifndef CHUNK_H
#define CHUNK_H


#include <cstddef>

#include <istream>
#include <map>
#include <string>


struct Chunk {
public:
	enum struct Type {
		OTHER,
		TEXT,
	};

private:
	static std::map< std::string, Type > typeMap;

public:
	unsigned int   length = 0;
	std::string    typeCode;
	unsigned char* raw = NULL;

	Chunk(const Chunk&) = delete;
	Chunk(Chunk&&);
	Chunk(std::istream&);

	virtual ~Chunk();

	std::string data() const;
	std::string name() const;
	Type        type() const;
};


#endif
