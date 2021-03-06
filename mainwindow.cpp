#include "mainwindow.h"
#include "ui_mainwindow.h"

QString folderPath, absFilePath, listView_2_image = "";
static const QString db_path = "m_managerV04.sqlite";
DbManager db(db_path);

DayLightClass dl_obj;
WeatherClass weather_obj;
GmapClass gmap_obj;
GCalendar gc_obj;
FaceRec face_obj;

double Glat=0, Glong=0;

std::string DateTime;

QString comboMake, comboDaylight, comboWeather, comboLocation, comboPerson, comboEvent;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString sPath = "C:/";
    dirModel = new QFileSystemModel (this);                     // setting up the directory model on tree view
    dirModel ->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirModel -> setRootPath(sPath);
    ui->treeView->setModel(dirModel);

    ui->treeView->hideColumn(1);    //hiding unnecessary columns on tree view
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);

    fileModel = new QFileSystemModel (this);                    // setting up the file model on list view
    fileModel ->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    fileModel -> setRootPath(sPath);

    QStringList filters;            // list view filters to show only image files
    filters << "*.JPG" << "*.PNG" << "*.ico" << "*.JPEG";
    fileModel->setNameFilters(filters);
    fileModel -> setNameFilterDisables(false);

    ui->listView->setModel(fileModel);

    db.createTable();

    show_allComboBoxes();

    ui->textEdit_device_code->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete dirModel;
    delete fileModel;
    delete ui;
}

/* This function is called when user selects a folder/ drive from the left tree view */
void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QString sPath = dirModel->fileInfo(index).absoluteFilePath();

    ui->listView->setRootIndex(fileModel->setRootPath(sPath));

    folderPath = sPath;
}

/* This function is called when user selects a photo from the bottom list view */
void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    if(!folderPath.isEmpty())
        {
        QString filename = fileModel->fileInfo(index).absoluteFilePath();

        // Show the selected file on imgLabel
        QPixmap pix(filename);
        int w = ui->imgLabel->width();
        int h = ui->imgLabel->height();
        ui->imgLabel->setPixmap (pix.scaled(w,h,Qt::KeepAspectRatio));

        absFilePath = filename;
    }
}

/* This function is called when user clicks button- Quick Browse */
void MainWindow::on_quickBrowse_clicked()
{
    QFileDialog dialog(this);
        dialog.setNameFilter(tr("Images(*.jpg *.png *.xpm)"));
        dialog.setViewMode(QFileDialog::Detail);
        QString filePath = QFileDialog::getOpenFileName(this,
                           tr("Open Images"), "",
                           tr("Image Files (*jpg *png *bmp)"));

        absFilePath = filePath;

        // Show the selected file on imgLabel
        QPixmap pix(filePath);
        int w = ui->imgLabel->width();
        int h = ui->imgLabel->height();
        ui->imgLabel->setPixmap (pix.scaled(w,h,Qt::KeepAspectRatio));
}

/* This function is called when user clicks button- Add to Library */
void MainWindow::on_pushButton_clicked()
{
    if(!absFilePath.isEmpty())
        {
            ui -> tabWidget->setCurrentIndex(1);

            // here the exif extractor will perform its work!!!
            if(!db.imgExists(absFilePath)) {
                ui->statusbar->showMessage("Collecting and extracting info, please wait...", 2000);
                extract_exif(absFilePath);
            }
            else {
                ui->statusbar->showMessage("Image already added in library, fetching info...", 2000);

                // Show fetched info on edit-text fields
                ui->textEdit_daylight->setText(db.getDaylightFromPath(absFilePath));
                ui->textEdit_weather->setText(db.getWeatherFromPath(absFilePath));
                ui->textEdit_location->setText(db.getLocationFromPath(absFilePath));
                ui->textEdit_person->setText(db.getPersonTagsFromPath(absFilePath));
                ui->textEdit_event->setText(db.getEventFromPath(absFilePath));
            }

            // Rotate the image according to the orientation
            QMatrix rm;
            QSqlQuery rotation_query = db.getOrientationFromPath(absFilePath);
            bool success_r = rotation_query.exec();

            if (success_r)
            {
                while(rotation_query.next()) {
                    if(rotation_query.value(0).toInt() == 6) rm.rotate(90);
                    else rm.rotate(0);
                }
            }

            // Show the image on imgLabel_2
            QPixmap pix(absFilePath);
            int w2 = ui->imgLabel_2->width();
            int h2 = ui->imgLabel_2->height();
            ui->imgLabel_2->setPixmap (pix.transformed(rm).scaled(w2,h2,Qt::KeepAspectRatio));

            // Shows image meta data in tab_2 table
            show_image_metadata();

            // Populates the listView_2
            show_image_result();

            // Update the combobox data
            show_allComboBoxes();
    }

    else{
        QMessageBox::information(this, tr("Important notice"), tr("Please select an image first and then add it to library."));
    }

    // Enable the Update button
    ui->pushButton_update->setEnabled(true);
}

