#pragma once
#include "core/ClientBuilder.h"
#include <QWidget>

class QSettings;
class QLineEdit;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QLabel;
class QPushButton;

namespace ncc::ui
{

class MainWindow final : public QWidget
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;

private:
	QSettings* settings = nullptr;
	QLineEdit* in_folder = nullptr;
	QLabel* lab_version = nullptr;
	QLineEdit* in_name = nullptr;
	QLineEdit* in_ip = nullptr;
	QSpinBox* in_port = nullptr;
	QComboBox* in_lang = nullptr;
	QCheckBox* cb_shortcut = nullptr;
	QPushButton* btn_create = nullptr;
	QLabel* lab_status = nullptr;

	void browseFolder();
	void createClient();
	void validate();
	void showAbout();

	void setFolder(const QString& folder);
	void setStatus(const QString& text, bool error);
	[[nodiscard]] QString clientName() const;
	[[nodiscard]] QString describe(BuildError error, const QString& name) const;
};

}
