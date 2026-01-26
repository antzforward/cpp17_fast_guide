#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <chrono>
#include <filesystem>
/*
特定场景：
日志记录和监控：如果你正在实现一个日志系统，你可能想要在另一个线程中异步写入日志，而且不关心写操作何时完成。
超时监管器：在某些应用程序中，你可能想要创建一个监视线程，以确保另一个操作不会超出预定时间。如果操作超时，这个监视线程可以采取相应的行动，例如取消操作或重启任务。
后台任务：比如定期清理缓存、检查更新或其他周期性任务，这些都可以作为分离的线程运行。
*/
void async_log(const std::string& message) {
    try {
        // 尝试打开文件，并附加到它
        std::ofstream log_file("log.txt", std::ios_base::app);
        
        if (!log_file.is_open()) {
            throw std::runtime_error("Unable to open log file.");
        }

        // 写入消息到文件
        log_file << message << std::endl;

        // 显式关闭文件
        log_file.close();
    } catch (const std::exception& e) {
        // 如果有任何错误发生，捕获并打印异常信息
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
}

int main() {
	std::cout << "The current working directory is " << std::filesystem::current_path() << std::endl;
	
    // 创建一个线程，将消息异步写入日志
    std::thread log_thread(async_log, "This is an asynchronous log message.");
    
    // 分离线程，让它独立运行
    log_thread.detach();//分离之后就没有办法退出了，只能由操作系统来处理了。

    // ... 主线程继续执行其他任务 ...

    // 注意：这里没有等待日志线程完成，我们假设它会正确地执行并在适当的时候结束
	
	// 确保主线程等待一段时间，以防程序过早退出//过早退出真的不行啊。
    std::this_thread::sleep_for(std::chrono::seconds(1));//用sleep机制来做不好，用其他的同步机制（不过都分离了，同步也没意义啊）

    std::cout << "Main thread ends." << std::endl;
    
    return 0;  // 主线程结束
}
/* 
注意事项：主线程的资源要隔离开！
需要注意的是，使用 detach 要非常小心，因为一旦线程被分离，你就失去了对它的控制，如果分离的线程访问了它不应该访问的资源（比如主线程已经释放的资源），就可能导致未定义行为，例如访问悬空指针或引用，甚至数据竞争。所以，在使用 detach 之前，务必确保线程的生命周期管理得当，并且所有资源的访问都是安全的。
*/