#include "ui/MainWindow.h"
#include "platform/FileVersion.h"
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QSettings>
#include <QSpinBox>
#include <QStandardPaths>
#include <QStyle>
#include <QVBoxLayout>
#include <filesystem>
#include <utility>

namespace ncc::ui
{

namespace
{

namespace key
{
constexpr QLatin1StringView Path{ "Path" };
constexpr QLatin1StringView Name{ "Name" };
constexpr QLatin1StringView Ip{ "IP" };
constexpr QLatin1StringView Port{ "Port" };
constexpr QLatin1StringView Language{ "Language" };
constexpr QLatin1StringView Shortcut{ "Shortcut" };
}

QString toQString(std::string_view text)
{
	return QString::fromUtf8(text.data(), static_cast<qsizetype>(text.size()));
}

std::filesystem::path toPath(const QString& text)
{
	return text.toStdU16String();
}

const QString kSourceExe = toQString(kSourceClient);

const QRegularExpression ipRegex(
	"^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9]?[0-9])\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9]?[0-9])$");
const QRegularExpression nameRegex("^[^\\\\/:*?\"<>|]+$");

QString desktopPath()
{
	return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

}

MainWindow::MainWindow(QWidget* parent)
	: QWidget(parent)
{
	setWindowTitle("NosTale Client Creator");
	setWindowIcon(QIcon(":/main/img/logo.ico"));

	settings = new QSettings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat, this);

	auto* title = new QLabel("NosTale Client Creator");
	title->setObjectName("title");
	auto* subtitle = new QLabel(tr("Create a NostaleClientX copy pointing to your own server"));
	subtitle->setObjectName("subtitle");
	auto* titles = new QVBoxLayout;
	titles->setSpacing(2);
	titles->addWidget(title);
	titles->addWidget(subtitle);

	in_folder = new QLineEdit;
	in_folder->setReadOnly(true);
	in_folder->setPlaceholderText(tr("Select your NosTale folder"));
	auto* btnBrowse = new QPushButton(tr("Browse..."));
	btnBrowse->setObjectName("secondary");
	btnBrowse->setCursor(Qt::PointingHandCursor);
	connect(btnBrowse, &QPushButton::clicked, this, &MainWindow::browseFolder);
	auto* folderRow = new QHBoxLayout;
	folderRow->addWidget(in_folder, 1);
	folderRow->addWidget(btnBrowse);
	lab_version = new QLabel;
	lab_version->setObjectName("hint");

	in_name = new QLineEdit(settings->value(key::Name, "NostaleClientPS").toString());
	in_name->setPlaceholderText("NostaleClientPS");
	in_name->setValidator(new QRegularExpressionValidator(nameRegex, this));
	auto* exeSuffix = new QLabel(".exe");
	exeSuffix->setObjectName("hint");
	auto* nameRow = new QHBoxLayout;
	nameRow->addWidget(in_name, 1);
	nameRow->addWidget(exeSuffix);

	in_ip = new QLineEdit(settings->value(key::Ip, "127.0.0.1").toString());
	in_ip->setPlaceholderText("127.0.0.1");
	in_ip->setValidator(new QRegularExpressionValidator(ipRegex, this));
	in_port = new QSpinBox;
	in_port->setRange(1, 65535);
	in_port->setValue(settings->value(key::Port, 4000).toInt());
	in_port->setButtonSymbols(QAbstractSpinBox::NoButtons);
	in_port->setFixedWidth(90);
	auto* serverRow = new QHBoxLayout;
	serverRow->addWidget(in_ip, 1);
	serverRow->addWidget(new QLabel(":"));
	serverRow->addWidget(in_port);

	in_lang = new QComboBox;
	for (const std::string_view language : kLanguages)
		in_lang->addItem(toQString(language));
	in_lang->setCurrentIndex(std::clamp(settings->value(key::Language, 0).toInt(), 0, static_cast<int>(kLanguages.size()) - 1));

	cb_shortcut = new QCheckBox(tr("Create a desktop shortcut"));
	cb_shortcut->setChecked(settings->value(key::Shortcut, true).toBool());
	connect(cb_shortcut, &QCheckBox::toggled, this, [this](bool checked) { settings->setValue(key::Shortcut, checked); });
	settings->setValue(key::Shortcut, cb_shortcut->isChecked());

	auto* form = new QFormLayout;
	form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
	form->setHorizontalSpacing(12);
	form->setVerticalSpacing(10);
	form->addRow(tr("NosTale folder"), folderRow);
	form->addRow("", lab_version);
	form->addRow(tr("Client name"), nameRow);
	form->addRow(tr("Server"), serverRow);
	form->addRow(tr("Language"), in_lang);
	form->addRow("", cb_shortcut);

	btn_create = new QPushButton(tr("Create client"));
	btn_create->setObjectName("primary");
	btn_create->setCursor(Qt::PointingHandCursor);
	connect(btn_create, &QPushButton::clicked, this, &MainWindow::createClient);

	lab_status = new QLabel;
	lab_status->setObjectName("status");
	lab_status->setWordWrap(true);

	auto* menuBar = new QMenuBar(this);
	QMenu* helpMenu = menuBar->addMenu(tr("&Help"));
	helpMenu->addAction(tr("&About"), this, &MainWindow::showAbout);

	auto* root = new QVBoxLayout(this);
	root->setMenuBar(menuBar);
	root->setContentsMargins(24, 16, 24, 20);
	root->setSpacing(16);
	root->addLayout(titles);
	root->addLayout(form);
	root->addWidget(btn_create);
	root->addWidget(lab_status);

