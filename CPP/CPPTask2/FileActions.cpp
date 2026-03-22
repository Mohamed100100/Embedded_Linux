
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <fcntl.h>
#include <initializer_list>
#include <stdlib.h>
#include <vector>


#include <string>
#include <iostream>


class FileActions {

    private :
    int *fd = nullptr;
    int *&fdRef = fd;
    std::vector<std::pair<std::string,int>> actions;
 

    public :
    // FileActions() = delete;
    
    FileActions(std::string copyPath ){
        fd = (int *)malloc(sizeof(int));       
        *fd = open(copyPath.data(),O_RDWR);
        if (*fd < 0) {
            std::cout << "Failed to open file: " << copyPath << "\n";
            free(fd);
            fd = NULL;
        }
    }

    FileActions( FileActions& obj){
        // fd = (int *)malloc(sizeof(int));
        fd = new int();
        *fd = *obj.fd;
        for (auto pairVal : obj.actions) {
            actions.push_back(pairVal);
        }
    }
    void registerActions(std::initializer_list<std::pair<std::string,int>> pairObjects){
        
        for (auto ptr = pairObjects.begin(); ptr != pairObjects.end(); ++ptr) {
            actions.push_back(*ptr);
        }
    }

    void executeActions(){
        for (auto pairVal : actions) {
            if(pairVal.first == "write"){
                ExecuteWrite(pairVal.second);
            }else{
                if(pairVal.first == "read"){
                    ExecuteRead(pairVal.second);
                }else{
                    if(pairVal.first == "close"){
                        ExecuteClose();
                    }else{
                        std::cout <<"Wrong action Name : " << pairVal.first << "\n";
                    }
                
                }
            }
        }
    }
    private : 
    void ExecuteWrite(int valueWritten){
        if(fd == nullptr){
            std::cout<< "Error while writing : NULL Pointer\n";
        }else{
            if(*fd < 0){
                std::cout<< "Error while writing : invalid file discriptor \n";
            }else{
                ssize_t written = write(*fd, (std::to_string(valueWritten)).data() , (std::to_string(valueWritten)).length());

                if (written == -1) {
                    std::cout << "Write failed!\n";
                }else{
                    std::cout << "Successfully write : "<< valueWritten << "\n";
                }
            }
	    
        }
    }
    private :
    void ExecuteRead(int bytes){
        if(fd == nullptr){
            std::cout<< "Error while writing : NULL Pointer\n";
        }else{
            if(*fd < 0){
                std::cout<< "Error while writing : invalid file discriptor \n";
            }else{
                std::string str;
                str.resize(bytes);
                ssize_t readBytes = read(*fd, &str[0] , bytes);

                if (readBytes == -1) {
                    std::cout << "Read failed!\n";
                }else{
                    std::cout << "Successfully Read\n";
                    std::cout << str << "\n";
                }
            }
	    
        }
    }

    private :
    void ExecuteClose(){
        if(fd == nullptr){
            std::cout<< "Error while writing : NULL Pointer\n";
        }else{
            if(*fd < 0){
                std::cout<< "Error while writing : invalid file discriptor \n";
            }else{

                ssize_t ret = close(*fd);

                if (ret == -1) {
                    std::cout << "Close failed!\n";
                }else{
                    std::cout << "Successfully Close\n";
                    *fd = -1;
                }
            }
        }
    }
    public:
    ~FileActions(){
        close(*fd);
        // free(fd);
        delete fd;
        fd = NULL;
        std::cout << "Destructor called \n";
    }
};


void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <file_path>\n";
    std::cout << "Example: " << programName << " test.txt\n";
}

int main(int argc, char* argv[]) {
    // Check if file path argument is provided
    if (argc != 2) {
        std::cerr << "Error: File path argument required!\n\n";
        printUsage(argv[0]);
        return 1;
    }

    std::string filePath = argv[1];
    std::cout << "Using file: " << filePath << "\n\n";

    // Test 1: Basic write operations
    std::cout << "=== Test 1: Write Operations ===\n";
    {
        FileActions fa(filePath);
        fa.registerActions({
            {"write", 123},
            {"write", 456},
            {"write", 789}
        });
        fa.executeActions();
    }
    std::cout << "\n";

    // Test 2: Read operations
    std::cout << "=== Test 2: Read Operations ===\n";
    {
        FileActions fa(filePath);
        fa.registerActions({
            {"read", 50}  // Read up to 50 bytes
        });
        fa.executeActions();
    }
    std::cout << "\n";

    // Test 3: Mixed operations
    std::cout << "=== Test 3: Mixed Operations ===\n";
    {
        FileActions fa(filePath);
        fa.registerActions({
            {"write", 999},
            {"read", 10},
            {"write", 111}
        });
        fa.executeActions();
    }
    std::cout << "\n";

    // Test 4: Close operation
    std::cout << "=== Test 4: Close Operation ===\n";
    {
        FileActions fa(filePath);
        fa.registerActions({
            {"write", 555},
            {"close",0},
            {"write", 666}  // This should fail after close
        });
        fa.executeActions();
    }
    std::cout << "\n";

    // Test 5: Invalid action
    std::cout << "=== Test 5: Invalid Action ===\n";
    {
        FileActions fa(filePath);
        fa.registerActions({
            {"invalid_action", 100}
        });
        fa.executeActions();
    }
    std::cout << "\n";

    std::cout << "All tests completed!\n";
    return 0;
}