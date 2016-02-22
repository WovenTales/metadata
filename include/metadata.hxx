#ifndef METADATA_H
#define METADATA_H


#include <chunk.hxx>

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

	        size_t size()                                     const noexcept { return ref.size();  };
	        bool   empty()                                    const noexcept { return ref.empty(); };

		std::list< const Chunk* >::const_iterator begin() const noexcept { return ref.begin(); };
		std::list< const Chunk* >::const_iterator end()   const noexcept { return ref.end();   };
	};

protected:
	std::list< Tag > tags;

public:
	virtual ~Metadata() = default;

	Metadata& operator=(Metadata&& rhs);

	        size_t size()                    const noexcept { return tags.size();  };
	        bool   empty()                   const noexcept { return tags.empty(); };

	virtual void   write(const std::string&) const = 0;
	        bool   remove(unsigned int);

	std::list< Tag >::const_iterator begin() const noexcept { return tags.cbegin(); };
	std::list< Tag >::const_iterator end()   const noexcept { return tags.cend();   };
};


#endif
