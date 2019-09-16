#ifndef _Path_Utils_H_
#define _Path_Utils_H_

#include <list>
#include <queue>

// 定义公共类型和方法

using list_type = std::vector<std::string>;
using pair_list = std::vector<std::pair<std::string, std::string>>;
using queue_type = std::queue<std::string, std::list<std::string>>;

// 判定目录是否有效
bool IsValidDirectory(const char* dir);

// 收集目录到列表中
bool ExtractDirectory(list_type& pathlist, const char* dir);

// 筛选指定目录下指定类型的文件到队列中
unsigned int FilterFile(queue_type& fileQueue, const std::string& dir, const list_type& suffixes);

// 给文件附加指定后缀，如果之前的后缀存在则替换，返回新的文件路径名
std::string AppointSuffix(const char * appPath, const char * suffix);

#endif // _Path_Utils_H_
