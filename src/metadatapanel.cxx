#include <metadatapanel.hxx>

// widgets
#include <QMessageBox>


MetadataPanel::MetadataPanel(const QString& path, QWidget* parent, Qt::WindowFlags f) : QScrollArea(parent) {
	data = Metadata(path.toStdString());

	grid   = new QFrame(this, f);
	layout = new QGridLayout;

	// Do want numerical iteration, to keep labels in same order as in file
	int s = data.size();
	auto e = data.begin();
	for (int i = 0; i < s; ++i, ++e) {
		QLabel* n      = new QLabel(e->label.c_str(), this);
		QLabel* d      = new QLabel(e->data.c_str(), this);
		QPushButton* b = (e->required() ? NULL : new QPushButton("Clear", this));
		labels.push_back(std::make_tuple(n, d, b));

		layout->addWidget(n, i, 0);

		d->setTextFormat(Qt::RichText);
		layout->addWidget(d, i, 1);

		if (b != NULL) {
			connect(b, &QPushButton::clicked, [i, this](){
					unsigned int offset = 0;
					auto l = this->labels.begin();
					for (int j = 0; j < i; ++j, ++l) {
						QPushButton* b = std::get<2>(this->labels[j]);
						if ((b != NULL) && (b->isEnabled() == false)) {
							++offset;
						}
					}
					this->clearTag(i, offset); });
			layout->addWidget(b, i, 2);
		}
	}

	grid->setLayout(layout);
	setWidget(grid);
}


bool MetadataPanel::isValid() const {
	return data.isValid();
}


void MetadataPanel::clearTag(unsigned int index, unsigned int offset) {
	QLabel*      title     = std::get<0>(labels[index]);
	QLabel*      contents  = std::get<1>(labels[index]);
	QPushButton* tagButton = std::get<2>(labels[index]);

	if (tagButton == NULL) {
		return;
	}

	if (data.remove(index - offset) == false) {
		QMessageBox err;
		err.setText("Unable to remove tag.");
		err.exec();

		return;
	}

	title->setText("<s>" + title->text() + "</s>");
	contents->setText("<s>" + contents->text() + "</s>");
	tagButton->setEnabled(false);
}
