#ifndef METADATA_H
#define METADATA_H


#include <chunk.hxx>

#include <fstream>
#include <list>
#include <string>


class Metadata {
public:
	struct Tag {
	public:
		Chunk::Type type;
		std::string label;
		std::string data;

		Tag(Chunk::Type, const std::string&, const std::string&);
	};

private:
	std::ifstream      file;
	std::list< Chunk > raw;
	std::list< Tag >   tags;

	bool read();

public:
	Metadata() = default;
	Metadata(Metadata&&);
	Metadata(const std::string&);

	Metadata& operator=(Metadata&& rhs);

	virtual ~Metadata();

	bool isValid() const;

	std::list< Tag >::const_iterator begin() const noexcept { return tags.cbegin(); };
	std::list< Tag >::const_iterator end()   const noexcept { return tags.cend();   };

	size_t size()  const noexcept { return tags.size();  };
	bool   empty() const noexcept { return tags.empty(); };

	void write(const std::string&) const;
};


#endif
