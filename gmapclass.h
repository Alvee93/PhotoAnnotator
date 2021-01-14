#ifndef GMAPCLASS_H
#define GMAPCLASS_H

#include <QObject>

//includes for http request
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QLoggingCategory> // this is for disabling the ssl certificate warnings

//includes for json parsing
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

//#include <QWebEngineView>

class GmapClass : public QObject
{
    Q_OBJECT
public:
    explicit GmapClass(QObject *parent = 0);
    QString getAddress(double map_lat, double map_long);
    //void showMap(double map_lat, double map_long);

signals:

public slots:
};

#endif // GMAPCLASS_H
