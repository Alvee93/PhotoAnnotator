#include "weatherclass.h"

#include <QApplication>

QString api_key = "_api_key_";

WeatherClass::WeatherClass(QObject *parent) : QObject(parent)
{

}

QString WeatherClass::getWeather(double map_lat, double map_long, string p_date)
{
    // This string will contain weather info
    QString weather_qStr;

    // Parsing date info
    string convDate = convertDate(p_date);
    string convDate_start = convDate.substr(0, 13);
    string convDate_end = convDate.substr(13, 13);

    QLoggingCategory::setFilterRules("qt.network.ssl.w arning=false");

    // Create custom temporary event loop on stack
    QEventLoop eventLoop_weather;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr_weather;
    QObject::connect(&mgr_weather, SIGNAL(finished(QNetworkReply*)), &eventLoop_weather, SLOT(quit()));

    // The HTTP request
    QNetworkRequest req( QUrl( QString("https://api.weatherbit.io/v1.0/history/hourly?"
                                       "lat=" + QString::number(map_lat) + "&lon=" + QString::number(map_long) +
                                       "&start_date=" + QString::fromStdString(convDate_start) +
                                       "&end_date=" + QString::fromStdString(convDate_end) +
                                       "&key=" + api_key)));

    QNetworkReply *reply_weather = mgr_weather.get(req);

    // Blocks stack until "finished()" has been called
    eventLoop_weather.exec();

    if (reply_weather->error() == QNetworkReply::NoError) {

        QByteArray jsonData_weather = reply_weather->readAll();
        QJsonDocument document_weather = QJsonDocument::fromJson(jsonData_weather);
        QJsonObject object_weather = document_weather.object();

        QJsonValue value_weather = object_weather.value("data");
        if(!value_weather.isNull() || value_weather != ""){
            if(!value_weather.toArray().isEmpty()){
            QJsonArray array_weather = value_weather.toArray();

            QJsonObject object_weather2 = array_weather[0].toObject();
            QJsonObject object_weather3 = object_weather2.value("weather").toObject();

            weather_qStr = object_weather3.value("description").toString();
            }
            else {
                weather_qStr = "Sorry, too early date found on image for forecasting.";
            }
        }

        else {
            weather_qStr = "Sorry, too early date found on image for forecasting.";
        }
    }

    else {
        //failure
        qDebug() << "weather data fetch failure = " <<reply_weather->errorString();
        weather_qStr = "N/A";
    }

    delete reply_weather;
    return weather_qStr;
}

string WeatherClass::convertDate(string org_date)
{
    string token_year, token_month, token_day, token_hour;

    token_year = org_date.substr(0, 4);
    token_month = org_date.substr(5, 2);
    token_day = org_date.substr(8, 2);
    token_hour = org_date.substr(11, 2);

    string mod_date_start = token_year + "-" + token_month + "-" + token_day + ":" + token_hour;

    int hour_int = stoi(token_hour);
    hour_int++;
    string mod_date_end = token_year + "-" + token_month + "-" + token_day + ":" + to_string(hour_int);

    string mod_date = mod_date_start + mod_date_end;

    return mod_date;
}
