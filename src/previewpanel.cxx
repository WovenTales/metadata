#include <previewpanel.hxx>

// core
#include <QSize>


PreviewPanel::PreviewPanel(const QPixmap& i, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f), img(i) {
	label  = new QLabel(this);
	layout = new QVBoxLayout();

	label->setScaledContents(true);

	layout->addWidget(label, Qt::AlignHCenter);
	this->setLayout(layout);

	// Setting label picture occurs in resizeEvent()
}


void PreviewPanel::resizeEvent(QResizeEvent* e) {
	QWidget::resizeEvent(e);

	QSize s = e->size();

	int l, t, r, b;
	layout->getContentsMargins(&l, &t, &r, &b);

	int h, w;
	w = (s.width()  - l - r);
	h = (s.height() - t - b - layout->spacing());  // Subtract any future widgets

	// FIXME: Aspect ratio not maintained after encapsulation into layout
	map = img.scaled(w, h, Qt::KeepAspectRatio);

	label->setPixmap(map);
	label->adjustSize();
}
