#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <memory>

using namespace std;
namespace fs = std::filesystem;

struct Node {
    string name;
    bool is_directory;
    string permissions;
    size_t size;
    vector<shared_ptr<Node>> children;

    Node(const string& name, bool is_directory, const string& permissions, size_t size)
        : name(name), is_directory(is_directory), permissions(permissions), size(size) {}
};

class DirectoryTree {
public:
    explicit DirectoryTree(const fs::path& root) : root_path(root) {
        if (!fs::exists(root)) {
            throw runtime_error("Directory does not exist: " + root.string());
        }
        build_tree(root, root_node);
    }

    void print_tree(bool show_all, bool show_details, int level = 0) {
        print_node(root_node, show_all, show_details, level);
    }

private:
    fs::path root_path;
    shared_ptr<Node> root_node = make_shared<Node>(root_path.filename().string(), true, "-", 0);

    void build_tree(const fs::path& path, shared_ptr<Node>& node) {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_directory()) {
                auto child = make_shared<Node>(
                    entry.path().filename().string(), true, get_permissions(entry.path()), 0);
                build_tree(entry.path(), child);
                node->children.push_back(child);
            } else {
                auto child = make_shared<Node>(
                    entry.path().filename().string(), false, get_permissions(entry.path()), fs::file_size(entry.path()));
                node->children.push_back(child);
            }
        }
    }

    string get_permissions(const fs::path& path) {
        fs::perms p = fs::status(path).permissions();
        string permissions;

        permissions += (p & fs::perms::owner_read)  ? "r" : "-";
        permissions += (p & fs::perms::owner_write) ? "w" : "-";
        permissions += (p & fs::perms::owner_exec)  ? "x" : "-";
        permissions += (p & fs::perms::group_read)  ? "r" : "-";
        permissions += (p & fs::perms::group_write) ? "w" : "-";
        permissions += (p & fs::perms::group_exec)  ? "x" : "-";
        permissions += (p & fs::perms::others_read) ? "r" : "-";
        permissions += (p & fs::perms::others_write) ? "w" : "-";
        permissions += (p & fs::perms::others_exec) ? "x" : "-";

        return permissions;
    }

    void print_node(const shared_ptr<Node>& node, bool show_all, bool show_details, int level) {
        if (!show_all && node->name.find('.') == 0) return;

        for (int i = 0; i < level; ++i) cout << "  ";
        if (show_details) {
            cout << (node->is_directory ? "[DIR] " : "[FILE] ") << node->name 
                 << " (" << node->size << " bytes, " << node->permissions << ")\n";
        } else {
            cout << node->name << "\n";
        }

        for (const auto& child : node->children) {
            print_node(child, show_all, show_details, level + 1);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <directory_path> [-a] [-d]\n";
        return 1;
    }

    fs::path directory_path = argv[1];
    bool show_all = false;
    bool show_details = false;

    for (int i = 2; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-a") show_all = true;
        if (arg == "-d") show_details = true;
    }

    try {
        DirectoryTree tree(directory_path);
        tree.print_tree(show_all, show_details);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
// run like this -> ./directory_tree /home/user -a -d