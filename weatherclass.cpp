#include "weatherclass.h"

//#include "mainwindow.h"
#include <QApplication>

WeatherClass::WeatherClass(QObject *parent) : QObject(parent)
{

}

QString WeatherClass::getWeather(double map_lat, double map_long, string p_date)
{
    QString weather_qStr; // this string will contain weather info

    // parsing date info

    string convDate = convertDate(p_date);
    string convDate_start = convDate.substr(0, 13);
    string convDate_end = convDate.substr(13, 13);      // start & end date parsing done

    QLoggingCategory::setFilterRules("qt.network.ssl.w arning=false");
    // create custom temporary event loop on stack
    QEventLoop eventLoop_weather;
    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr_weather;
    QObject::connect(&mgr_weather, SIGNAL(finished(QNetworkReply*)), &eventLoop_weather, SLOT(quit()));
    // the HTTP request
    QNetworkRequest req( QUrl( QString("https://api.weatherbit.io/v1.0/history/hourly?"
                                       "lat=" + QString::number(map_lat) + "&lon=" + QString::number(map_long) +
                                       "&start_date=" + QString::fromStdString(convDate_start) +
                                       "&end_date=" + QString::fromStdString(convDate_end) +
                                       "&key=5b8bcd2ec026441682f44a78ababca01")));
    //new key: 5b8bcd2ec026441682f44a78ababca01
    //old key: dd2ccb3aa99a47c08dda5e55196a8ddd
    //simple testing response of ip: http://ip.jsontest.com/
    QNetworkReply *reply_weather = mgr_weather.get(req);
    eventLoop_weather.exec(); // blocks stack until "finished()" has been called

    if (reply_weather->error() == QNetworkReply::NoError) {
        //success

        //qDebug() << reply_weather->readAll();  //reads quite well!!   // ** start json parsing
        QByteArray jsonData_weather = reply_weather->readAll();
        //file.close();
        //qDebug() << jsonData_weather;
        QJsonDocument document_weather = QJsonDocument::fromJson(jsonData_weather);
        QJsonObject object_weather = document_weather.object();

        QJsonValue value_weather = object_weather.value("data");
        if(!value_weather.isNull() || value_weather != ""){
            if(!value_weather.toArray().isEmpty()){
            QJsonArray array_weather = value_weather.toArray();
            //qDebug() << "just before foreach";

            QJsonObject object_weather2 = array_weather[0].toObject();
            QJsonObject object_weather3 = object_weather2.value("weather").toObject();
            //qDebug() << object_weather3.value("description").toString();

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
    //cout << token_year;
    token_month = org_date.substr(5, 2);
    //cout << token_month;
    token_day = org_date.substr(8, 2);
    //cout << token_day;
    token_hour = org_date.substr(11, 2);
    //cout << token_hour;

    string mod_date_start = token_year + "-" + token_month + "-" + token_day + ":" + token_hour;
    //cout << mod_date_start << endl;

    int hour_int = stoi(token_hour);
    hour_int++;
    //cout << hour_int << endl;
    string mod_date_end = token_year + "-" + token_month + "-" + token_day + ":" + to_string(hour_int);

    string mod_date = mod_date_start + mod_date_end;

    return mod_date;
}
