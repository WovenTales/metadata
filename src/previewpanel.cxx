#include <previewpanel.hxx>

// core
#include <QSize>

// gui
#include <QPainter>


namespace metadata {


PreviewPanel::PreviewPanel(const QPixmap& i, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f), img(i) { }


void PreviewPanel::paintEvent(QPaintEvent* e) {
	QFrame::paintEvent(e);

	QPainter p(this);
	QPixmap scaled = img.scaled(contentsRect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	p.drawPixmap(((width() - scaled.width()) / 2), ((height() - scaled.height()) / 2), scaled);
}


bool PreviewPanel::isValid() const {
	return !img.isNull();
}


}
