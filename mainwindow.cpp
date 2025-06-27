// mainwindow.cpp
#include "mainwindow.h"
#include <QGridLayout>      // For grid layout
#include <QWidget>       // For setting a central widget
#include <QSharedPointer> // For QSharedPointer
#include <QDateTime>     // For QDateTime operations
#include <QPalette>      // For setting background color
#include <QColor>        // For QColor

// Constructor receives all plot data
MainWindow::MainWindow(QWidget* parent,
    const QVector<double>& plot1_x_time,
    const QVector<double>& plot1_y_engine_load,
    const QVector<double>& plot2_x_engine_load,
    const QVector<double>& plot2_y_sfoc,
    const QVector<double>& plot3_x_time,
    const QVector<double>& plot3_y_sog,
    const QVector<double>& plot3_y_stw,
    const QVector<double>& plot4_x_sog,
    const QVector<double>& plot4_y_prop_power,
    const QVector<double>& plot5_x_wind_dir,
    const QVector<double>& plot5_y_wind_speed)
    : QMainWindow(parent)
{
    setWindowTitle("Marine Engine Efficiency Analyzer");
    setMinimumSize(1200, 900);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QPalette pal = centralWidget->palette();
    pal.setColor(QPalette::Window, QColor(25, 25, 112)); // MidnightBlue - This color will show *between* plots
    centralWidget->setAutoFillBackground(true);
    centralWidget->setPalette(pal); // Applies pal color to centralWidget

    QGridLayout* mainLayout = new QGridLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // --- Plot 1: Engine Load Over Time ---
    customPlot1 = new QCustomPlot(this);
    mainLayout->addWidget(customPlot1, 0, 0, 2, 1);

    // Add graph for Engine Load, using the default yAxis1
    customPlot1->addGraph();
    customPlot1->graph(0)->setName("Engine Load"); // Name the first graph (for the legend later)
    customPlot1->graph(0)->setPen(QPen(QColor(0, 100, 0))); // Dark Green for Engine Load line
    customPlot1->graph(0)->setData(plot1_x_time, plot1_y_engine_load);

    // Setup plot common properties with the new title and Y-axis label
    setupPlot(customPlot1, "Engine Load Over Time", "Time", "Engine Load (%)");

    setupDateTimeAxis(customPlot1, plot1_x_time, customPlot1->xAxis);

    customPlot1->rescaleAxes();

    markDayChanges(customPlot1, plot1_x_time);

    // Legend will show name of graph"
    customPlot1->legend->setVisible(true);
    customPlot1->legend->setBrush(QBrush(QColor(255, 255, 255, 150))); // Semi-transparent white background for legend
    customPlot1->legend->setTextColor(Qt::black); // Black text for legend on white background

    customPlot1->replot();

    // --- Plot 2: SFOC vs. Engine Load ---
    customPlot2 = new QCustomPlot(this);
    mainLayout->addWidget(customPlot2, 0, 1, 1, 1);
    customPlot2->addGraph();
    setupPlot(customPlot2, "SFOC vs. Engine Load", "Engine Load (%)", "SFOC (gr/kWh)");
    customPlot2->graph(0)->setData(plot2_x_engine_load, plot2_y_sfoc);
    customPlot2->graph(0)->setPen(QPen(QColor(255, 69, 0))); // Orange Red
    customPlot2->graph(0)->setLineStyle(QCPGraph::lsNone);
    customPlot2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    customPlot2->rescaleAxes();
    customPlot2->yAxis->setRange(50, 300);
    customPlot2->replot();


    // --- Plot 3: SOG & STW Over Time ---
    customPlot3 = new QCustomPlot(this);
    mainLayout->addWidget(customPlot3, 1, 1, 1, 1);
    customPlot3->addGraph();
    customPlot3->graph(0)->setName("SOG (kn)");
    customPlot3->graph(0)->setPen(QPen(QColor(255, 10, 0))); // Dark Red
    customPlot3->graph(0)->setData(plot3_x_time, plot3_y_sog);
    customPlot3->addGraph();
    customPlot3->graph(1)->setName("STW (kn)");
    customPlot3->graph(1)->setPen(QPen(QColor(139, 69, 19))); // Saddle Brown
    customPlot3->graph(1)->setData(plot3_x_time, plot3_y_stw);
    setupPlot(customPlot3, "Speed Performance Over Time", "Time", "Speed (kn)");
    setupDateTimeAxis(customPlot3, plot3_x_time, customPlot3->xAxis);
    customPlot3->rescaleAxes();
    markDayChanges(customPlot3, plot3_x_time);
    customPlot3->legend->setVisible(true);
    customPlot3->legend->setBrush(QBrush(QColor(255, 255, 255, 150)));
    customPlot3->legend->setTextColor(Qt::black);
    customPlot3->replot();

    // --- Plot 4: Hull & Propeller Performance ---
    customPlot4 = new QCustomPlot(this);
    mainLayout->addWidget(customPlot4, 2, 0, 1, 1);
    customPlot4->addGraph();
    setupPlot(customPlot4, "Hull & Propeller Performance", "Speed Over Ground (kn)", "Propeller Power (kW)");
    customPlot4->graph(0)->setData(plot4_x_sog, plot4_y_prop_power);
    customPlot4->graph(0)->setPen(QPen(QColor(65, 105, 225))); // Royal Blue
    customPlot4->graph(0)->setLineStyle(QCPGraph::lsNone);
    customPlot4->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    customPlot4->rescaleAxes();
    customPlot4->replot();

    // --- Plot 5: Relative Wind Conditions ---
    customPlot5 = new QCustomPlot(this);
    mainLayout->addWidget(customPlot5, 2, 1, 1, 1);
    customPlot5->addGraph();
    setupPlot(customPlot5, "Relative Wind Conditions", "Relative Wind Direction (deg)", "Relative Wind Speed (m/s)");
    customPlot5->graph(0)->setData(plot5_x_wind_dir, plot5_y_wind_speed);
    customPlot5->graph(0)->setPen(QPen(QColor(128, 0, 128))); // Purple
    customPlot5->graph(0)->setLineStyle(QCPGraph::lsNone);
    customPlot5->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 7));
    customPlot5->rescaleAxes();
    customPlot5->replot();

    centralWidget->setLayout(mainLayout);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupPlot(QCustomPlot* plot, const QString& title, const QString& xAxisLabel, const QString& yAxisLabel)
{

    plot->setBackground(Qt::white); // Plot background is white

    // Set axis lines, ticks, and labels to black for visibility on white background
    plot->xAxis->setBasePen(QPen(Qt::black, 1));
    plot->yAxis->setBasePen(QPen(Qt::black, 1));
    plot->xAxis->setTickPen(QPen(Qt::black, 1));
    plot->yAxis->setTickPen(QPen(Qt::black, 1));
    plot->xAxis->setSubTickPen(QPen(Qt::black, 1));
    plot->yAxis->setSubTickPen(QPen(Qt::black, 1));

    plot->xAxis->setTickLabelColor(Qt::black);
    plot->yAxis->setTickLabelColor(Qt::black);
    plot->xAxis->setLabelColor(Qt::black);
    plot->yAxis->setLabelColor(Qt::black);

    // Set grid lines to light gray and dotted
    plot->xAxis->grid()->setPen(QPen(QColor(192, 192, 192), 1, Qt::DotLine));
    plot->yAxis->grid()->setPen(QPen(QColor(192, 192, 192), 1, Qt::DotLine));
    plot->xAxis->grid()->setSubGridVisible(false);
    plot->yAxis->grid()->setSubGridVisible(false);

    // Set plot title
    if (plot->plotLayout()->element(0, 0) == nullptr || !dynamic_cast<QCPTextElement*>(plot->plotLayout()->element(0, 0))) {
        plot->plotLayout()->insertRow(0);
        QCPTextElement* titleElement = new QCPTextElement(plot);
        plot->plotLayout()->addElement(0, 0, titleElement);
    }
    QCPTextElement* currentTitleElement = dynamic_cast<QCPTextElement*>(plot->plotLayout()->element(0, 0));
    if (currentTitleElement) {
        currentTitleElement->setText(title);
        currentTitleElement->setFont(QFont("sans", 12, QFont::Bold));
        currentTitleElement->setTextColor(Qt::black); // Title color is black
    }

    plot->xAxis->setLabel(xAxisLabel);
    plot->yAxis->setLabel(yAxisLabel);

    // Enable general plot interactions
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void MainWindow::setupDateTimeAxis(QCustomPlot* plot, const QVector<double>& xData, QCPAxis* axis)
{
    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeFormat("dd/MM/yyyy\nHH:mm");
    axis->setTicker(dateTimeTicker);
    axis->setTickLabelRotation(0);
    axis->setTickLabelFont(QFont(font().family(), 8));
}

void MainWindow::markDayChanges(QCustomPlot* plot, const QVector<double>& xData)
{
    if (xData.isEmpty()) {
        return;
    }

    QDateTime currentDay = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(xData.first())).date().startOfDay();

    for (int i = 0; i < xData.size(); ++i)
    {
        QDateTime pointDateTime = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(xData[i]));

        if (pointDateTime.date() > currentDay.date())
        {
            QDateTime midnightOfNewDay = pointDateTime.date().startOfDay();
            QCPItemLine* dayLine = new QCPItemLine(plot);
            dayLine->setPen(QPen(Qt::red, 1, Qt::DashLine));
            dayLine->start->setAxes(plot->xAxis, plot->yAxis);
            dayLine->end->setAxes(plot->xAxis, plot->yAxis);
            dayLine->start->setCoords(midnightOfNewDay.toSecsSinceEpoch(), plot->yAxis->range().lower);
            dayLine->end->setCoords(midnightOfNewDay.toSecsSinceEpoch(), plot->yAxis->range().upper);
            QCPItemText* dayLabel = new QCPItemText(plot);
            dayLabel->position->setParentAnchor(dayLine->start);
            dayLabel->position->setCoords(5, 15);
            dayLabel->setText(midnightOfNewDay.date().toString("MMM dd"));
            dayLabel->setFont(QFont(font().family(), 9, QFont::Bold));
            dayLabel->setColor(Qt::red);
            dayLabel->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            currentDay = midnightOfNewDay;
        }
    }
}