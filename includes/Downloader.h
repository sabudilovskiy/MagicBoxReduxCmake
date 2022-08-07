//
// Created by MRV on 8/5/2022.
//

#ifndef MAGICBOXREDUXCMAKE_DOWNLOADER_H
#define MAGICBOXREDUXCMAKE_DOWNLOADER_H
#include <QFile>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <cds/container/fcdeque.h>
#include <atomic>
#include <xxhash.h>
using dhash_t = XXH64_hash_t;
using dnetwork_error = QNetworkReply::NetworkError;
using dfile_error = QFile::FileError;
class Downloader : public QObject{
    Q_OBJECT
    QNetworkAccessManager* network_access_manager;
    struct Item{
        QUrl url;
        std::wstring path;
        dhash_t hash;

        Item(const QUrl &url, const std::wstring &path, unsigned long long int hash) : url(url), path(path),
            hash(hash) {}
        Item(Item&& other): url(std::move(other.url)), path(std::move(other.path)), hash(other.hash){

        }
        Item(const Item& other): url(other.url), path(other.path), hash(other.hash){

        }
        Item& operator=(const Item& other){
            url = other.url;
            path = other.path;
            hash = other.hash;
            return *this;
        }
        Item& operator=(Item&& other){
            std::swap(other.url, url);
            std::swap(other.path, path);
            std::swap(other.hash, hash);
            return *this;
        }
        Item() {}
    } cur;
    cds::container::FCDeque<Downloader::Item> items;
    std::atomic_bool work;
public:
    Downloader(QObject* parent = nullptr);
    dhash_t add_download(const QUrl& url, const std::wstring& path);
signals:
    void downloaded_file(dhash_t hash_item, dnetwork_error network_err, dfile_error file_err );
private slots:
    void download();
    void downloaded(QNetworkReply* reply);
};


#endif //MAGICBOXREDUXCMAKE_DOWNLOADER_H
