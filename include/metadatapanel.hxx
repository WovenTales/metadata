#ifndef METADATAPANEL_H
#define METADATAPANEL_H


#include <utility>
#include <vector>

// core
#include <Qt>
#include <QString>
#include <QStringList>

// gui
#include <QImage>

// widgets
#include <QFrame>
#include <QGridLayout>
#include <QLabel>


class MetadataPanel : public QFrame {
private:
	QImage      img;
	QStringList keys;

	QGridLayout* layout;

	std::vector< std::pair< QLabel*, QLabel* > > labels;

public:
	MetadataPanel(const QString&, QWidget* = 0, Qt::WindowFlags = 0);

	bool isValid() const;
};


#endif
