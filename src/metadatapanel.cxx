#include <metadatapanel.hxx>


MetadataPanel::MetadataPanel(const QString& path, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f) {
	img = QImage(path);
	keys = img.textKeys();

	layout = new QGridLayout;

	int l = keys.length();
	for (int i = 0; i < l; ++i) {
		QLabel* k = new QLabel(keys[i], this);
		QLabel* t = new QLabel(img.text(keys[i]), this);
		labels.push_back(std::make_pair(k, t));

		layout->addWidget(k, i, 0);
		layout->addWidget(t, i, 1);
	}

	this->setLayout(layout);
}
