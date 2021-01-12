#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>

/**
 * \class DbManager
 *
 * \brief SQL Database Manager class
 *
 * DbManager sets up the connection with SQL database
 * and performs some basics queries. The class requires
 * existing SQL database. You can create it with sqlite:
 * 1. $ sqlite3 db_name.db
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
     * @brief Creates a new 'images' table if it doesn't already exist
     * @return true - 'images' table created successfully, false - table not created
     */
    bool createTable();

    /**
     * @brief Add image data to db
     * @param img - path of image to add & other params
     * @return true - person added successfully, false - image not added
     */
    int addImg(const QString& img, const QString& make, const QString& model, const QString& software, const int& bps, const int& width, const int& height, const QString& description, const int& orientation, const QString& copyright, const QString& datetime, const QString& o_datetime, const QString& d_datetime, const QString& subsecond, const int& exposure, const double& f_stop, const int& iso, const double& s_distance, const double& e_bias, const int& flash, const int& metering_mode, const double& focal_length, const double& focal_length_35mm, const double& latitude, const double& longitude, const double& altitude, const double& min_focal_length, const double& max_focal_length, const double& min_f_stop, const double& max_f_stop,const QString& lens_make, const QString& lens_model); // was type bool

    /**
     * @brief Remove image data from db
     * @param img - path of image to remove.
     * @return true - image removed successfully, false - image not removed
     */
    bool removeImg(const QString& img);

    /**
     * @brief Check if person of name "name" exists in db
     * @param img - path of image to check.
     * @return true - image exists, false - image does not exist
     */
    bool imgExists(const QString& img) const;

    /**
     * @brief Print paths of all images in db
     */
    void printAllImages() const;

    /**
     * @brief Remove all images from db
     * @return true - all images removed successfully, false - not removed
     */
    bool removeAllImages();

private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
