#ifndef METADATA_H
#define METADATA_H


#include <chunk.hxx>

#include <fstream>
#include <list>
#include <string>


class Metadata {
public:
	struct Tag {
	private:
		std::list< const Chunk* > ref;

		friend Metadata;

	public:
		Chunk::Type type;
		std::string label;
		std::string data;

		Tag(const Chunk*);
		virtual ~Tag();

		void addChunk(const Chunk*);

		bool required()             const;
	};

private:
	std::ifstream    file;
	std::list< Tag > tags;

	bool read();

public:
	Metadata() = default;
	Metadata(Metadata&&);
	Metadata(const std::string&);

	Metadata& operator=(Metadata&& rhs);

	virtual ~Metadata();

	bool isValid()                           const;

	size_t size()                            const noexcept { return tags.size();  };
	bool   empty()                           const noexcept { return tags.empty(); };

	void   write(const std::string&)         const;
	bool   remove(unsigned int);

	std::list< Tag >::const_iterator begin() const noexcept { return tags.cbegin(); };
	std::list< Tag >::const_iterator end()   const noexcept { return tags.cend();   };
};


#endif
