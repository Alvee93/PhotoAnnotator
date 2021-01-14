#include "mainwindow.h"
#include "ui_mainwindow.h"

QString folderPath, absFilePath, listView_2_image = "";
static const QString db_path = "m_managerV04.sqlite";
DbManager db(db_path);

DayLightClass dl_obj;
WeatherClass weather_obj;
GmapClass gmap_obj;

double Glat=0, Glong=0;

std::string DateTime;

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

    show_comboBox_make();
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

    //qDebug() << "sPath = " << sPath;

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
        //db.addImg(filename);
        //ui -> label -> setNum(db.imgExists(filename));

        /*QPixmap icon(filename);
        QFileIconProvider provider;
        QFileInfo file(filename);
        icon = provider.icon(QFileIconProvider::File).pixmap(32, 32);
        ui->imgLabel_test->setPixmap(icon);

        QStandardItemModel *model = new QStandardItemModel(this);
        ui->listView_test->setModel(model);
        model->appendRow(new QStandardItem(QIcon(filename), ""));*/
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

            show_comboBox_make();
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
    // Disable the Update button
    //ui->pushButton_update->setEnabled(false);

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

/* This function populates the listView_2 based on what is selected in the comboBox_make for the first time the app is launched */
void MainWindow::show_image_result()
{
    // Get all image-paths from db based on make
    QString make = ui->comboBox_make->currentText();
    QSqlQueryModel *modal = new QSqlQueryModel;
    QSqlQuery query = db.getPathsFromMake(make);
    bool success = query.exec();

    if(!success){
        qDebug() << "Fetching metadata failed: " << query.lastError();
    }

    // Set the modal data & populate this on the listView_2
    modal->setQuery(query);
    ui->listView_2->setModel(modal);
}

/* This function popilates the comboBox_make with 'make' information */
void MainWindow::show_comboBox_make()
{
    // Get all makes from db
    QSqlQueryModel *modal = new QSqlQueryModel;
    QSqlQuery query = db.getAllMakes();
    bool success = query.exec();

    if(!success){
        qDebug() << "Fetching make failed: " << query.lastError();
    }

    modal->setQuery(query);
    ui->comboBox_make->setModel(modal);
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
        //return 0;
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
    //ui->label_debug->setText(daylight_status);
    //qDebug() << "daylight_status = " << daylight_status;

    //weather status
    QString weather_status = weather_obj.getWeather(latitude, longitude, o_datetime.toLocal8Bit().constData());
    //ui->label_debug->setText(weather_status);
    //qDebug() << "weather_status = " << weather_status;

    QString location;
    if(latitude != 0 || longitude != 0) location = gmap_obj.getAddress(latitude, longitude);
    else location = QString("N/A");
    //ui->label->setText(location);

    QString person_tags = QString("N/A");
    QString event = "N/A";

    db.addImg(imgPath, make, model, software, bps, width, height, description,
              orientation, copyright, datetime, o_datetime, d_datetime, subsecond,
              exposure, f_stop, iso, s_distance, e_bias, flash, metering_mode, focal_length,
              focal_length_35mm, latitude, longitude, altitude, min_focal_length, max_focal_length,
              min_f_stop, max_f_stop, lens_make, lens_model, daylight_status, weather_status, location, person_tags, event);
    //qDebug() << "o_datetime = " << o_datetime;

    ui->textEdit_daylight->setText(daylight_status);
    ui->textEdit_weather->setText(weather_status);
    ui->textEdit_location->setText(location);
    ui->textEdit_person->setText(person_tags);
    ui->textEdit_event->setText(event);

    /*ui->cMake-> setText(result.Make.c_str());
    ui->cMod-> setText(result.Model.c_str());
    ui->eSoft-> setText(result.Software.c_str());
    ui->iTitle-> setText(result.ImageDescription.c_str());
    ui->eTime-> setText(QString("1/ %1 Sec").arg(1.0 / result.ExposureTime));//format: setText(QString("%1").arg(c)
    ui->iSpeedRating-> setNum(result.ISOSpeedRatings);
    ui->iBPS-> setNum(result.BitsPerSample);
    ui->iWidth-> setText(QString(result.ImageWidth));
    ui->iHeight-> setText(QString(result.ImageHeight));
    ui->iOrientation-> setNum(result.Orientation);
    ui->copyR-> setText(result.Copyright.c_str());
    ui->iDTime-> setText(result.DateTime.c_str());
    ui->iiDTOri-> setText(result.DateTimeOriginal.c_str());
    ui->iDTDi-> setText(result.DateTimeDigitized.c_str());
    ui->sSTO-> setText(result.SubSecTimeOriginal.c_str());
    ui->fStop-> setText("f/" + QString::number(result.FNumber));
    ui->sDistance-> setText(QString::number(result.SubjectDistance));
    ui->eBias-> setText(QString::number(result.ExposureBiasValue) + " EV");
    ui->rFlash-> setText("Flash: " + QString::number(result.Flash));
    ui->rMetering-> setText(QString::number(result.MeteringMode)); // ///////////////
    ui->fLength-> setText(QString("%2 mm").arg(result.FocalLength));
    ui->fLength35-> setText(QString::number(result.FocalLengthIn35mm) + " mm");
    //ui->cMake-> setText(result.Make.c_str());

    //result.ExposureTime
    */
}

