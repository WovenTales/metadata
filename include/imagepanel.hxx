#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H


#include <metadatapanel.hxx>
#include <previewpanel.hxx>

// core
#include <Qt>
#include <QString>

// gui
#include <QPixmap>

// widgets
#include <QFrame>
#include <QHBoxLayout>
#include <QWidget>


class ImagePanel : public QFrame {
private:
	QPixmap  img;

	PreviewPanel*  pPanel;
	MetadataPanel* mPanel;

	QHBoxLayout* layout;

public:
	ImagePanel(const QString&, QWidget* = 0, Qt::WindowFlags = 0);
};


#endif
