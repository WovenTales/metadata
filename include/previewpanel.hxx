#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H


#include <string>

// core
#include <Qt>

// gui
#include <QPaintEvent>
#include <QPixmap>


// widgets
#include <QFrame>


namespace metadata {


class PreviewPanel : public QFrame {
private:
	const QPixmap& img;

	virtual void paintEvent(QPaintEvent*) override;

public:
	PreviewPanel(const QPixmap&, QWidget* = 0, Qt::WindowFlags = 0);

	bool isValid() const;
};


}
#endif
