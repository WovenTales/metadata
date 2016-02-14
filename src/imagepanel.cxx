#include <imagepanel.hxx>

#include <fstream>

// core
#include <QFileDialog>

// widgets
#include <QMessageBox>


ImagePanel::ImagePanel(const QString& path, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f) {
	// TODO: Use null panel rather than requiring some image
	QString usePath = (path.isEmpty() ? QFileDialog::getOpenFileName(0, "", "", "Images (*.png)") : path);

	std::ifstream file(usePath.toStdString());
	if (!file.good()) {
		QMessageBox err;
		err.setText("The image '" + usePath + "' cannot be opened.");
		err.exec();

		img = QPixmap();
	} else {
		img = QPixmap(usePath);
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
