#include <previewpanel.hxx>

// core
#include <QSize>


PreviewPanel::PreviewPanel(const QPixmap& i, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f) {
	label  = new PictureLabel(i, this);
	layout = new QVBoxLayout();
	cross  = new QHBoxLayout();

	cross->addStretch();
	cross->addWidget(label, Qt::AlignHCenter);
	cross->addStretch();

	layout->addStretch();
	layout->addLayout(cross);
	layout->addStretch();
	setLayout(layout);
}


// TODO: Rather hacky way of doing it, but aspect ratio doesn't work in a layout
void PreviewPanel::resizeEvent(QResizeEvent* e) {
	QFrame::resizeEvent(e);

	QSize s = e->size();

	int l, t, r, b;
	layout->getContentsMargins(&l, &t, &r, &b);

	int h, w;
	w = (s.width()  - l - r);
	h = (s.height() - t - b - layout->spacing());  // Subtract any future widgets

	int hAspect = label->heightForWidth(w);
	// The available height is greater than that from stretching to max width
	if (h >= hAspect) {
		h = hAspect;
	} else {
		w = label->widthForHeight(h);
	}

	label->setFixedSize(w, h);
}



bool PreviewPanel::isValid() const {
	return label->isValid();
}