/* Shows the (paths of) images on listView_2 depending on what is selected on the comboBox_make for camera_make */
void MainWindow::on_comboBox_make_currentIndexChanged(const QString &arg1) //const QString &arg1
{
    // Get all image-paths from db for make
    QSqlQueryModel *modal = new QSqlQueryModel;
    QString make = ui->comboBox_make->currentText();
    QSqlQuery query = db.getPathsFromMake(make);
    bool success = query.exec();

    if (query.exec())
    {
       if (query.next())
       {
            modal->setQuery(query);
            ui->listView_2->setModel(modal);   // it exists
            //ui->listView_2->setModel(fileModel->setRootPath();
            //ui->listView_2->setRootIndex(fileModel->setRootPath(sPath));
            //qDebug() << modal;
       }
    }
    else if(!success){
        qDebug() << "Fetching result failed: " << query.lastError();
    }



    /*QStandardItemModel *model = new QStandardItemModel(this);
    ui->listView_2->setModel(model);
    while(query.next()) {
        model->appendRow(new QStandardItem(QIcon(query.value(0).toString()), "teststr"));
        //qDebug() << query.value(0).toString();
    }*/
    //qDebug() << "model->item(0, 0) = " << model->item(0, 0)->text();
}

/* When user selects a photo from the listView_2, the image is loaded in imgLabel_3 & respective data is populated in tableView_2 */
void MainWindow::on_listView_2_activated(const QModelIndex &index)
{
    QString image_path = ui->listView_2->model()->data(index).toString();
    //qDebug() << "val = " << val; //val
    //qDebug() << "index = " << ui->listView_2->model()->index(0, 0).data().toMap().value("name").toString(); //index

    // Get the Glat, Glong & DateTime data from selected image
    extractDatetimeLatLongData(db.getDatetimeLatLongData(image_path));
    //qDebug() << "getApiData(val) = " << QString::fromStdString(db.getApiData(image_path));

    // Show the image location on Google Map
    if(Glat != 0 || Glong != 0) {
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        ui->mView_2->setAttribute( Qt::WA_DeleteOnClose );      // delete object on closing or exiting
        ui->mView_2->load(QUrl("https://www.google.com/maps/place/"
                           + QString::number(Glat) + "," + QString::number(Glong)));
        ui->mView_2->setZoomFactor(0.85);
        //ui->mView_2->resize(680, 490);
        ui->mView_2->setWindowTitle("Image Location on Map");
        ui->mView_2->show();

        //Reduce the size of imgLabel_3 and tableView_2 if there is map data
        ui->imgLabel_3->setFixedSize(400, ui->imgLabel_3->height());
        ui->imgLabel_3->move(250, ui->imgLabel_3->pos().y());
        ui->tableView_2->setFixedSize(650, 100);
    }
    else {
        //If there is no map data, set mView_2 as invisible and increase the size of imgLabel_3 and tableView_2
        ui->mView_2->setVisible(0);
        ui->imgLabel_3->setFixedSize(750, ui->imgLabel_3->height()); //setFixedSize(800, 391)
        ui->imgLabel_3->move(350, ui->imgLabel_3->pos().y());
        ui->tableView_2->setFixedSize(1040, 100);
    }
    //qDebug() << "Glat = " << Glat;


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
    //ui->imgLabel_3->setText(val);

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
    //ui->tableView_2->horizontalHeader()->setContentsMargins(50, 50, 50, 50); // left, top, right, bottom
    //ui->tableView_2->setContentsMargins(100, 100, 100, 100);
}

void MainWindow::on_listView_2_clicked(const QModelIndex &index)
{
    /*QString val = ui->listView_2->model()->data(index).toString();
    qDebug() << "val = " << val; //val
    qDebug() << "index = " << index; //index*/
}

void MainWindow::on_listView_2_doubleClicked(const QModelIndex &index)
{
    /*QString val = ui->listView_2->model()->data(index).toString();
    qDebug() << "val = " << val; //val
    qDebug() << "index = " << index; //index*/
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
