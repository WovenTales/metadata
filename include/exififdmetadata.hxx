#ifndef EXIF_IFD_METADATA_H
#define EXIF_IFD_METADATA_H


#include <metadata.hxx>

#include <fstream>
#include <string>


class ExifIFDMetadata : public Metadata {
	METADATA_CONSTRUCTORS(ExifIFDMetadata);

private:
	bool bigEndian;
	unsigned int exifCount;

protected:
	virtual void read(std::istream&) override;

public:
	ExifIFDMetadata(std::istream&, bool);

	virtual void write(const std::string&) override;
};


#endif
