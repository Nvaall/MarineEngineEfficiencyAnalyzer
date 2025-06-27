#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // Required for std::transform


std::vector<float> convertStringVectorToFloatVector(const std::vector<std::string>& stringVec)
{
    std::vector<float> floatVec;
    floatVec.reserve(stringVec.size()); // Pre-allocate memory to avoid reallocations

    // Use std::transform to apply std::stof to each element
    std::transform(stringVec.begin(), stringVec.end(),
        std::back_inserter(floatVec),
        [](const std::string& str) {
            return std::stof(str); // Direct conversion, no error checks
        });
    return floatVec;
}