#include "gmapclass.h"

using namespace std;

GmapClass::GmapClass(QObject *parent) : QObject(parent)
{

}

QString GmapClass::getAddress(double map_lat, double map_long)
{
    ostringstream streamObj1;
    streamObj1 << fixed;
    streamObj1 << setprecision(15);
    streamObj1 << map_lat;
    string m_lat = streamObj1.str();

    ostringstream streamObj2;
    streamObj2 << fixed;
    streamObj2 << setprecision(15);
    streamObj2 << map_long;
    string m_long = streamObj2.str();

    // This string will hold the address or blank string
    QString addr_qStr;

    // Just to ignore the logged ssl warnings
    QLoggingCategory::setFilterRules("qt.network.ssl.w arning=false");

    // Create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // The HTTP request
    QNetworkRequest req( QUrl( QString("https://maps.googleapis.com/maps/api/geocode/json?latlng="
                                       + QString::fromStdString(m_lat) + "," + QString::fromStdString(m_long)
                                       +"&result_type=administrative_area_level_2"
                                        "&key=AIzaSyBysUF3Tsd1ebG5zIHeYJOPiVlD_lUfwSs")));
    QNetworkReply *addr_reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (addr_reply->error() == QNetworkReply::NoError) {

        QByteArray jsonData = addr_reply->readAll();

        QJsonDocument document = QJsonDocument::fromJson(jsonData);
        QJsonObject object = document.object();

        QJsonValue value = object.value("results");
        QJsonArray array = value.toArray();

        addr_qStr = array[0].toObject().value("formatted_address").toString();
    }
    else {
        //failure
        qDebug() << "Address fetch failure = " <<addr_reply->errorString();
        addr_qStr = "N/A";
    }
    delete addr_reply;
    return addr_qStr;
}
