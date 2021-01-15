#include <dbmanager.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

DbManager::DbManager(const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
}

DbManager::~DbManager()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

bool DbManager::isOpen() const
{
    return m_db.isOpen();
}

bool DbManager::createTable()
{
    bool success = false;

    QSqlQuery query;
    query.prepare("CREATE TABLE images(id INTEGER PRIMARY KEY, path TEXT, make TEXT, model TEXT, software TEXT, "
                  "bps INTEGER, width INTEGER, height INTEGER, description TEXT, orientation INTEGER, copyright TEXT, "
                  "datetime TEXT, o_datetime TEXT, d_datetime TEXT, subsecond TEXT, exposure INTEGER, f_stop REAL, "
                  "iso INTEGER, s_distance REAL, e_bias REAL, flash INTEGER, metering_mode INTEGER, focal_length REAL, "
                  "focal_length_35mm REAL, latitude REAL, longitude REAL, altitude REAL, min_focal_length REAL, "
                  "max_focal_length REAL, min_f_stop REAL, max_f_stop REAL, lens_make TEXT, lens_model TEXT, "
                  "daylight_status TEXT, weather_status TEXT, location TEXT, person_tags TEXT, event TEXT);");

    if (!query.exec())
    {
        qDebug() << "Couldn't create the table 'images': one might already exist.";
        success = false;
    }

    return success;
}

int DbManager::addImg(const QString& img, const QString& make, const QString& model, const QString& software, const int& bps, const int& width, const int& height, const QString& description, const int& orientation, const QString& copyright, const QString& datetime, const QString& o_datetime, const QString& d_datetime, const QString& subsecond, const int& exposure, const double& f_stop, const int& iso, const double& s_distance, const double& e_bias, const int& flash, const int& metering_mode, const double& focal_length, const double& focal_length_35mm, const double& latitude, const double& longitude, const double& altitude, const double& min_focal_length, const double& max_focal_length, const double& min_f_stop, const double& max_f_stop,const QString& lens_make, const QString& lens_model, QString daylight_status, QString weather_status, QString location, QString person_tags, QString event)
{
    bool success = false;

    if ((!imgExists(img)) && (!img.isEmpty()))
    {
        QSqlQuery queryAdd;
        queryAdd.prepare("INSERT INTO images (path, make, model, software, bps, width, height, description, orientation, copyright, "
                         "datetime, o_datetime, d_datetime, subsecond, exposure, f_stop, iso, s_distance, e_bias, flash, metering_mode, "
                         "focal_length, focal_length_35mm, latitude, longitude, altitude, min_focal_length, max_focal_length, min_f_stop, "
                         "max_f_stop, lens_make, lens_model, daylight_status, weather_status, location, person_tags, event) VALUES (:path, :make, :model, :software, :bps, :width, :height, :description, :orientation, :copyright, :datetime, :o_datetime, :d_datetime, :subsecond, :exposure, :f_stop, :iso, :s_distance, :e_bias, :flash, :metering_mode, :focal_length, :focal_length_35mm, :latitude, :longitude, :altitude, :min_focal_length, :max_focal_length, :min_f_stop, :max_f_stop, :lens_make, :lens_model, :daylight_status, :weather_status, :location, :person_tags, :event)");
        queryAdd.bindValue(":path", img);
        queryAdd.bindValue(":make", make);
        queryAdd.bindValue(":model", model);
        queryAdd.bindValue(":software", software);
        queryAdd.bindValue(":bps", bps);
        queryAdd.bindValue(":width", width);
        queryAdd.bindValue(":height", height);
        queryAdd.bindValue(":description", description);
        queryAdd.bindValue(":orientation", orientation);
        queryAdd.bindValue(":copyright", copyright);
        queryAdd.bindValue(":datetime", datetime);
        queryAdd.bindValue(":o_datetime", o_datetime);
        queryAdd.bindValue(":d_datetime", d_datetime);
        queryAdd.bindValue(":subsecond", subsecond);
        queryAdd.bindValue(":exposure", exposure);
        queryAdd.bindValue(":f_stop", f_stop);
        queryAdd.bindValue(":iso", iso);
        queryAdd.bindValue(":s_distance", s_distance);
        queryAdd.bindValue(":e_bias", e_bias);
        queryAdd.bindValue(":flash", flash);
        queryAdd.bindValue(":metering_mode", metering_mode);
        queryAdd.bindValue(":focal_length", focal_length);
        queryAdd.bindValue(":focal_length_35mm", focal_length_35mm);
        queryAdd.bindValue(":latitude", latitude);
        queryAdd.bindValue(":longitude", longitude);
        queryAdd.bindValue(":altitude", altitude);
        queryAdd.bindValue(":min_focal_length", min_focal_length);
        queryAdd.bindValue(":max_focal_length", max_focal_length);
        queryAdd.bindValue(":min_f_stop", min_f_stop);
        queryAdd.bindValue(":max_f_stop", max_f_stop);
        queryAdd.bindValue(":lens_make", lens_make);
        queryAdd.bindValue(":lens_model", lens_model);
        queryAdd.bindValue(":daylight_status", daylight_status);
        queryAdd.bindValue(":weather_status", weather_status);
        queryAdd.bindValue(":location", location);
        queryAdd.bindValue(":person_tags", person_tags);
        queryAdd.bindValue(":event", event);

        if(queryAdd.exec())
        {
            success = true;
        }
        else
        {
            qDebug() << "Add image failed: " << queryAdd.lastError();
        }
    }
    else if(imgExists(img)){
        qDebug() << "Add image failed: image already exists";
    }
    else
    {
        qDebug() << "Add image failed: name cannot be empty";
    }

    return success;
}

