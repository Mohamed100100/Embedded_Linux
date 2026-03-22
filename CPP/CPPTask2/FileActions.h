#ifndef FILEACTIONS_H
#define FILEACTIONS_H

#include <string>
#include <vector>
#include <utility>
#include <initializer_list>

class FileActions {
private:
    int *fd = nullptr;
    int *&fdRef = fd;
    std::vector<std::pair<std::string, int>> actions;

    void ExecuteWrite(int valueWritten);
    void ExecuteRead(int bytes);
    void ExecuteClose();

public:
    FileActions() = delete;
    FileActions(std::string copyPath);
    FileActions(FileActions& obj);
    
    void registerActions(std::initializer_list<std::pair<std::string, int>> pairObjects);
    void executeActions();
    
    ~FileActions();
};

#endif // FILEACTIONS_H