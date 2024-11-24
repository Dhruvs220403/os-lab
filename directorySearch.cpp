#include <iostream>
#include <filesystem>
#include <string>
#include <iomanip>
#include <chrono>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

namespace fs = std::filesystem;
using namespace std;

// Function to print file attributes
void printFileAttributes(const fs::path& path) {
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == -1) {
        perror("stat");
        return;
    }

    // File type
    string type = fs::is_directory(path) ? "Directory" : "File";

    // Permissions
    string permissions;
    permissions += (fileStat.st_mode & S_IRUSR) ? "r" : "-";
    permissions += (fileStat.st_mode & S_IWUSR) ? "w" : "-";
    permissions += (fileStat.st_mode & S_IXUSR) ? "x" : "-";
    permissions += (fileStat.st_mode & S_IRGRP) ? "r" : "-";
    permissions += (fileStat.st_mode & S_IWGRP) ? "w" : "-";
    permissions += (fileStat.st_mode & S_IXGRP) ? "x" : "-";
    permissions += (fileStat.st_mode & S_IROTH) ? "r" : "-";
    permissions += (fileStat.st_mode & S_IWOTH) ? "w" : "-";
    permissions += (fileStat.st_mode & S_IXOTH) ? "x" : "-";

    // Timestamps
    auto ctime = chrono::system_clock::to_time_t(chrono::file_clock::from_time_t(fileStat.st_ctime));
    auto atime = chrono::system_clock::to_time_t(chrono::file_clock::from_time_t(fileStat.st_atime));
    auto mtime = chrono::system_clock::to_time_t(chrono::file_clock::from_time_t(fileStat.st_mtime));

    // Owner and group
    struct passwd* pw = getpwuid(fileStat.st_uid);
    struct group* gr = getgrgid(fileStat.st_gid);
    string owner = pw ? pw->pw_name : to_string(fileStat.st_uid);
    string group = gr ? gr->gr_name : to_string(fileStat.st_gid);

    // Print attributes
    cout << "Path: " << path << "\n"
         << "Type: " << type << "\n"
         << "Permissions: " << permissions << "\n"
         << "Owner: " << owner << "\n"
         << "Group: " << group << "\n"
         << "Creation Time: " << put_time(localtime(&ctime), "%F %T") << "\n"
         << "Last Access Time: " << put_time(localtime(&atime), "%F %T") << "\n"
         << "Last Modification Time: " << put_time(localtime(&mtime), "%F %T") << "\n"
         << "-------------------------------------------\n";
}

// Function to search for a file or directory recursively
void searchDirectory(const fs::path& directory, const string& target) {
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.path().filename() == target) {
            cout << "Found: " << entry.path() << "\n";
            printFileAttributes(entry.path());
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <directory> <target_name>\n";
        return 1;
    }

    fs::path directory = argv[1];
    string targetName = argv[2];

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        cerr << "Error: " << directory << " is not a valid directory.\n";
        return 1;
    }

    searchDirectory(directory, targetName);

    return 0;
}
