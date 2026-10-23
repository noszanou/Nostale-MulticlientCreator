#include "ui/MainWindow.h"
#include "ui/StyleSheet.h"
#include <QApplication>
#include <QFontDatabase>
#include <QStyleHints>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	app.styleHints()->setColorScheme(Qt::ColorScheme::Dark);
	app.setStyle("Fusion");
	QFontDatabase::addApplicationFont(":/main/font/PoetsenOne-Regular.ttf");
	app.setStyleSheet(QString::fromUtf8(ncc::ui::kStyleSheet.data(), static_cast<qsizetype>(ncc::ui::kStyleSheet.size())));

	ncc::ui::MainWindow window;
	window.show();
	return app.exec();
}
