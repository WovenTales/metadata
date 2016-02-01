#ifndef METADATA_H
#define METADATA_H


#include <chunk.hxx>

#include <fstream>
#include <list>
#include <string>


class Metadata {
private:
	std::ifstream      file;
	std::list< Chunk > chunks;

	bool read();

public:
	Metadata() = default;
	Metadata(Metadata&&);
	Metadata(const std::string&);

	Metadata& operator=(Metadata&& rhs);

	virtual ~Metadata();

	bool isValid() const;

	std::list< Chunk >::const_iterator  begin() const noexcept { return chunks.cbegin(); };
	std::list< Chunk >::const_iterator  end()   const noexcept { return chunks.cend();   };

	size_t size()  const noexcept { return chunks.size();  };
	bool   empty() const noexcept { return chunks.empty(); };
};


#endif
