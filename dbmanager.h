#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <string>
using namespace std;

/**
 * \class DbManager
 *
 * \brief SQL Database Manager class
 *
 * DbManager sets up the connection with SQL database
 * and performs some basics queries. The class requires
 * existing SQL database. We can create it with sqlite:
 * 1. $ sqlite3 people.db
 * 2. sqilte> CREATE TABLE images(id INTEGER PRIMARY KEY, path TEXT, ...);
 * 3. sqlite> .quit
 */
class DbManager
{
public:
    /**
     * @brief Constructor
     *
     * Constructor sets up connection with db and opens it
     * @param path - absolute path to db file
     */
    DbManager(const QString& path);

    /**
     * @brief Destructor
     *
     * Close the db connection
     */
    ~DbManager();

    bool isOpen() const;

    /**
     * @brief Creates a new 'people' table if it doesn't already exist
     * @return true - 'people' table created successfully, false - table not created
     */
    bool createTable();

    /**
     * @brief Add image data to db
     * @param img - path of image, etc..
     * @return true - image added successfully, false - image not added
     */
    int addImg(const QString& img, const QString& make, const QString& model, const QString& software, const int& bps, const int& width, const int& height, const QString& description, const int& orientation, const QString& copyright, const QString& datetime, const QString& o_datetime, const QString& d_datetime, const QString& subsecond, const int& exposure, const double& f_stop, const int& iso, const double& s_distance, const double& e_bias, const int& flash, const int& metering_mode, const double& focal_length, const double& focal_length_35mm, const double& latitude, const double& longitude, const double& altitude, const double& min_focal_length, const double& max_focal_length, const double& min_f_stop, const double& max_f_stop,const QString& lens_make, const QString& lens_model, QString daylight_status, QString weather_status, QString location, QString person_tags, QString event); // was type bool

    /**
     * @brief Check if image exists in db
     * @param img - image-path to check.
     * @return true - image exists, false - image does not exist
     */
    bool imgExists(const QString& img) const;

    /**
     * @brief Remove image data from db
     * @param img - path of image to remove.
     * @return true - image removed successfully, false - image not removed
     */
    bool removeImg(const QString& img);

    /**
     * @brief Print images in db
     */
    void printAllImages() const;

    /**
     * @brief Get o_datetime, latitude, longitude from image
     * @param val - image path for the data.
     * @return lat, long & original_datetime
     */
    string getDatetimeLatLongData(QString image_path);

    /**
     * @brief Get all data for image
     * @param image_path - image path for the data.
     * @return QSqlQuery data for selected image
     */
    QSqlQuery getAllData(QString image_path);

    /**
     * @brief Get all paths of images depending on make
     * @param make - image make for the data.
     * @return QSqlQuery data for selected image
     */
    QSqlQuery getPathsFromMake(QString make);

    /**
     * @brief Get orientation of specific image
     * @param image_path - image path for the data.
     * @return QSqlQuery data for selected image
     */
    QSqlQuery getOrientationFromPath(QString image_path);

    /**
     * @brief Get daylight of specific image
     * @param image_path - image path for the data.
     * @return daylight data for selected image
     */
    QString getDaylightFromPath(QString image_path);

    /**
     * @brief Get weather of specific image
     * @param image_path - image path for the data.
     * @return weather data for selected image
     */
    QString getWeatherFromPath(QString image_path);

    /**
     * @brief Get location of specific image
     * @param image_path - image path for the data.
     * @return weather data for selected image
     */
    QString getLocationFromPath(QString image_path);

    /**
     * @brief Get person_tags of specific image
     * @param image_path - image path for the data.
     * @return weather data for selected image
     */
    QString getPersonTagsFromPath(QString image_path);

    /**
     * @brief Get event of specific image
     * @param image_path - image path for the data.
     * @return weather data for selected image
     */
    QString getEventFromPath(QString image_path);

    /**
     * @brief Get all makes of images
     * @return QSqlQuery data for selected image
     */
    QSqlQuery getAllMakes();

    /**
     * @brief Updates daylight, weather, location, person_tags & event for of specific image
     * @param image_path - image path for the data.
     */
    bool setDaylightFromPath(QString daylight, QString image_path) const;

    /**
     * @brief Updates weather for of specific image
     * @param image_path - image path for the data.
     */
    bool setWeatherFromPath(QString weather, QString image_path);

    /**
     * @brief Updates location for of specific image
     * @param image_path - image path for the data.
     */
    bool setLocationFromPath(QString location, QString image_path);

    /**
     * @brief Updates person for of specific image
     * @param p_tags - person_tags for the images
     * @param image_path - image path for the image.
     */
    bool setPersonFromPath(QString p_tags, QString image_path) const;

    /**
     * @brief Updates event for of specific image
     * @param image_path - image path for the data.
     */
    bool setEventFromPath(QString event, QString image_path);

    /**
     * @brief Remove all images from db
     * @return true - all images removed successfully, false - not removed
     */
    bool removeAllImages();

private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