/* This function is called when user clicks button- Update */
void MainWindow::on_pushButton_update_clicked()
{
    QString daylight = ui->textEdit_daylight->toPlainText();
    QString weather = ui->textEdit_weather->toPlainText();
    QString location = ui->textEdit_location->toPlainText();
    QString person_tag = ui->textEdit_person->toPlainText();
    QString event = ui->textEdit_event->toPlainText();

    if(db.setPersonFromPath(person_tag, absFilePath)
            & db.setDaylightFromPath(daylight, absFilePath)
            & db.setWeatherFromPath(weather, absFilePath)
            & db.setLocationFromPath(location, absFilePath)
            & db.setEventFromPath(event, absFilePath)) ui->statusbar->showMessage("Data updated!", 5000);
    else ui->statusbar->showMessage("Data update failed!", 5000);

    // Update the combobox data
    show_allComboBoxes();
}

/* This function gets the image metadata from db and shows in tab_2 table */
void MainWindow::show_image_metadata()
{
    // Get all data from db
    QSqlQueryModel *modal = new QSqlQueryModel;
    QSqlQuery query = db.getAllData(absFilePath);
    bool success = false;

    success = query.exec();
    if(!success){
        qDebug() << "Fetching metadata failed: " << query.lastError();
    }

    modal->setQuery(query);
    ui->tableView->setModel(modal);
}

/* This function populates the listView_2 when an image is added into library */
void MainWindow::show_image_result()
{
    // Get all image-paths from db based on make
    QString make = ui->comboBox_make->currentText();
    QString daylight = ui->comboBox_daylight->currentText();
    QString weather = ui->comboBox_weather->currentText();
    QString location = ui->comboBox_location->currentText();
    QString person = ui->comboBox_person->currentText();
    QString event = ui->comboBox_event->currentText();

    QSqlQueryModel *modal = new QSqlQueryModel;
    QSqlQuery query = db.getPathsFromAttributes(make, daylight, weather, location, person, event);
    bool success = query.exec();

    if(!success){
        qDebug() << "Fetching metadata failed: " << query.lastError();
    }

    // Set the modal data & populate this on the listView_2
    modal->setQuery(query);
    ui->listView_2->setModel(modal);
    ui->statusbar->showMessage("Total items = " + QString(modal->rowCount()), 4000);
}

