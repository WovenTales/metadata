#ifndef METADATA_H
#define METADATA_H


#include <chunk.hxx>

#include <fstream>
#include <string>
#include <vector>


class Metadata {
private:
	std::ifstream        file;
	std::vector< Chunk > chunks;

	bool read();

public:
	Metadata() = default;
	Metadata(Metadata&&);
	Metadata(const std::string&);

	Metadata& operator=(Metadata&& rhs);

	virtual ~Metadata();

	bool isValid() const;

	std::vector< Chunk >::const_iterator  begin()              const noexcept { return chunks.cbegin(); };
	std::vector< Chunk >::const_iterator  end()                const noexcept { return chunks.cend();   };
	std::vector< Chunk >::const_reference operator[](size_t n) const          { return chunks[n];       };
	std::vector< Chunk >::const_reference at(size_t n)         const          { return chunks.at(n);    };
	const Chunk*                          data()               const noexcept { return chunks.data();   };

	size_t size()  const noexcept { return chunks.size();  };
	bool   empty() const noexcept { return chunks.empty(); };
};


#endif
