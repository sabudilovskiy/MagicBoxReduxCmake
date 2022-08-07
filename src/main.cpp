#include "SettingsWindow.h"
#include "xml.h"
#include <QApplication>
#include <AppSettings.h>
#include "Downloader.h"
#include "Support.h"
#include "ModDownloader.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Downloader downloader;
    AppSettings app_settings;
    return a.exec();
}