/* This function popilates the comboBox_make with 'make' information */
void MainWindow::show_allComboBoxes()
{
    // Get all makes from db
    QSqlQueryModel *modal = new QSqlQueryModel;
    QSqlQuery query = db.getAllMakes();
    bool success = query.exec();

    if(!success){
        qDebug() << "Fetching makes failed: " << query.lastError();
    }

    modal->setQuery(query);
    ui->comboBox_make->setModel(modal);

    // Get all daylights from db
    QSqlQueryModel *modal_d = new QSqlQueryModel;
    query = db.getAllDaylights();
    success = query.exec();

    if(!success){
        qDebug() << "Fetching daylights failed: " << query.lastError();
    }

    modal_d->setQuery(query);
    ui->comboBox_daylight->setModel(modal_d);

    // Get all weathers from db
    QSqlQueryModel *modal_w = new QSqlQueryModel;
    query = db.getAllWeathers();
    success = query.exec();

    if(!success){
        qDebug() << "Fetching weathers failed: " << query.lastError();
    }

    modal_w->setQuery(query);
    ui->comboBox_weather->setModel(modal_w);

    // Get all locations from db
    QSqlQueryModel *modal_l = new QSqlQueryModel;
    query = db.getAllLocations();
    success = query.exec();

    if(!success){
        qDebug() << "Fetching locations failed: " << query.lastError();
    }

    modal_l->setQuery(query);
    ui->comboBox_location->setModel(modal_l);

    // Get all persons from db
    QSqlQueryModel *modal_p = new QSqlQueryModel;
    query = db.getAllPersons();
    success = query.exec();

    if(!success){
        qDebug() << "Fetching persons failed: " << query.lastError();
    }

    modal_p->setQuery(query);
    ui->comboBox_person->setModel(modal_p);

    // Get all events from db
    QSqlQueryModel *modal_e = new QSqlQueryModel;
    query = db.getAllEvents();
    success = query.exec();

    if(!success){
        qDebug() << "Fetching events failed: " << query.lastError();
    }

    modal_e->setQuery(query);
    ui->comboBox_event->setModel(modal_e);
}

/* This function extracts the Exif information from image and stores them in db */
void MainWindow::extract_exif(QString imgPath)
{
    QByteArray ba = imgPath.toLatin1();
    const char *temp = ba.data();

    FILE *fp = fopen(temp,"rb");// binPix; ;

    fseek(fp, 0, SEEK_END);
    unsigned long fsize = ftell(fp);
    rewind(fp);
    unsigned char *buf = new unsigned char[fsize];

    if (fread(buf, 1, fsize, fp) != fsize) {
        qDebug() << "Cannot read file";
        delete[] buf;
      }

    fclose(fp);

    easyexif::EXIFInfo result;
    int code = result.parseFrom(buf, fsize);
    delete[] buf;
    if (code) {
        qDebug() << "Error parsing Exif data.";
      }

    QString make = result.Make.c_str();
    QString model = result.Model.c_str();
    QString software = result.Software.c_str();
    int bps = result.BitsPerSample;
    int width = result.ImageWidth;
    int height = result.ImageHeight;
    QString description = result.ImageDescription.c_str();
    int orientation = result.Orientation;
    QString copyright = result.Copyright.c_str();
    QString datetime = result.DateTime.c_str();
    QString o_datetime = result.DateTimeOriginal.c_str();
    QString d_datetime = result.DateTimeDigitized.c_str();
    QString subsecond = result.SubSecTimeOriginal.c_str();
    int exposure = 1.0 / result.ExposureTime;
    double f_stop = result.FNumber;
    int iso = result.ISOSpeedRatings;
    double s_distance = result.SubjectDistance;
    double e_bias = result.ExposureBiasValue;
    int flash = result.Flash;
    int metering_mode = result.MeteringMode;
    double focal_length = result.FocalLength;
    double focal_length_35mm = result.FocalLengthIn35mm;
    double latitude = result.GeoLocation.Latitude;
    double longitude = result.GeoLocation.Longitude;
    double altitude = result.GeoLocation.Altitude;
    double min_focal_length = result.LensInfo.FocalLengthMin;
    double max_focal_length = result.LensInfo.FocalLengthMax;
    double min_f_stop = result.LensInfo.FStopMin;
    double max_f_stop = result.LensInfo.FStopMax;
    QString lens_make = result.LensInfo.Make.c_str();
    QString lens_model = result.LensInfo.Model.c_str();

    //daylight status
    QString daylight_status = dl_obj.getDayLight(latitude, longitude, o_datetime.toLocal8Bit().constData());

    //weather status
    QString weather_status = weather_obj.getWeather(latitude, longitude, o_datetime.toLocal8Bit().constData());

    QString location;
    if(latitude != 0 || longitude != 0) location = gmap_obj.getAddress(latitude, longitude);
    else location = QString("N/A");

    QString person_tags = QString("N/A");
    QString event = "N/A";

    db.addImg(imgPath, make, model, software, bps, width, height, description,
              orientation, copyright, datetime, o_datetime, d_datetime, subsecond,
              exposure, f_stop, iso, s_distance, e_bias, flash, metering_mode, focal_length,
              focal_length_35mm, latitude, longitude, altitude, min_focal_length, max_focal_length,
              min_f_stop, max_f_stop, lens_make, lens_model, daylight_status, weather_status, location, person_tags, event);

    ui->textEdit_daylight->setText(daylight_status);
    ui->textEdit_weather->setText(weather_status);
    ui->textEdit_location->setText(location);
    ui->textEdit_person->setText(person_tags);
    ui->textEdit_event->setText(event);
}

