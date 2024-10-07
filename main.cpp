#include <opencv2/opencv.hpp>
#include <windows.h>  
#include <dirent.h>   
#include <cstring>    
#include <fstream>    
#include <iostream>   
#include "augmentation.h"  

using namespace std;
using namespace cv;

// Create a directory if it does not exist
void createDirectory(const string& path) {
    if (CreateDirectoryA(path.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
        cout << "Directory created or already exists: " << path << endl;
    }
    else {
        cerr << "Error creating directory: " << path << endl;
    }
}

// Check file extension
bool hasValidExtension(const string& filename, const string& extension) {
    size_t dot_pos = filename.find_last_of(".");
    if (dot_pos == string::npos) return false;
    string file_ext = filename.substr(dot_pos);
    return (file_ext == extension);
}

// Getting the list of files with a certain extensionvector<string> getFilesInDirectory(const string& path, const string& extension) {
    vector<string> files;
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            string file_name = ent->d_name;
            if (file_name != "." && file_name != ".." && hasValidExtension(file_name, extension)) {
                files.push_back(path + "/" + file_name);
            }
        }
        closedir(dir);
    }
    else {
        cerr << "Could not open directory: " << path << endl;
    }
    return files;
}

// Function for copying .txt files
void copyTxtFile(const string& src_image, const string& dst_image) {
    string src_txt = src_image.substr(0, src_image.find_last_of(".")) + ".txt";
    string dst_txt = dst_image.substr(0, dst_image.find_last_of(".")) + ".txt";

    ifstream src_file(src_txt, ios::binary);
    ofstream dst_file(dst_txt, ios::binary);

    if (src_file && dst_file) {
        dst_file << src_file.rdbuf();
        cout << "Copied: " << src_txt << " to " << dst_txt << endl;
    }
    else {
        cerr << "Error copying text file: " << src_txt << " or opening destination: " << dst_txt << endl;
    }
}

int main() {
    string input_folder, output_folder, extension;
    int num_augmentations;

    // CLI Input
    cout << "Input folder path: ";
    getline(cin, input_folder);

    cout << "Output folder path: ";
    getline(cin, output_folder);

    cout << "Augmentations var: ";
    cin >> num_augmentations;

    cout << "Extension (.png, .jpg, etc.): ";
    cin >> extension;

    // Create an output folder if it does not exist
    createDirectory(output_folder);

    // Get a list of all files with the desired extension in the folder
    vector<string> input_files = getFilesInDirectory(input_folder, extension);

    // Go through all the images in the folder
    for (const string& input_path : input_files) {
        size_t dot_pos = input_path.find_last_of(".");
        size_t slash_pos = input_path.find_last_of("/");
        string file_name = input_path.substr(slash_pos + 1, dot_pos - slash_pos - 1);  // Имя без расширения
        string file_extension = input_path.substr(dot_pos);  // Расширение

        // Load img
        Mat src = imread(input_path);
        if (src.empty()) {
            cerr << "Failed to load image: " << input_path << endl;
            continue;
        }

        // perform multiple augmentations for each image
        for (int i = 1; i <= num_augmentations; ++i) {
            Mat augmented = augmentImage(src);

            // New Name
            string output_path = output_folder + "/" + file_name + "_" + to_string(i) + file_extension;

            imwrite(output_path, augmented);

            // Copy txt
            copyTxtFile(input_path, output_path);

            cout << "Saved: " << output_path << endl;
        }
    }

    return 0;
}
