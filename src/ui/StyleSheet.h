#pragma once
#include <string_view>

namespace ncc::ui
{

inline constexpr std::string_view kStyleSheet = R"(
* { font-family: "Segoe UI"; font-size: 10pt; color: #e6e9ef; }
ncc--ui--MainWindow, QMessageBox, QDialog { background: #1b1e26; }
QLabel#title { font-family: "Poetsen One"; font-size: 20pt; color: #ffffff; }
QLabel#subtitle, QLabel#hint { color: #8b93a7; }
QLabel#status { color: #4ade80; }
QLabel#status[error="true"] { color: #f87171; }
QLineEdit, QSpinBox {
    background: #252a35; border: 1px solid #343b4a; border-radius: 6px;
    padding: 7px 9px; selection-background-color: #3b82f6;
}
QComboBox {
    background: #252a35; border: 1px solid #343b4a; border-radius: 6px; padding: 7px 9px;
}
QComboBox:focus, QComboBox:on { border-color: #3b82f6; }
QComboBox::drop-down { border: none; width: 24px; }
QComboBox QAbstractItemView {
    background: #252a35; border: 1px solid #343b4a; selection-background-color: #3b82f6; outline: none;
}
QLineEdit:focus, QSpinBox:focus { border-color: #3b82f6; }
QLineEdit:read-only { color: #b7bfd0; }
QPushButton {
    border: none; border-radius: 6px; padding: 8px 16px; font-weight: 600;
}
QPushButton#secondary { background: #2f3644; }
QPushButton#secondary:hover { background: #3a4253; }
QPushButton#primary { background: #3b82f6; color: white; padding: 11px; font-size: 11pt; }
QPushButton#primary:hover { background: #2f6fdb; }
QPushButton#primary:pressed { background: #275fbd; }
QPushButton#primary:disabled { background: #2a303c; color: #5f6779; }
QMenuBar { background: #161920; border-bottom: 1px solid #2a303c; padding: 2px 8px; }
QMenuBar::item { background: transparent; padding: 5px 10px; border-radius: 4px; }
QMenuBar::item:selected { background: #2f3644; }
QMenu { background: #252a35; border: 1px solid #343b4a; padding: 4px; }
QMenu::item { padding: 6px 24px 6px 14px; border-radius: 4px; }
QMenu::item:selected { background: #3b82f6; color: white; }
QCheckBox::indicator {
    width: 16px; height: 16px; border-radius: 4px; border: 1px solid #343b4a; background: #252a35;
}
QCheckBox::indicator:checked { background: #3b82f6; border-color: #3b82f6; }
QToolTip { background: #252a35; border: 1px solid #343b4a; color: #e6e9ef; }
)";

}
