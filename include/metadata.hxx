#ifndef METADATA_H
#define METADATA_H


#include <fstream>
#include <string>


class Metadata {
private:
	std::ifstream file;

public:
	Metadata() = default;
	Metadata(Metadata&&);
	Metadata(const std::string&);

	Metadata& operator=(Metadata&& rhs);

	virtual ~Metadata();

	bool isValid() const;
};


#endif