/* When user selects a photo from the listView_2, the image is loaded in imgLabel_3 & respective data is populated in tableView_2 */
void MainWindow::on_listView_2_activated(const QModelIndex &index)
{
    QString image_path = ui->listView_2->model()->data(index).toString();
    listView_2_image = image_path;

    // Get the Glat, Glong & DateTime data from selected image
    extractDatetimeLatLongData(db.getDatetimeLatLongData(image_path));

    // Show the image location on Google Map
    if(Glat != 0 || Glong != 0) {
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        ui->mView_2->setAttribute( Qt::WA_DeleteOnClose );      // delete object on closing or exiting
        ui->mView_2->load(QUrl("https://www.google.com/maps/place/"
                           + QString::number(Glat) + "," + QString::number(Glong)));
        ui->mView_2->setZoomFactor(0.85);
        ui->mView_2->setWindowTitle("Image Location on Map");
        ui->mView_2->show();

        //Reduce the size of imgLabel_3 and tableView_2 if there is map data
        ui->imgLabel_3->setFixedSize(400, ui->imgLabel_3->height());
        ui->imgLabel_3->move(420, ui->imgLabel_3->pos().y());
        ui->tableView_2->setFixedSize(800, 100);
    }
    else {
        //If there is no map data, set mView_2 as invisible and increase the size of imgLabel_3 and tableView_2
        ui->mView_2->setVisible(0);
        ui->imgLabel_3->setFixedSize(750, ui->imgLabel_3->height()); //setFixedSize(800, 391)
        ui->imgLabel_3->move(430, ui->imgLabel_3->pos().y());
        ui->tableView_2->setFixedSize(1180, 100);
    }

    // Rotate the image according to the orientation
    QMatrix rm;
    QSqlQuery rotation_query = db.getOrientationFromPath(image_path);
    bool success_r = rotation_query.exec();

    if (success_r)
    {
        while(rotation_query.next()) {
            if(rotation_query.value(0).toInt() == 6) rm.rotate(90);
            else rm.rotate(0);
        }
    }

    QPixmap pix(image_path);
    int w = ui->imgLabel_3->width ();
    int h = ui->imgLabel_3->height ();
    ui->imgLabel_3->setPixmap (pix.transformed(rm).scaled (w,h,Qt::KeepAspectRatio));

    // Get all data from db
    QSqlQueryModel *modal = new QSqlQueryModel;
    QSqlQuery query = db.getAllData(image_path);
    bool success = query.exec();

    if(!success){
        qDebug() << "Fetching metadata failed: " << query.lastError();
    }

    modal->setQuery(query);
    ui->tableView_2->setModel(modal);

    ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_2->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
}

void MainWindow::extractDatetimeLatLongData(string s)
{
    string delimiter = ",";
    size_t pos = 0;
    string token;
    int i=1;
    while ((pos = s.find(delimiter)) != std::string::npos) {

        token = s.substr(0, pos);
        if(i==1){
            DateTime = token;
        }
        if(i==2){
            Glat = atof(token.c_str());
        }
        if(i==3){
            Glong = atof(token.c_str());
        }
        i+=1;
        s.erase(0, pos + delimiter.length());
    }
}

/* Deletes the selected image on listView_2 */
void MainWindow::on_pushButton_remImage_clicked()
{
    if(!listView_2_image.isNull()) db.removeImg(listView_2_image);
    show_allComboBoxes();
    ui->statusbar->showMessage("Image successfully deleted from app!", 2000);
}

