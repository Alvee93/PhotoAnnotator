#ifndef DAYLIGHTCLASS_H
#define DAYLIGHTCLASS_H

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

#include <iostream>     // for date type conversion
using namespace std;
class DayLightClass : public QObject
{
    Q_OBJECT
public:
    explicit DayLightClass(QObject *parent = 0);
    QString getDayLight(double map_lat, double map_long, string p_date);
    string convertDate(string org_date);

signals:

public slots:
};

#endif // DAYLIGHTCLASS_H
