// csvIntoColumns.h
#ifndef CSV_INTO_COLUMNS_H
#define CSV_INTO_COLUMNS_H

#include <vector>
#include <string>

/**
 * @brief Reads a CSV file and organizes its data into columns.
 *
 * Each inner vector in the returned std::vector<std::vector<std::string>>
 * represents a column from the CSV file. The first element of each column
 * vector will typically be its header.
 *
 * @param filename The path to the CSV file.
 * @param delimiter The character used to separate values in the CSV (default is ',').
 * @return A std::vector of std::vector<std::string> where each inner vector is a column.
 * Returns an empty outer vector if the file cannot be opened or is empty.
 */
std::vector<std::vector<std::string>> readCsv(const std::string& filename, char delimiter = ',');

/**
 * @brief Prints the CSV data that is organized by columns.
 *
 * This function assumes the input data is in a column-major format
 * (i.e., data[i] is the i-th column).
 *
 * @param columns A std::vector of std::vector<std::string> where each inner vector is a column.
 */
void printCsvData(const std::vector<std::vector<std::string>>& columns);

#endif // CSV_INTO_COLUMNS_H