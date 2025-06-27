// csvIntoColumns.cpp

#include "csvIntoColumns.h" 
#include <fstream>   
#include <sstream>   
#include <iostream>  
#include <algorithm> 

// Function to read the CSV and return data organized by columns
std::vector<std::vector<std::string>> readCsv(const std::string& filename, char delimiter)
{
    // Temporary storage for data in row-major format (how it's read from file)
    std::vector<std::vector<std::string>> row_major_data;
    std::ifstream file(filename); // Attempt to open the file

    // Check if the file was successfully opened
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return {}; // Return an empty vector indicating failure
    }

    std::string line;
    size_t max_cols = 0; // Keep track of the maximum number of columns found in any row

    // Read file line by line
    while (std::getline(file, line))
    {
        std::vector<std::string> row; // Stores cells for the current row
        std::string current_cell_content;

        // Parse each character in the line to extract cells
        for (char c : line) {
            if (c == delimiter) {
                // If delimiter found and not inside quotes, add accumulated cell content
                row.push_back(current_cell_content);
                current_cell_content.clear(); // Reset for next cell
            }
            else {
                current_cell_content += c; // Accumulate characters for current cell
            }
        }
        row.push_back(current_cell_content); // Add the last cell of the line

        // Add the parsed row to the row_major_data, if it's not entirely empty
        if (!row.empty() || !line.empty()) {
            row_major_data.push_back(row);
            max_cols = std::max(max_cols, row.size()); // Update max_cols
        }
    }

    // Now, transpose the row-major data into column-major format.
    if (row_major_data.empty()) {
        return {}; // No data was read
    }

    // Initialize the final column_major_data vector with the correct number of columns
    // Each inner vector will store elements for one column
    std::vector<std::vector<std::string>> column_major_data(max_cols);

    // Populate column_major_data by transposing row_major_data
    for (const auto& row : row_major_data) {
        for (size_t col_idx = 0; col_idx < max_cols; ++col_idx) {
            // If the current row has a value for this column index, use it
            if (col_idx < row.size()) {
                column_major_data[col_idx].push_back(row[col_idx]);
            }
            else {
                // Otherwise, pad with an empty string to maintain column length consistency
                column_major_data[col_idx].push_back("");
            }
        }
    }

    return column_major_data; // Return the data organized by columns
}

// Function to print the CSV data that is organized by columns
void printCsvData(const std::vector<std::vector<std::string>>& columns) {
    if (columns.empty()) {
        std::cout << "No data to display (columns vector is empty).\n";
        return;
    }

    // Find the maximum number of rows (height) across all columns for consistent printing
    size_t num_rows = 0;
    for (const auto& col : columns) {
        num_rows = std::max(num_rows, col.size());
    }

    // Iterate through logical rows for printing
    for (size_t r = 0; r < num_rows; ++r) {
        // Iterate through each column
        for (size_t c = 0; c < columns.size(); ++c) {
            // Check if the current column has a value for this row index
            if (r < columns[c].size()) {
                std::cout << columns[c][r];
            }
            else {
                std::cout << ""; // Print empty for missing values
            }

            // Print a separator between columns, but not after the last one
            if (c < columns.size() - 1) {
                std::cout << " | ";
            }
        }
        std::cout << std::endl; // New line after each logical row
    }
}