#include "network.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__
#if 0
#ifdef Q_OS_WIN
#include "curl/curl.h"

void test(){
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();qdebug<<curl;
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com/");

#ifdef SKIP_PEER_VERIFICATION
        /*
     * If you want to connect to a site who is not using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
        /*
     * If the site you are connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl refuses to connect. You can skip this
     * check, but it makes the connection insecure.
     */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

        /* cache the CA cert bundle in memory for a week */
        curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void *ptr, size_t size, size_t nmemb, void *stream){
            qdebug<<ptr<<size<<nmemb<<stream;
        });

        /* Perform the request, res gets the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}
#endif
#endif

NetWorkwidget::NetWorkwidget(QWidget *parent)
    : QWidget{parent}
{
#if 0
#ifdef Q_OS_WIN
    test();return;
#endif
#endif
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,this, [=](QNetworkReply *reply){
        qdebug << reply->size() << reply->url();
        QByteArray data = reply->readAll();
        QFile file("./recvdata");
        if(file.open(QIODevice::WriteOnly)){
            file.write(data);
        }
        reply->deleteLater();
    });
    //manager->get(QNetworkRequest(QUrl("https://www.baidu.com")));

    QUrl url("http://gips0.baidu.com/it/u=1690853528,2506870245&fm=3028&app=3028&f=JPEG&fmt=auto?w=1024&h=1024");
    QNetworkRequest *request = new QNetworkRequest();
    //request->setUrl(QUrl("http://qt-project.org"));
    //request->setRawHeader("User-Agent", "MyOwnBrowser 1.0");
    request->setUrl(url);

    QNetworkReply *reply = manager->get(*request);
    connect(reply, &QIODevice::readyRead, this, [=](){
        qdebug << "readyRead";
    });
    connect(reply, &QNetworkReply::errorOccurred,this, [=](QNetworkReply::NetworkError err){
        qdebug << err << qt_error_string(err);
    });
    connect(reply, &QNetworkReply::sslErrors,
            this, [=](const QList<QSslError> &errors){
        for(int i=0;i<errors.size();i++){
            qdebug<<errors[i].errorString();
        }
        // foreach (auto error, errors) {
        //     qdebug<<error.errorString();
        // }
        // //QList<QSslError>::iterator itstl;
        // QList<QSslError>::const_iterator itstlconst;
        // for(itstlconst = errors.begin();itstlconst != errors.end();itstlconst++){
        //     qdebug<<itstlconst->errorString();
        // }
        // // 读写
        // //QMutableListIterator<QSslError> itjava(errors);
        // QListIterator<QSslError> itjavaconst(errors);
        // while (itjavaconst.hasNext()) {
        //     qdebug << itjavaconst.next().errorString();
        // }
    });

}
