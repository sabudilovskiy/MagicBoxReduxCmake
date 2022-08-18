#include <QDebug>
#include <QFile>
#include <magic_enum.hpp>
#include <filesystem>
#include "Downloader.h"
#include "Support.h"
dhash_t dhash64(const std::wstring& str){
    return XXH64(str.c_str(), sizeof(str[0])*str.length(), 0);
}
void Downloader::download() {
    if (!items.empty()){
        items.pop_front(cur);
        QNetworkRequest request(cur.url);
        //request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::RedirectPolicy::NoLessSafeRedirectPolicy);
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
    QVariant code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (code.toInt() == 301 || code.toInt() == 302){
        QVariant redirection_url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        QUrl qurl(redirection_url.toUrl());
        if (qurl.scheme().isEmpty() && qurl.host().isEmpty()){
            QUrl old_qurl = reply->url();
            qurl.setHost(old_qurl.host());
            qurl.setScheme(old_qurl.scheme());
        }
        auto new_reply = network_access_manager->get(QNetworkRequest(qurl));
        QObject::connect(new_reply, &QNetworkReply::downloadProgress, this, &Downloader::downloaded_progress);
        reply->deleteLater();
        return;
    }
    if (reply->error() == dnetwork_error::NoError){
       file.open( QFile::WriteOnly);
       if (!file.error()){
           auto data = reply->readAll();

           qDebug() << data.size() << '\n';
           qDebug() << reply->isFinished() << '\n';
           file.write(data);
           file.close();
       }
    }
    //else qDebug() << magic_enum::enum_name(reply->error()).data();
    emit downloaded_file(cur.hash, reply->error(), file.error());
    emit download();
}

Downloader::Downloader(QObject *parent) :
    QObject(parent)
{
    work.store(false);
    network_access_manager = new QNetworkAccessManager(this);
    QObject::connect(network_access_manager,
                     &QNetworkAccessManager::finished,
                     this,
                     &Downloader::downloaded
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
