#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H


#include <metadatapanel.hxx>
#include <previewpanel.hxx>

#include <string>

// core
#include <Qt>
#include <QString>

// gui
#include <QPixmap>

// widgets
#include <QFrame>
#include <QHBoxLayout>
#include <QWidget>


namespace metadata {


class ImagePanel : public QFrame {
private:
	QPixmap img;

	PreviewPanel*  pPanel = NULL;
	MetadataPanel* mPanel = NULL;

	QHBoxLayout*   layout = NULL;

public:
	ImagePanel(const QString& = "", QWidget* = 0, Qt::WindowFlags = 0);

	void write(const std::string& path) { mPanel->write(path); };
};


}
#endif
