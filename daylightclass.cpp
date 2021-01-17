#include "daylightclass.h"
#include <iostream>
#include <stdio.h>
#include <QEventLoop>
using namespace std;
DayLightClass::DayLightClass(QObject *parent) : QObject(parent)
{

}

QString DayLightClass::getDayLight(double map_lat, double map_long, string p_date)
{
    // This is done by Current_Status below qstring
    string convDate = convertDate(p_date);
    string p_date_only = convDate.substr(0, 10);
    string current_time_hour = convDate.substr(11, 2);

    QLoggingCategory::setFilterRules("qt.network.ssl.w arning=false");  // just to ignore the logged ssl warnings

    // Create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // The HTTP request
    QNetworkRequest req( QUrl( QString("https://api.sunrise-sunset.org/json?lat="
                                       + QString::number(map_lat) + "&lng=" + QString::number(map_long)
                                       +"&date=" + QString::fromUtf8(p_date_only.c_str()) + "&formatted=0")));

    QNetworkReply *addr_reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    QString Current_Status;

    if (addr_reply->error() == QNetworkReply::NoError) {

        //success
        QByteArray jsonData = addr_reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(jsonData);
        QJsonObject object = document.object();

        QJsonObject object2 = object.value("results").toObject(); //day_length
        int DLSunrise = stoi(object2.value("sunrise").toString().toStdString().substr(11, 2)) - (12+6);
        int DLSunset = stoi(object2.value("sunset").toString().toStdString().substr(11, 2)) + (0+6);
        int DLNoon = stoi(object2.value("solar_noon").toString().toStdString().substr(11, 2)) + (0+6);

        int Current_Time = stoi(current_time_hour);

        if (Current_Time == DLSunrise){
            Current_Status = "Sunrise";
        }

        else if (Current_Time > DLSunrise && Current_Time < DLNoon){
            Current_Status = "Morning";
        }

        else if (Current_Time >= DLNoon && Current_Time < (DLSunset-3)){
            Current_Status = "Noon";
        }

        else if (Current_Time >= (DLSunset-3) && Current_Time < DLSunset){
            Current_Status = "Afternoon";
        }

        else if (Current_Time == DLSunset){
            Current_Status = "Sunset";
        }

        else if (Current_Time > DLSunset && Current_Time < DLSunset+2){
            Current_Status = "Dusk";
        }

        else {
            Current_Status = "Night";
        }
    }

    else {
        //failure
        qDebug() << "daylight_status fetch failure = " <<addr_reply->errorString();
        Current_Status = "N/A";
    }
    delete addr_reply;
    return Current_Status;

}

string DayLightClass::convertDate(string org_date)
{
    string token_year, token_month, token_day, token_hour;

    token_year = org_date.substr(0, 4);
    token_month = org_date.substr(5, 2);
    token_day = org_date.substr(8, 2);
    token_hour = org_date.substr(11, 2);

    string mod_date_start = token_year + "-" + token_month + "-" + token_day + ":" + token_hour;

    return mod_date_start;
}