bool DbManager::imgExists(const QString& img) const
{
    bool exists = false;

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT path FROM images WHERE path = (:path)");
    checkQuery.bindValue(":path", img);

    if (checkQuery.exec())
    {
        if (checkQuery.next())
        {
            exists = true;
        }
    }
    else
    {
        qDebug() << "Image exist query failed: " << checkQuery.lastError();
    }

    return exists;
}

bool DbManager::removeImg(const QString& img)
{
    bool success = false;

    if (imgExists(img))
    {
        QSqlQuery queryDelete;
        queryDelete.prepare("DELETE FROM images WHERE path = (:path)");
        queryDelete.bindValue(":path", img);
        success = queryDelete.exec();

        if(!success)
        {
            qDebug() << "Remove image failed: " << queryDelete.lastError();
        }
    }
    else
    {
        qDebug() << "Remove image failed: image doesnt exist";
    }

    return success;
}

void DbManager::printAllImages() const
{
    qDebug() << "Images in db:";
    QSqlQuery query("SELECT * FROM images");
    int idName = query.record().indexOf("path");
    while (query.next())
    {
        QString name = query.value(idName).toString();
        qDebug() << "===" << name;
    }
}

QSqlQuery DbManager::getAllData(QString image_path)
{
    QSqlQuery query2;
    bool success2 = false;

    query2.prepare("SELECT make, model, software, bps, width, height, description, orientation, copyright, datetime, "
                  "o_datetime, d_datetime, subsecond, exposure, f_stop, iso, s_distance, e_bias, flash, metering_mode, "
                  "focal_length, focal_length_35mm, latitude, longitude, altitude, min_focal_length, max_focal_length, "
                  "min_f_stop, max_f_stop, lens_make, lens_model, daylight_status, weather_status, location, person_tags, event"
                  " FROM images WHERE path = (:path2)");
    query2.bindValue(":path2", image_path.toUtf8().constData());

    success2 = query2.exec();
    if(!success2){
        qDebug() << "Fetching metadata failed: " << query2.lastError();
    }
    else{
        //qDebug() << "Success fetching all data.";
    }

    return query2;
}

QSqlQuery DbManager::getAllMakes()
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT DISTINCT make FROM images");
    success2 = query.exec();
    if(!success2){
        qDebug() << "Fetching all make failed: " << query.lastError();
    }
    else{
        //qDebug() << "Success fetching all make data.";
    }

    return query;
}

QSqlQuery DbManager::getAllDaylights()
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT DISTINCT daylight_status FROM images");
    success2 = query.exec();
    if(!success2){
        qDebug() << "Fetching all daylight_status failed: " << query.lastError();
    }
    else{
        //qDebug() << "Success fetching all daylight_status data.";
    }

    return query;
}

QSqlQuery DbManager::getAllWeathers()
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT DISTINCT weather_status FROM images");
    success2 = query.exec();
    if(!success2){
        qDebug() << "Fetching all weather_status failed: " << query.lastError();
    }
    else{
        //qDebug() << "Success fetching all daylight_status data.";
    }

    return query;
}

