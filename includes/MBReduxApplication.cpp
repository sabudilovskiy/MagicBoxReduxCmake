//
// Created by MRV on 8/18/2022.
//

#include "MBReduxApplication.h"

MBReduxApplication::MBReduxApplication(int& argc, char** argv) : QApplication(argc, argv),_main_window(_app_settings, _downloader){
    _main_window.show();
    QObject::connect(&_main_window, &MainWindow::exit_app, this, &MBReduxApplication::exitApp);
}

void MBReduxApplication::exitApp() {
    exit();
}
