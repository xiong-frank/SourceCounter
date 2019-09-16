#ifndef _Analyze_Thread_H_
#define _Analyze_Thread_H_

#include <mutex>

// 分析线程类
class AnalyzeThread
{
public:

    // 定义行类型
    enum LineType
    {
        UnknowLine      = 0x00,     // 未知类型
        EmptyLine       = 0x01,     // 空白行
        EffectiveLine   = 0x02,     // 有效行
        CommentLine     = 0x04,     // 注释行

        TypeMask        = 0x0f      // 类型掩码
    };

    // 定义行状态
    enum class LineMode : unsigned char
    {
        Nothing,                    // 无状态
        Quoting,                    // 引用中
        Annotating                  // 注释中
    };

private:

    queue_type m_FileQueue;         // 文件队列
    std::mutex m_Mutex;             // 线程锁

    // 分析参数对象
    struct _analyze_arg {
        LineMode _lm{ LineMode::Nothing };
        unsigned int _arg{ 0 };
    };

    // 禁止拷贝
    AnalyzeThread(const AnalyzeThread&) = delete;
    AnalyzeThread& operator=(const AnalyzeThread&) = delete;

    // 取一个文件
    bool _PickFile(std::string& file);

    // 分析线程
    void _Analyze(ReportList& reports, const list_type& singles, const pair_list& multiples);

    // 分析一个文件并返回统计报告
    static FileReport AnalyzeFile(const std::string& file, const list_type& singles, const pair_list& multiples);
    
    // 对应模式下的分析方法，下述3个函数将相互配合调用共同分析一行数据的类型。
    static unsigned int AnalyzeByNothing(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
    static unsigned int AnalyzeByQuoting(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
    static unsigned int AnalyzeByAnnotating(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);

public:

    // 默认构造
    AnalyzeThread() = default;

    const queue_type& GetFileQueue() const { return m_FileQueue; }
    unsigned int GetFileCount() const { return m_FileQueue.size(); }

    // 从指定目录中提取指定后缀名的源文件到队列中
    unsigned int ExtractFile(const std::string& fromPath, const list_type& suffixes);

    // 启动线程
    bool Start(unsigned int nThread, ReportList& reports, const list_type& singles, const pair_list& multiples);

};

#endif // _Analyze_Thread_H_
