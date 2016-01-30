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


class MetadataPanel : public QFrame {
private:
	Metadata    data;

	QGridLayout* layout;

	std::vector< std::pair< QLabel*, QLabel* > > labels;

public:
	MetadataPanel(const QString&, QWidget* = 0, Qt::WindowFlags = 0);

	bool isValid() const;
};


#endif