QSqlQuery DbManager::getAllLocations()
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT DISTINCT location FROM images");
    success2 = query.exec();
    if(!success2){
        qDebug() << "Fetching all location failed: " << query.lastError();
    }
    else{
        //qDebug() << "Success fetching all location data.";
    }

    return query;
}

QSqlQuery DbManager::getAllPersons()
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT DISTINCT person_tags FROM images");
    success2 = query.exec();
    if(!success2){
        qDebug() << "Fetching all location failed: " << query.lastError();
    }
    else{
        //qDebug() << "Success fetching all location data.";
    }

    return query;
}

QSqlQuery DbManager::getAllEvents()
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT DISTINCT event FROM images");
    success2 = query.exec();
    if(!success2){
        qDebug() << "Fetching all event failed: " << query.lastError();
    }
    else{
        //qDebug() << "Success fetching all event data.";
    }

    return query;
}

string DbManager::getDatetimeLatLongData(QString image_path)
{
    QSqlQuery query2;
    bool success2 = false;
    string raw_data="";

    query2.prepare("SELECT o_datetime, latitude, longitude FROM images WHERE path = (:path2)");
    query2.bindValue(":path2", image_path.toUtf8().constData());

    success2 = query2.exec();
    if(!success2){
        qDebug() << "Fetching metadata failed: " << query2.lastError();
    }
    if(query2.first()){
        for(int i=0; i<3; i++){
            raw_data = raw_data + query2.value(i).toString().toUtf8().constData() + ",";
        }

    }
    else{
        qDebug() << "Fetching data failed";
    }

    return raw_data;
}

QSqlQuery DbManager::getPathsFromAttributes(QString make, QString daylight, QString weather, QString location, QString person, QString event)
{
    QSqlQuery query;
    bool success = false;
    QString query_text = "SELECT path FROM images WHERE 1 = 1";

    if(!make.isEmpty()) query_text += " AND make = (:make)";
    if(!daylight.isEmpty()) query_text += " AND daylight_status = (:daylight_status)";
    if(!weather.isEmpty()) query_text += " AND weather_status = (:weather_status)";
    if(!location.isEmpty()) query_text += " AND location = (:location)";
    if(!person.isEmpty()) query_text += " AND person_tags = (:person_tags)";
    if(!event.isEmpty()) query_text += " AND event = (:event)";


    query.prepare(query_text);

    if(!make.isEmpty()) query.bindValue(":make", make.toUtf8().constData());
    if(!daylight.isEmpty()) query.bindValue(":daylight_status", daylight.toUtf8().constData());
    if(!weather.isEmpty()) query.bindValue(":weather_status", weather.toUtf8().constData());
    if(!location.isEmpty()) query.bindValue(":location", location.toUtf8().constData());
    if(!person.isEmpty()) query.bindValue(":person_tags", person.toUtf8().constData());
    if(!event.isEmpty()) query.bindValue(":event", event.toUtf8().constData());

    success = query.exec();
    if(!success)
    {
        qDebug() << "Fetching Attribute metadata failed: " << query.lastError();
    }
    else
    {
        //qDebug() << "Success Attribute make data.";
    }

    return query;
}

QSqlQuery DbManager::getOrientationFromPath(QString image_path)
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT orientation FROM images WHERE path = (:path)");
    query.bindValue(":path", image_path.toUtf8().constData());

    success2 = query.exec();
    if(!success2){
        qDebug() << "Fetching metadata failed: " << query.lastError();
    }
    else
    {
        //qDebug() << "Success orientation make data.";
    }

    return query;
}

QString DbManager::getDaylightFromPath(QString image_path)
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT daylight_status FROM images WHERE path = (:path)");
    query.bindValue(":path", image_path.toUtf8().constData());

    success2 = query.exec();
    if(!success2)
    {
        qDebug() << "Fetching daylight failed: " << query.lastError();
        return "N/A";
    }
    else
    {
        while(query.next()) {
            return query.value(0).toString();
        }
        //qDebug() << "Success fetching daylight data.";
    }

    return "N/A";
}

QString DbManager::getWeatherFromPath(QString image_path)
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT weather_status FROM images WHERE path = (:path)");
    query.bindValue(":path", image_path.toUtf8().constData());

    success2 = query.exec();
    if(!success2)
    {
        qDebug() << "Fetching weather_status failed: " << query.lastError();
        return "N/A";
    }
    else
    {
        while(query.next()) {
            return query.value(0).toString();
        }
        //qDebug() << "Success fetching weather_status data.";
    }

    return "N/A";
}

