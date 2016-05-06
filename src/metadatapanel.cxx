#include <metadatapanel.hxx>

#include <metadatafactory.hxx>

// core
#include <Qt>
#include <QMargins>

// widgets
#include <QMessageBox>
#include <QStyle>


namespace metadata {


MetadataPanel::MetadataPanel(const QString& path, QWidget* parent, Qt::WindowFlags f) : QScrollArea(parent) {
	grid   = new QFrame(this, f);
	layout = new QGridLayout;

	try {
		data = MetadataFactory::generate(path.toStdString());
	} catch (char e) {
		data = NULL;
	}

	if (data != NULL) {
		// Do want numerical iteration as well, to insert labels into proper row
		int i = 0, skip = 0;
		for (auto e = data->begin(); e != data->end(); ++e, ++i) {
			if (e->type == Chunk::Type::HIDE) {
				++skip;
				continue;
			}

			QLabel*      n = new QLabel(e->label.c_str(), this);
			QLabel*      d;
			QPushButton* b = (e->required() ? NULL : new QPushButton("Clear", this));

			if (e->type == Chunk::Type::NONE) {
				d = NULL;
			} else {
				d = new QLabel(e->data.c_str(), this);
			}

			labels.push_back(std::make_tuple((i + skip), n, d, b));

			layout->addWidget(n, i, 0);

			// TODO QScrollArea height minimum larger than few-line content's
			if (d != NULL) {
				d->setWordWrap(true);
				d->setTextFormat(Qt::RichText);
				d->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
				d->setFrameStyle(QFrame::Panel | QFrame::Plain);
				d->setStyleSheet("border: 1px solid dimgray");

				layout->addWidget(d, i, 1);
			}

			if (b != NULL) {
				connect(b, &QPushButton::clicked, [i, this](){ this->clearTag(i); });
				layout->addWidget(b, i, 2);
			}
		}
	}

	grid->setLayout(layout);
	setWidget(grid);

	// Ensure there is enough space for the vertical scroll bar without covering content
	//     verticalScrollBar() isn't yet initialized (says width == 100)
	QMargins m = contentsMargins();
	setFixedWidth(grid->sizeHint().width() + style()->pixelMetric(QStyle::PM_ScrollBarExtent) + m.left() + m.right());
}


MetadataPanel::~MetadataPanel() {
	if (data != NULL) {
		delete data;
	}
}


bool MetadataPanel::isValid() const {
	return (data != NULL);
}


void MetadataPanel::clearTag(unsigned int index) {
	if (index > labels.size()) {
		QMessageBox err;
		err.setText("Removal index out of bounds.");
		err.exec();

		return;
	}

	auto l = labels[index];
	unsigned int dataIndex = std::get<0>(l);
	QLabel*      title     = std::get<1>(l);
	QLabel*      contents  = std::get<2>(l);
	QPushButton* tagButton = std::get<3>(l);

	if (tagButton == NULL) {
		// Specified tag is marked as required
		return;
	}

	unsigned int offset = 0;
	for (auto i = labels.begin(); *i != l; ++i) {
		if (std::get<1>(*i)->text().startsWith("<s>")) {
			++offset;
		}
	}

	if (data->remove(dataIndex - offset) == false) {
		QMessageBox err;
		err.setText("Unable to remove tag.");
		err.exec();

		return;
	}

	title->setText("<s>" + title->text() + "</s>");
	if (contents != NULL) {
		contents->setText("<s>" + contents->text() + "</s>");
	}
	tagButton->setEnabled(false);
}


}
