#include <imagepanel.hxx>


ImagePanel::ImagePanel(const QString& path, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f) {
	img = QPixmap(path);

	view = new PreviewPanel(img, this);
	data = new MetadataPanel(path, this);
	layout = new QHBoxLayout();

	layout->addWidget(view);
	layout->addWidget(data);
	this->setLayout(layout);
}
