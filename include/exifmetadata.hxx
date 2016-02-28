#ifndef EXIF_METADATA_H
#define EXIF_METADATA_H


#include <metadata.hxx>

#include <fstream>
#include <string>


class ExifMetadata : public Metadata {
	METADATA_CONSTRUCTORS(ExifMetadata);

private:
	bool bigEndian;

protected:
	virtual void read(std::istream&) override;

public:
	virtual void write(const std::string&) override;
};


#endif
