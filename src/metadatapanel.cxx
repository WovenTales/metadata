#include <metadatapanel.hxx>


MetadataPanel::MetadataPanel(const QString& path, QWidget* parent, Qt::WindowFlags f) : QScrollArea(parent) {
	data = Metadata(path.toStdString());

	grid   = new QFrame(this, f);
	layout = new QGridLayout;

	// Do want numerical iteration, to keep labels in same order as in file
	int s = data.size();
	for (int i = 0; i < s; ++i) {
		QLabel* n = new QLabel(data[i].name().c_str(), this);
		QLabel* d = new QLabel(data[i].data().c_str(), this);
		labels.push_back(std::make_pair(n, d));

		d->setTextFormat(Qt::RichText);

		layout->addWidget(n, i, 0);
		layout->addWidget(d, i, 1);
	}

	grid->setLayout(layout);
	setWidget(grid);
}


bool MetadataPanel::isValid() const {
	return data.isValid();
}
