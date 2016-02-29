#include <imagepanel.hxx>

#include <metadatafiletype.hxx>

#include <fstream>

// widgets
#include <QMessageBox>


ImagePanel::ImagePanel(const QString& path, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f) {
	// Don't show another dialog if "cannot be opened" was already displayed
	bool trigger = false;

	if (path.isEmpty()) {
		img = QPixmap();
	} else {
		std::ifstream file(path.toStdString());
		if (file.fail()) {
			QMessageBox err;
			err.setText("The image '" + path + "' cannot be opened.");
			err.exec();

			img = QPixmap();
			trigger = true;
		} else {
			img = QPixmap(path);
		}
		file.close();
	}

	if ((trigger == true) || (detectFileType(path.toStdString()) == MetadataFileType::INVALID)) {
		return;
	}

	mPanel = new MetadataPanel(path, this);
	if (mPanel->isValid()) {
		pPanel = new PreviewPanel(img, this);
	} else {
		QMessageBox err;
		err.setText("The image '" + path + "' is not in a supported format.");
		err.exec();

		delete mPanel;
		mPanel = NULL;

		return;
	}

	layout = new QHBoxLayout();

	layout->addWidget(pPanel);
	layout->addWidget(mPanel);
	layout->setStretch(0, 1);
	layout->setStretch(1, 0);

	// TODO Doesn't actually work
	//pPanel->resize((width() - mPanel->width() - layout->spacing()), pPanel->height());
	//pPanel->setMaximumWidth(width() - mPanel->width() - layout->spacing());

	setLayout(layout);
}
