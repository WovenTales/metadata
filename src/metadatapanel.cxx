#include <metadatapanel.hxx>

// widgets
#include <QMessageBox>


MetadataPanel::MetadataPanel(const QString& path, QWidget* parent, Qt::WindowFlags f) : QScrollArea(parent) {
	data = Metadata(path.toStdString());

	grid   = new QFrame(this, f);
	layout = new QGridLayout;

	// Do want numerical iteration as well, to insert labels into proper row
	int i = 0, skip = 0;
	for (auto e = data.begin(); e != data.end(); ++e, ++i) {
		if (e->type == Chunk::Type::HIDE) {
			++skip;
			continue;
		}

		QLabel* n      = new QLabel(e->label.c_str(), this);
		QLabel* d      = new QLabel(e->data.c_str(), this);
		QPushButton* b = (e->required() ? NULL : new QPushButton("Clear", this));
		labels.push_back(std::make_tuple((i + skip), n, d, b));

		layout->addWidget(n, i, 0);

		d->setTextFormat(Qt::RichText);
		layout->addWidget(d, i, 1);

		if (b != NULL) {
			connect(b, &QPushButton::clicked, [i, this](){ this->clearTag(i); });
			layout->addWidget(b, i, 2);
		}
	}

	grid->setLayout(layout);
	setWidget(grid);
}


bool MetadataPanel::isValid() const {
	return data.isValid();
}


void MetadataPanel::clearTag(unsigned int index) {
	unsigned int dataIndex = std::get<0>(labels[index]);
	QLabel*      title     = std::get<1>(labels[index]);
	QLabel*      contents  = std::get<2>(labels[index]);
	QPushButton* tagButton = std::get<3>(labels[index]);

	if (tagButton == NULL) {
		return;
	}

	if (data.remove(dataIndex) == false) {
		QMessageBox err;
		err.setText("Unable to remove tag.");
		err.exec();

		return;
	}

	title->setText("<s>" + title->text() + "</s>");
	contents->setText("<s>" + contents->text() + "</s>");
	tagButton->setEnabled(false);
}
