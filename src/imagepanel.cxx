#include <imagepanel.hxx>

#include <cstddef>

// core
#include <Qt>
#include <QSize>

// gui
#include <QResizeEvent>

// widgets
#include <QWidget>
#include <QVBoxLayout>


ImagePanel::ImagePanel(std::string path) {
	img = QPixmap(path.c_str());

	label  = new QLabel(this);
	file   = new QLabel(this);
	layout = new QVBoxLayout();

	label->setAlignment(Qt::AlignHCenter);
	label->setScaledContents(true);

	file->setAlignment(Qt::AlignHCenter);
	file->setText(path.c_str());

	layout->addWidget(label);
	layout->addWidget(file);
	this->setLayout(layout);

	// Setting label picture occurs in resizeEvent()
}


void ImagePanel::resizeEvent(QResizeEvent* e) {
	QWidget::resizeEvent(e);

	QSize s = e->size();

	int l, t, r, b;
	layout->getContentsMargins(&l, &t, &r, &b);

	int h, w;
	w = (s.width()  - l - r);
	h = (s.height() - t - b - layout->spacing() - file->size().height());

	// FIXME: Aspect ratio not maintained after encapsulation into layout
	map = img.scaled(w, h, Qt::KeepAspectRatio);

	label->setPixmap(map);
	label->adjustSize();
}