	connect(in_name, &QLineEdit::textChanged, this, &MainWindow::validate);
	connect(in_ip, &QLineEdit::textChanged, this, &MainWindow::validate);

	setFolder(settings->value(key::Path).toString());
	setFixedWidth(520);
	setFixedHeight(sizeHint().height());
}

MainWindow::~MainWindow()
{
	settings->sync();
}

void MainWindow::setFolder(const QString& folder)
{
	const QString source = folder + "/" + kSourceExe;
	if (!folder.isEmpty() && QFileInfo::exists(source))
	{
		const auto version = platform::fileVersion(toPath(source)).value_or("1.0.0.0");
		in_folder->setText(QDir::toNativeSeparators(folder));
		lab_version->setText(tr("%1 version %2").arg(kSourceExe, toQString(version)));
		settings->setValue(key::Path, folder);
	}
	else
	{
		in_folder->clear();
		lab_version->setText(tr("%1 not found").arg(kSourceExe));
	}
	validate();
}

void MainWindow::browseFolder()
{
	QString start = in_folder->text();
	if (start.isEmpty())
	{
		const QString userLink = desktopPath() + "/NosTale.lnk";
		const QString link = QFile::exists(userLink) ? userLink : "C:/Users/Public/Desktop/NosTale.lnk";
		start = QFileInfo(QFile::symLinkTarget(link)).absolutePath();
	}
	const QString exe = QFileDialog::getOpenFileName(this, tr("Select %1").arg(kSourceExe), start, kSourceExe + " (" + kSourceExe + ")");
	if (!exe.isEmpty())
		setFolder(QFileInfo(exe).absolutePath());
}

QString MainWindow::clientName() const
{
	QString name = in_name->text().trimmed();
	if (name.endsWith(".exe", Qt::CaseInsensitive))
		name.chop(4);
	return name;
}

void MainWindow::validate()
{
	const QString name = clientName();
	const bool ok = !in_folder->text().isEmpty()
		&& !name.isEmpty() && name.compare("NostaleClientX", Qt::CaseInsensitive) != 0
		&& ipRegex.match(in_ip->text()).hasMatch();
	btn_create->setEnabled(ok);
	if (!ok)
		lab_status->clear();
}

void MainWindow::setStatus(const QString& text, bool error)
{
	lab_status->setProperty("error", error);
	lab_status->style()->unpolish(lab_status);
	lab_status->style()->polish(lab_status);
	lab_status->setText(text);
}

QString MainWindow::describe(BuildError error, const QString& name) const
{
	switch (error)
	{
		using enum BuildError;
	case InvalidIp:
	case IpNotFound:
		return tr("IP not found in the client (unsupported version?) or longer than %1 characters").arg(kMaxIpLength);
	case InvalidPort:
	case PortTableNotFound:
		return tr("Port table not found in the client (unsupported version?)");
	case InvalidLanguage:
	case LanguageNotFound:
	case LauncherCheckNotFound:
		return tr("Could not create %1.exe (client patch not found)").arg(name);
	case SourceUnreadable:
		return tr("Could not open %1").arg(kSourceExe);
	case WriteFailed:
		return tr("Could not write %1.exe (file in use?)").arg(name);
	}
	std::unreachable();
}

void MainWindow::createClient()
{
	const QString folder = QDir::fromNativeSeparators(in_folder->text());
	const QString name = clientName();
	const ClientSpec spec{
		.folder = toPath(folder),
		.name = toPath(name),
		.ip = in_ip->text().toStdString(),
		.port = static_cast<std::uint16_t>(in_port->value()),
		.language = static_cast<std::uint8_t>(in_lang->currentIndex()),
	};
	const QString target = folder + "/" + name + ".exe";

	if (QFileInfo::exists(target)
		&& QMessageBox::question(this, windowTitle(), tr("%1.exe already exists, replace it?").arg(name)) != QMessageBox::Yes)
		return;

	if (const auto result = buildClient(spec); !result)
	{
		setStatus(describe(result.error(), name), true);
		return;
	}

	settings->setValue(key::Name, name);
	settings->setValue(key::Ip, in_ip->text());
	settings->setValue(key::Port, in_port->value());
	settings->setValue(key::Language, in_lang->currentIndex());

	QString msg = tr("%1.exe created → %2:%3").arg(name, in_ip->text()).arg(in_port->value());
	if (cb_shortcut->isChecked())
	{
		const QString shortcut = desktopPath() + "/" + name + ".lnk";
		QFile::remove(shortcut);
		if (QFile::link(target, shortcut))
			msg += "\n" + tr("Shortcut added on the desktop");
	}
	setStatus(msg, false);
}

void MainWindow::showAbout()
{
	QMessageBox box(this);
	box.setWindowTitle(tr("About"));
	box.setIconPixmap(QIcon(":/main/img/logo.ico").pixmap(64, 64));
	box.setTextFormat(Qt::RichText);
	box.setTextInteractionFlags(Qt::TextBrowserInteraction);
	box.setText("<h3>NosTale Client Creator</h3>"
		"<p>Created by <a style=\"color:#60a5fa\" href=\"https://www.elitepvpers.com/forum/members/3141926-pumba98.html\">Pumba98</a><br>"
		"Maintained by <a style=\"color:#60a5fa\" href=\"https://www.elitepvpers.com/forum/members/7384346-zanouu.html\">Zanou</a></p>");
	box.exec();
}

}