/* Shows the (paths of) images on listView_2 fetching data from db*/
void MainWindow::updateListViewResult()
{
    QSqlQueryModel *modal = new QSqlQueryModel;
        QSqlQuery query = db.getPathsFromAttributes(comboMake, comboDaylight, comboWeather, comboLocation, comboPerson, comboEvent);

        bool success = query.exec();

        if (query.exec())
        {
            modal->setQuery(query);
            ui->listView_2->setModel(modal);   // it exists
        }
        else if(!success){
            qDebug() << "Fetching 'currentIndexChanged' failed: " << query.lastError();
        }

        ui->statusbar->showMessage(QString(modal->rowCount()), 4000);
}

/* Shows the (paths of) images on listView_2 depending on what is selected on the comboBox_make for camera_make */
void MainWindow::on_comboBox_make_currentIndexChanged(const QString &arg1) //const QString &arg1
{
    // Get all image-paths from db for makes
    comboMake = ui->comboBox_make->currentText();

    // Update the listView_2
    updateListViewResult();
}

void MainWindow::on_comboBox_daylight_currentIndexChanged(const QString &arg1)
{
    // Get all image-paths from db for daylights
    comboDaylight = ui->comboBox_daylight->currentText();

    // Update the listView_2
    updateListViewResult();
}

void MainWindow::on_comboBox_weather_currentIndexChanged(const QString &arg1)
{
    // Get all image-paths from db for weathers
    comboWeather = ui->comboBox_weather->currentText();

    // Update the listView_2
    updateListViewResult();
}

void MainWindow::on_comboBox_location_currentIndexChanged(const QString &arg1)
{
    // Get all image-paths from db for locations
    comboLocation = ui->comboBox_location->currentText();

    // Update the listView_2
    updateListViewResult();
}

void MainWindow::on_comboBox_person_currentIndexChanged(const QString &arg1)
{
    // Get all image-paths from db for persons
    comboPerson = ui->comboBox_person->currentText();

    // Update the listView_2
    updateListViewResult();
}

void MainWindow::on_comboBox_event_currentIndexChanged(const QString &arg1)
{
    // Get all image-paths from db for events
    comboEvent = ui->comboBox_event->currentText();

    // Update the listView_2
    updateListViewResult();
}

/* Login to Google to grant permission before fetching event data. */
void MainWindow::on_pushButton_glogin_clicked()
{
    QString user_code = gc_obj.gLogin();
    ui->statusbar->showMessage(user_code, 10000);
    ui->textEdit_device_code->setVisible(true);
    ui->textEdit_device_code->setText(user_code);

    // WebView Portion
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    ui->mView_login->setAttribute( Qt::WA_DeleteOnClose );      // delete object on closing or exiting
    QWebEngineProfile::defaultProfile()->setHttpUserAgent("Mozilla/5.0 (X11; Linux x86_64; rv:57.0) Gecko/20100101 Firefox/57.0");
    ui->mView_login->load(QUrl("https://www.google.com/device"));
    ui->mView_login->setZoomFactor(0.85);
    ui->mView_login->resize(ui->mView_login->width(), ui->mView_login->height());
    ui->mView_login->show();
}

/* Fetch event info */
void MainWindow::on_pushButton_get_event_clicked()
{
    ui->textEdit_device_code->setVisible(false);

    extractDatetimeLatLongData(db.getDatetimeLatLongData(absFilePath));

    QString date_time(QString::fromStdString(DateTime));

    //'date' contains this format- "2019:09:04"
    QString date = date_time.left(10);
    date.replace(":", "-"); //'date' contains this format- "2019-09-04"

    // First need to call this for the system to get the access token, when request is made
    gc_obj.getAccessToken();

    QString g_calendar_event = gc_obj.getEvent(date);
    ui->textEdit_event->setText(g_calendar_event);
}

void MainWindow::on_pushButton_get_person_clicked()
{
    QPixmap pix(absFilePath);
    vector <QString> p_names = face_obj.predictFaces(QPixmapToCvMat(pix));
    stringstream ss;

    for(size_t i = 0; i < p_names.size(); ++i)
    {
      if(i != 0)
        ss << ", ";
      ss << p_names[i].toLocal8Bit().constData();
    }

    string persons = ss.str();
    ui->textEdit_person->setText(QString::fromStdString(persons));
}

void MainWindow::on_pushButton_2_clicked()
{
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
    lic_obj = new LicDialog(this);
    lic_obj->show();
}
