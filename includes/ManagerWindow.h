#ifndef MANAGERWINDOW_H
#define MANAGERWINDOW_H

#include <QWidget>
#include "ModDownloader.h"
#include "AppSettings.h"
namespace Ui {
class ManagerWindow;
}
enum InstalledPolicy{
    SKIP, REINSTALL, ASK
};
class ManagerWindow : public QWidget
{
protected:
    Q_OBJECT
    const AppSettings& _app_settings;
    ModDownloader& _mod_downloader;
    std::vector<ModVersion> _mods_db, _mods_sdk;
    Ui::ManagerWindow *ui;
    int _downloaded = 0, _downloading = 0;
    InstalledPolicy _installed_policy;
public:
    explicit ManagerWindow(const AppSettings& app_settings, ModDownloader &mod_downloader, QWidget *parent = nullptr);
    ~ManagerWindow();
protected:
    void add_downloading();
    void add_downloaded();
    void update_mod_progress_bar();
protected slots:
    int get_index(DOWNLOAD_INFO mod) const;
    void update_settings();
    void update_mod();
    void on_pushButton_download_clicked();
    void update_bytes_progress_bar(qint64 downloaded_bytes, qint64 total_bytes);
    void downloading_mod(DOWNLOAD_INFO mod);
    void downloaded_mod(DOWNLOAD_INFO mod, dnetwork_error network_err, dfile_error file_err);
};

#endif // MANAGERWINDOW_H