QString DbManager::getLocationFromPath(QString image_path)
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT location FROM images WHERE path = (:path)");
    query.bindValue(":path", image_path.toUtf8().constData());

    success2 = query.exec();
    if(!success2)
    {
        qDebug() << "Fetching location failed: " << query.lastError();
        return "N/A";
    }
    else{
        while(query.next())
        {
            return query.value(0).toString();
        }
        //qDebug() << "Success fetching location data.";
    }

    return "N/A";
}

QString DbManager::getPersonTagsFromPath(QString image_path)
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT person_tags FROM images WHERE path = (:path)");
    query.bindValue(":path", image_path.toUtf8().constData());

    success2 = query.exec();
    if(!success2)
    {
        qDebug() << "Fetching person_tags failed: " << query.lastError();
        return "N/A";
    }
    else
    {
        while(query.next()) {
            return query.value(0).toString();
        }
        //qDebug() << "Success fetching person_tags data.";
    }

    return "N/A";
}

QString DbManager::getEventFromPath(QString image_path)
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("SELECT event FROM images WHERE path = (:path)");
    query.bindValue(":path", image_path.toUtf8().constData());

    success2 = query.exec();
    if(!success2)
    {
        qDebug() << "Fetching event failed: " << query.lastError();
        return "N/A";
    }
    else
    {
        while(query.next()) {
            return query.value(0).toString();
        }
        //qDebug() << "Success fetching event data.";
    }

    return "N/A";
}

bool DbManager::setDaylightFromPath(QString daylight, QString image_path) const
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("UPDATE images SET daylight_status = (:daylight) WHERE path = (:path)");
    query.bindValue(":daylight", daylight.toUtf8().constData());
    query.bindValue(":path", image_path.toUtf8().constData());
    success2 = query.exec();

    if(!success2)
    {
        qDebug() << "Update daylight failed: " << query.lastError();
        return false;
    }
    else{
        //qDebug() << "Success updating daylight.";
    }
    return true;
}

bool DbManager::setWeatherFromPath(QString weather, QString image_path)
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("UPDATE images SET weather_status = (:weather_status) WHERE path = (:path)");
    query.bindValue(":weather_status", weather.toUtf8().constData());
    query.bindValue(":path", image_path.toUtf8().constData());
    success2 = query.exec();

    if(!success2)
    {
        qDebug() << "Update weather failed: " << query.lastError();
        return false;
    }
    else
    {
        //qDebug() << "Success updating weather.";
    }
    return true;
}

bool DbManager::setLocationFromPath(QString location, QString image_path)
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("UPDATE images SET location = (:location) WHERE path = (:path)");
    query.bindValue(":location", location.toUtf8().constData());
    query.bindValue(":path", image_path.toUtf8().constData());
    success2 = query.exec();

    if(!success2)
    {
        qDebug() << "Update location failed: " << query.lastError();
        return false;
    }
    else
    {
        //qDebug() << "Success updating location.";
    }
    return true;
}

bool DbManager::setPersonFromPath(QString p_tags, QString image_path) const
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("UPDATE images SET person_tags = (:p_tags) WHERE path = (:path)");
    query.bindValue(":p_tags", p_tags.toUtf8().constData());
    query.bindValue(":path", image_path.toUtf8().constData());
    success2 = query.exec();

    if(!success2)
    {
        qDebug() << "Update person_tags failed: " << query.lastError();
        return false;
    }
    else
    {
        //qDebug() << "Success updating person_tags.";
    }
    return true;
}

bool DbManager::setEventFromPath(QString event, QString image_path)
{
    QSqlQuery query;
    bool success2 = false;

    query.prepare("UPDATE images SET event = (:event) WHERE path = (:path)");
    query.bindValue(":event", event.toUtf8().constData());
    query.bindValue(":path", image_path.toUtf8().constData());
    success2 = query.exec();

    if(!success2)
    {
        qDebug() << "Update event failed: " << query.lastError();
        return false;
    }
    else
    {
        //qDebug() << "Success updating event.";
    }
    return true;
}

bool DbManager::removeAllImages()
{
    bool success = false;

    QSqlQuery removeQuery;
    removeQuery.prepare("DELETE FROM images");

    if (removeQuery.exec())
    {
        success = true;
    }
    else
    {
        qDebug() << "remove all images failed: " << removeQuery.lastError();
    }

    return success;
}
