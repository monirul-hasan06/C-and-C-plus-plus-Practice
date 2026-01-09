#include <iostream>
#include <fstream>

class FileHandler {
private:
    std::fstream file;
    std::string filename;
public:
    FileHandler(const std::string& fname) : filename(fname) {
        std::cout << "Constructor: Opening file " << filename << std::endl;
        file.open(filename, std::ios::in | std::ios::out | std::ios::app);
    }
    ~FileHandler() {
        if (file.is_open()) {
            std::cout << "Destructor: Closing file " << filename << std::endl;
            file.close();
        }
    }
    // ... other methods ...
};

// Function that returns a FileHandler object
FileHandler make_file_handler(const std::string& fname) {
    FileHandler temp(fname);
    return temp;
}