#ifndef GCALENDAR_H
#define GCALENDAR_H

// Qt includes
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
//includes for json parsing
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QLoggingCategory> // this is for disabling the ssl certificate warnings
#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QString>
#include <iostream>

class GCalendar
{
public:
    GCalendar();
    QString gLogin();
    void getAccessToken();
    QString getEvent(QString date);
};

#endif // GCALENDAR_H
