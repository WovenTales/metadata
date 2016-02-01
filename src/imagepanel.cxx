#include <imagepanel.hxx>

#include <fstream>

// widgets
#include <QMessageBox>


ImagePanel::ImagePanel(const QString& path, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f) {
	// Need workaround to separate unopenable file from bad formats
	std::ifstream file(path.toStdString());
	if (!file.good()) {
		QMessageBox err;
		err.setText("The image '" + path + "' cannot be opened.");
		err.exec();

		img = QPixmap();
	} else {
		img = QPixmap(path);
	}
	file.close();

	pPanel = new PreviewPanel(img, this);
	mPanel = new MetadataPanel(path, this);

	// Don't show another dialog if the image is null
	if ((!pPanel->isValid() || !mPanel->isValid()) && !img.isNull()) {
		QMessageBox err;
		err.setText("The image '" + path + "' is not in a supported format.");
		err.exec();

		// TODO: Close window
	}

	layout = new QHBoxLayout();

	layout->addWidget(pPanel);
	layout->addWidget(mPanel);
	setLayout(layout);
}
