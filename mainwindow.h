// mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include <QVector>
#include <QString>
#include <QDateTime>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr,
        const QVector<double>& plot1_x_time = {},
        const QVector<double>& plot1_y_engine_load = {},
        const QVector<double>& plot2_x_engine_load = {},
        const QVector<double>& plot2_y_sfoc = {},
        const QVector<double>& plot3_x_time = {},
        const QVector<double>& plot3_y_sog = {},
        const QVector<double>& plot3_y_stw = {},
        const QVector<double>& plot4_x_sog = {},
        const QVector<double>& plot4_y_prop_power = {},
        const QVector<double>& plot5_x_wind_dir = {},
        const QVector<double>& plot5_y_wind_speed = {});

    ~MainWindow();

private:
    QCustomPlot* customPlot1;
    QCustomPlot* customPlot2;
    QCustomPlot* customPlot3;
    QCustomPlot* customPlot4;
    QCustomPlot* customPlot5;

    void setupPlot(QCustomPlot* plot, const QString& title, const QString& xAxisLabel, const QString& yAxisLabel);
    void setupDateTimeAxis(QCustomPlot* plot, const QVector<double>& xData, QCPAxis* axis);
    void markDayChanges(QCustomPlot* plot, const QVector<double>& xData);

};

#endif // MAINWINDOW_H