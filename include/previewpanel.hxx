#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H


#include <picturelabel.hxx>

#include <string>

// core
#include <Qt>

// gui
#include <QResizeEvent>

// widgets
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>


class PreviewPanel : public QFrame {
private:
	PictureLabel* label;
	QVBoxLayout*  layout;
	QHBoxLayout*  cross;

public:
	PreviewPanel(const QPixmap&, QWidget* = 0, Qt::WindowFlags = 0);

	virtual void resizeEvent(QResizeEvent*);

	bool isValid() const;
};


#endif
