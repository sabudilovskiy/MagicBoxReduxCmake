#include <QFile>
#include "Downloader.h"
#include "Support.h"
dhash_t dhash64(const std::wstring& str){
    return XXH64(str.c_str(), sizeof(str[0])*str.length(), 0);
}
void Downloader::download() {
    if (!items.empty()){
        items.pop_front(cur);
        QNetworkRequest request(cur.url);
        emit start_downloading(cur.hash);
        auto reply = network_access_manager->get(request);
        QObject::connect(reply, &QNetworkReply::downloadProgress, this, &Downloader::downloaded_progress);
    }
    else work.store(false);
}
void Downloader::downloaded(QNetworkReply* reply) {
    QObject::disconnect(reply, &QNetworkReply::downloadProgress, this, &Downloader::downloaded_progress);
    QFile file(QString::fromStdWString(cur.path));
    std::wstring folder = get_folder(cur.path);
    if (!std::filesystem::exists(folder)){
        std::filesystem::create_directories(folder);
    }
    if (reply->error() == dnetwork_error::NoError){
       file.open( QFile::WriteOnly);
       if (!file.error()){
           file.write(reply->readAll());
           file.close();
       }
    }
    emit downloaded_file(cur.hash, reply->error(), file.error());
    emit download();
}

Downloader::Downloader(QObject *parent) :
    QObject(parent)
{
    work.store(false);
    network_access_manager = new QNetworkAccessManager(this);
    QObject::connect(network_access_manager,
                     SIGNAL (finished(QNetworkReply*)),
                     this,
                     SLOT (downloaded(QNetworkReply*))
                     );
}

dhash_t Downloader::add_download(const QUrl &url, const std::wstring & path) {
    std::wstring full_name = url.path().toStdWString() + path;
    dhash_t hash_item = dhash64(full_name);
    Item item(url, path, hash_item);
    items.push_back(item);
    if (!work.exchange(true)) emit download();
    return hash_item;
}

//void Downloader::download_bytes(qint64 byteRecd, quint64 bytesTotal) {
//    emit downloaded_progress(byteRecd, bytesTotal);
//}