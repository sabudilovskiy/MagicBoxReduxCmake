#ifndef SDKSETTINGS_H
#define SDKSETTINGS_H

#include "xmlnode.h"
#include <unordered_map>
#include "MyTab.h"
#include <QMainWindow>
namespace Ui {
class SettingsWindow;
}

class UnknownTypeSettings : public std::exception{

};
class UnknownTab : public std::exception{

};

class SettingsWindow : public QMainWindow
{
    Q_OBJECT
    std::vector<QSpinBox*> _spin_boxes;
    std::vector<QCheckBox*> _check_boxes;
    std::vector<QTextEdit*> _text_edits;
    std::vector<QDoubleSpinBox*> _double_spin_boxes;
    std::vector<QComboBox*> _combo_boxes;
    std::unordered_map<std::wstring, MyTab> _tabs;
    std::wstring _name_fl;
    std::wstring _name_file;
    std::wstring _dir;
    std::vector<QString> _files;
public:
    explicit SettingsWindow(std::wstring dir, std::wstring name_fl, QWidget *parent = nullptr);
    ~SettingsWindow();
    void add_elem(const std::wstring& name, XMLNode& node);
    MyTab* add_tab(std::wstring name, XMLNode& node);
    MyTab* find_tab(const std::wstring& key);
signals:
    void resaved_file();
private slots:
    void on_load_button_clicked();
    void on_save_button_clicked();
private:
    Ui::SettingsWindow *ui;
};

#endif // SDKSETTINGS_H
