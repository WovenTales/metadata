#include <metadatapanel.hxx>


MetadataPanel::MetadataPanel(const QString& path, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f) {
	data = Metadata(path.toStdString());

	layout = new QGridLayout;

	// Do want numerical iteration, to keep labels in same order as in file
	int s = data.size();
	for (int i = 0; i < s; ++i) {
		QLabel* t = new QLabel(data[i].type().c_str(), this);
		QLabel* d = new QLabel(data[i].data().c_str(), this);
		labels.push_back(std::make_pair(t, d));

		layout->addWidget(t, i, 0);
		layout->addWidget(d, i, 1);
	}

	setLayout(layout);
}


bool MetadataPanel::isValid() const {
	return data.isValid();
}
