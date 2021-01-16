#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QFileDialog>
#include <exif.h>
#include <dbmanager.h>
#include <stdio.h>
#include <fstream>
#include<QtCore>
#include<QtGui>

#include <QSqlQueryModel>
#include <QSqlQuery>

#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

#include <QMessageBox>

#include <stdlib.h> //------------------------------------ MAY BE UNUSEFUL
#include <daylightclass.h>
#include <weatherclass.h>
#include <gmapclass.h>
#include <gcalendar.h>

#include <QWebEngineView>
#include <QWebEngineProfile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void extract_exif(QString imgPath);
    void extractDatetimeLatLongData(string s);

private slots:
    void on_treeView_clicked(const QModelIndex &index);

    void on_listView_clicked(const QModelIndex &index);

    void on_pushButton_clicked();

    void on_quickBrowse_clicked();

    void show_image_metadata();
    void show_image_result();
    void show_allComboBoxes();

    void on_listView_2_activated(const QModelIndex &index);

    void on_listView_2_clicked(const QModelIndex &index);

    void on_listView_2_doubleClicked(const QModelIndex &index);

    void on_pushButton_update_clicked();

    void on_pushButton_remImage_clicked();

    void updateListViewResult();
    void on_comboBox_make_currentIndexChanged(const QString &arg1);
    void on_comboBox_daylight_currentIndexChanged(const QString &arg1);

    void on_comboBox_weather_currentIndexChanged(const QString &arg1);

    void on_comboBox_location_currentIndexChanged(const QString &arg1);

    void on_comboBox_person_currentIndexChanged(const QString &arg1);

    void on_comboBox_event_currentIndexChanged(const QString &arg1);

    void on_pushButton_glogin_clicked();

    void on_pushButton_get_event_clicked();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *dirModel;
    QFileSystemModel *fileModel;
    //QStandardItemModel *model;
};
#endif // MAINWINDOW_H
