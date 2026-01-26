#include <iostream>
#include <string>
#include <regex>
#include <filesystem>
#include <vector>
namespace fs=std::filesystem;//并没有使用using的定义，直接用namespace newname=。。。的形式来简写了
int main(){
	std::cout<<fs::current_path()<<std::endl;
	std::cout<<fs::current_path().parent_path()<<std::endl;
	std::vector<fs::path> directories;
	std::regex txt_regex_folder("^[0-9]{2}[A-Za-z_]+");
	try{
		for (const auto& dir_entry : fs::directory_iterator(fs::current_path().parent_path())) {
			if( fs::is_directory(dir_entry) ){
				auto p = dir_entry.path();
				auto lastDirectory = p.filename().string();
				if( std::regex_match(lastDirectory,txt_regex_folder))		
					directories.push_back(p);
			}	
		}
		//使用R"()"的形式，在反斜杠不多的情况下，还不如直接用。
		std::regex cppfile_regex(R"(^([0-9A-Za-z_]+)\.cpp$)");
		for(const auto& folder : directories ){
			for(const auto& file_entry : fs::recursive_directory_iterator(folder))
			{	
				auto filename = file_entry.path().filename().string();
				if(fs::is_regular_file(file_entry) && std::regex_match(filename,cppfile_regex)){
					std::cout<<"合格的cpp文件是： "<<file_entry.path()<<std::endl;
				}
			}
		}
    }catch (fs::filesystem_error & e) {
		// 如果在遍历过程中发生错误，比如没有权限访问目录
		std::cerr << e.what() << std::endl;
	}
	
	std::regex re("a(a*)b");
    std::string target("aaab");
    std::smatch sm;
 
    std::cout << sm.size() << '\n';
 
    std::regex_match(target, sm, re);
    for(int i=0;i<sm.size();i++)
		std::cout << sm[i].str() << '\n';
}