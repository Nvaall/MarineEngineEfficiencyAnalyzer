// csvIntoColumns.h
#ifndef CSV_INTO_COLUMNS_H
#define CSV_INTO_COLUMNS_H

#include <vector>
#include <string>

// Reads a CSV file and organizes its data into columns.
 

std::vector<std::vector<std::string>> readCsv(const std::string& filename, char delimiter = ',');

// Prints the CSV data that is organized by columns.

void printCsvData(const std::vector<std::vector<std::string>>& columns);

#endif // CSV_INTO_COLUMNS_H