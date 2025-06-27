// main.cpp

#include "csvIntoColumns.h" 
#include "stringToFloatVector.h"
#include "mainwindow.h"
#include <QApplication>
#include <QVector>       // Required for QVector
#include <QDateTime>     // Required for QDateTime for timestamp parsing
#include <QDebug>        // Required for qDebug() for debugging output
#include <iostream>      // Required for std::cerr, std::endl
#include <algorithm>     // Required for std::min
#include <limits>        // Required for std::numeric_limits<float>::quiet_NaN()

int main(int argc, char* argv[])
{
    QApplication a(argc, argv); // Create the QApplication instance

    const std::string datapointsFilename = "Book1.csv";

    // Read the CSV data into a column-major format
    std::vector<std::vector<std::string>> dataPoints = readCsv(datapointsFilename);

    // Basic error checking if CSV reading failed or returned empty data
    if (dataPoints.empty()) {
        std::cerr << "Error: No data read from CSV or file not found. Exiting." << std::endl;
        return 1;
    }

    // Ensure the CSV has at least 11 columns as expected for your data
    if (dataPoints.size() < 11) {
        std::cerr << "Error: CSV file does not contain enough columns. Expected at least 11, got " << dataPoints.size() << ". Exiting." << std::endl;
        return 1;
    }

    // --- Data Extraction and Conversion ---
    // 1. Extract and convert timestamps for plot's X-axis
    QVector<double> plot_time_data; // Will hold QDateTime::toSecsSinceEpoch() values
    QVector<QString> timeUI_qstring_labels; // Will hold original QString labels (for debugging/optional use)

    // Check if the timestamp column has data beyond the header
    if (dataPoints[0].size() > 1) {
        for (size_t i = 0; i < dataPoints[0].size(); ++i) { // Start from 0 (no header)
            QString dateTimeString = QString::fromStdString(dataPoints[0][i]).trimmed();

            QDateTime dateTime = QDateTime::fromString(dateTimeString, "dd/MM/yyyy HH:mm");

            if (!dateTime.isValid()) {
                qDebug() << "ERROR: Failed to parse datetime string:" << dateTimeString;
                qDebug() << "  Expected format: dd/MM/yyyy HH:mm (e.g., 08/03/2021 10:29)";
                plot_time_data.push_back(0.0); // Push 0.0 for invalid dates (epoch start)
            }
            else {
                plot_time_data.push_back(dateTime.toSecsSinceEpoch()); 
            }
            timeUI_qstring_labels.push_back(dateTimeString); // Store for labels parameter
        }
    }
    else {
        std::cerr << "Warning: Time column (Column 0) is empty or only contains a header. No X-axis data for plot 1." << std::endl;
    }

    // Declare and populate std::vector<std::string> for each column, skipping the header (index 0)
    std::vector<std::string> SOG_str, STW_str, PropPower_str, PropRev_str, FOC_str,
        Tmean_str, Trim_str, ShipHeadingDeg_str, RelWindDirDeg_str, RelWindSpeed_str;

    // Helper lambda for cleaner column extraction
    auto extractColumn = [&](std::vector<std::string>& dest, size_t colIdx) {
        if (dataPoints.size() > colIdx && dataPoints[colIdx].size() > 1) {
            for (size_t i = 0; i < dataPoints[colIdx].size(); ++i) { // Start from 0 (no header)
                dest.push_back(dataPoints[colIdx][i]);
            }
        }
        else {
            std::cerr << "Warning: Column " << colIdx + 1 << " is empty or only contains a header." << std::endl;
        }
        };

    extractColumn(SOG_str, 1);           // SOG (Column 2)
    extractColumn(STW_str, 2);           // STW (Column 3)
    extractColumn(PropPower_str, 3);     // PropPower (Column 4)
    extractColumn(PropRev_str, 4);       // PropRev (Column 5)
    extractColumn(FOC_str, 5);           // FOC (Column 6)
    extractColumn(Tmean_str, 6);         // Tmean (Column 7)
    extractColumn(Trim_str, 7);          // Trim (Column 8)
    extractColumn(ShipHeadingDeg_str, 8); // ShipHeadingDeg (Column 9)
    extractColumn(RelWindDirDeg_str, 9); // RelWindDirDeg (Column 10)
    extractColumn(RelWindSpeed_str, 10); // RelWindSpeed (Column 11)


    // Convert string vectors to float vectors
    std::vector<float> SOG_float = convertStringVectorToFloatVector(SOG_str);
    std::vector<float> STW_float = convertStringVectorToFloatVector(STW_str);
    std::vector<float> PropPower_float = convertStringVectorToFloatVector(PropPower_str);
    std::vector<float> PropRev_float = convertStringVectorToFloatVector(PropRev_str);
    std::vector<float> FOC_float = convertStringVectorToFloatVector(FOC_str);
    std::vector<float> Tmean_float = convertStringVectorToFloatVector(Tmean_str);
    std::vector<float> Trim_float = convertStringVectorToFloatVector(Trim_str);
    std::vector<float> ShipHeadingDeg_float = convertStringVectorToFloatVector(ShipHeadingDeg_str);
    std::vector<float> RelWindDirDeg_float = convertStringVectorToFloatVector(RelWindDirDeg_str);
    std::vector<float> RelWindSpeed_float = convertStringVectorToFloatVector(RelWindSpeed_str);

    // Calculate Engine Load % (MCR = 9930 kW)
    std::vector<float> EngineLoad_float;
    float MCR = 9930.0f;
    EngineLoad_float.reserve(PropPower_float.size());
    for (size_t i = 0; i < PropPower_float.size(); ++i) {
        EngineLoad_float.push_back(PropPower_float[i] / MCR * 100.0f); // Convert to percentage
    }

    // Calculate SFOC in gr/kWh [FOC/PropPower]
    std::vector<float> SFOC_float;
    SFOC_float.reserve(PropPower_float.size());
    size_t common_data_size = std::min(PropPower_float.size(), FOC_float.size());

    for (size_t i = 0; i < common_data_size; ++i) {
        if (PropPower_float[i] != 0.0f) {
            SFOC_float.push_back((FOC_float[i] * 1000000.0f) / 24.0f / PropPower_float[i]);
        }
        else {
            SFOC_float.push_back(std::numeric_limits<float>::quiet_NaN()); // Use NaN for undefined values
        }
    }


    // --- Prepare data for QCustomPlot (QVector<double>) for all 5 plots ---
    int data_points_count = plot_time_data.size(); // Use this as the common size

    // Plot 1: Engine Load Over Time (EngineLoad vs Time)
    QVector<double> plot1_y_engine_load(data_points_count); 
    for (int i = 0; i < data_points_count; ++i) {
        plot1_y_engine_load[i] = (i < EngineLoad_float.size()) ? static_cast<double>(EngineLoad_float[i]) : 0.0;
    }

    // Plot 2: SFOC vs. Engine Load
    QVector<double> plot2_x_engine_load(data_points_count);
    QVector<double> plot2_y_sfoc(data_points_count);
    for (int i = 0; i < data_points_count; ++i) {
        plot2_x_engine_load[i] = (i < EngineLoad_float.size()) ? static_cast<double>(EngineLoad_float[i]) : 0.0;
        plot2_y_sfoc[i] = (i < SFOC_float.size()) ? static_cast<double>(SFOC_float[i]) : 0.0;
    }

    // Plot 3: Speed Over Ground (SOG) & Speed Through Water (STW) Over Time
    QVector<double> plot3_y_sog(data_points_count);
    QVector<double> plot3_y_stw(data_points_count);
    for (int i = 0; i < data_points_count; ++i) {
        plot3_y_sog[i] = (i < SOG_float.size()) ? static_cast<double>(SOG_float[i]) : 0.0;
        plot3_y_stw[i] = (i < STW_float.size()) ? static_cast<double>(STW_float[i]) : 0.0;
    }

    // Plot 4: Hull & Propeller Performance (SOG vs PropPower)
    QVector<double> plot4_x_sog(data_points_count);
    QVector<double> plot4_y_prop_power(data_points_count);
    for (int i = 0; i < data_points_count; ++i) {
        plot4_x_sog[i] = (i < SOG_float.size()) ? static_cast<double>(SOG_float[i]) : 0.0;
        plot4_y_prop_power[i] = (i < PropPower_float.size()) ? static_cast<double>(PropPower_float[i]) : 0.0;
    }

    // Plot 5: Environmental Factors: Wind Speed & Direction
    QVector<double> plot5_x_wind_dir(data_points_count);
    QVector<double> plot5_y_wind_speed(data_points_count);
    for (int i = 0; i < data_points_count; ++i) {
        plot5_x_wind_dir[i] = (i < RelWindDirDeg_float.size()) ? static_cast<double>(RelWindDirDeg_float[i]) : 0.0;
        plot5_y_wind_speed[i] = (i < RelWindSpeed_float.size()) ? static_cast<double>(RelWindSpeed_float[i]) : 0.0;
    }

    // Create an instance of our MainWindow, passing all the prepared data
    MainWindow w(nullptr,
        plot_time_data, plot1_y_engine_load, // Plot 1 data
        plot2_x_engine_load, plot2_y_sfoc,                     // Plot 2 data
        plot_time_data, plot3_y_sog, plot3_y_stw,             // Plot 3 data (using common time data)
        plot4_x_sog, plot4_y_prop_power,                      // Plot 4 data
        plot5_x_wind_dir, plot5_y_wind_speed                 // Plot 5 data
        );
    w.show(); // Display the main window

    return a.exec(); // Start the Qt event loop
}