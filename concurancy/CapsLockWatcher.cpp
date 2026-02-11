#include <fstream>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

constexpr const char* CAPSLOCK_FILE_PATH =
"/sys/class/leds/input4::capslock/brightness";


constexpr char INIT_STATE      = -1;
constexpr char PREV_INIT_STATE = -2;

char capsLockState = INIT_STATE;
bool flag = false;

void CapsLockWatcherCallable(std::mutex &mx, std::condition_variable &cv)
{
    std::cout << "Provider CallBack Thread is created\n";

    std::ifstream CapsLockFile(CAPSLOCK_FILE_PATH);
    if(!CapsLockFile.is_open()){
        std::cout << "The file can't be opened successfully\n";
        return;
    }

    char capsLockPrevState = PREV_INIT_STATE;

    std::unique_lock<std::mutex> ul(mx, std::defer_lock);

    while(1)
    {
        // ---- read file safely (no lock needed) ----
        CapsLockFile.clear();
        CapsLockFile.seekg(0);
        CapsLockFile >> capsLockPrevState;

        
        if(capsLockState != capsLockPrevState)
        {
            ul.lock();
            // update the shared resource
            capsLockState = capsLockPrevState;
            flag = true;               
            /*************************************** */
            ul.unlock();
            cv.notify_one();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void CapsLockWaiterCallable(std::mutex &mx, std::condition_variable &cv)
{
    std::cout << "Consumer CallBack Thread is created\n";

    std::unique_lock<std::mutex> ul(mx,std::defer_lock);

    while(1)
    {
        ul.lock();
        cv.wait(ul, []{ return flag; }); 
        ul.unlock();
        // read the shared resource
        flag = false;
        std::cout << "Caps Lock : " << ((capsLockState=='0') ? "OFF" : "ON") << std::endl;
        /********************************************************** */
    }
}

int main()
{
    std::mutex shared_mx;
    std::condition_variable cv;

    std::thread CapsLockProvider(CapsLockWatcherCallable,
                                 std::ref(shared_mx), std::ref(cv));

    std::thread CapsLockConsumer(CapsLockWaiterCallable,
                                 std::ref(shared_mx), std::ref(cv));

    CapsLockConsumer.join();
    CapsLockProvider.join();
}
