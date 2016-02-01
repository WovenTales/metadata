#ifndef METADATAPANEL_H
#define METADATAPANEL_H


#include <metadata.hxx>

#include <utility>
#include <vector>

// core
#include <Qt>
#include <QString>

// widgets
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>


class MetadataPanel : public QScrollArea {
private:
	Metadata     data;

	QFrame*      grid;
	QGridLayout* layout;

	std::vector< std::pair< QLabel*, QLabel* > > labels;

public:
	MetadataPanel(const QString&, QWidget* = 0, Qt::WindowFlags = 0);

	bool isValid() const;
};


#endif
