#ifndef _File_Report_H_
#define _File_Report_H_

// 文件统计报告对象
class FileReport
{
private:

    std::string m_FilePath;         // 文件路径
    unsigned int m_Total;           // 物理行数
    unsigned int m_Empty;           // 空白行数
    unsigned int m_Effective;       // 有效行数
    unsigned int m_Comment;         // 注释行数
    unsigned int m_SpendTime;       // 时间开销

public:

    FileReport() = default;

    explicit FileReport(const std::string& path)
        : m_FilePath(path), m_Total(0), m_Empty(0), m_Effective(0), m_Comment(0), m_SpendTime(0)
    { }

    FileReport(const FileReport& r)
        : m_FilePath(r.m_FilePath)
        , m_Total(r.m_Total)
        , m_Empty(r.m_Empty)
        , m_Effective(r.m_Effective)
        , m_Comment(r.m_Comment)
        , m_SpendTime(r.m_SpendTime)
    { }

    FileReport& operator=(const FileReport& r)
    {
        if (this != &r)
        {
            m_FilePath  = r.m_FilePath;
            m_Total     = r.m_Total;
            m_Empty     = r.m_Empty;
            m_Effective = r.m_Effective;
            m_Comment   = r.m_Comment;
            m_SpendTime = r.m_SpendTime;
        }

        return *this;
    }

    void SetFilePath(const std::string& path) { m_FilePath = path; }
    void SetTotal(unsigned int n) { m_Total = n; }
    void SetEmpty(unsigned int n) { m_Empty = n; }
    void SetEffective(unsigned int n) { m_Effective = n; }
    void SetComment(unsigned int n) { m_Comment = n; }
    void SetSpendTime(unsigned int t) { m_SpendTime = t; }

    void AddTotal(unsigned int n) { m_Total += n; }
    void AddEmpty(unsigned int n) { m_Empty += n; }
    void AddEffective(unsigned int n) { m_Effective += n; }
    void AddComment(unsigned int n) { m_Comment += n; }

    const std::string& GetFilePath() const { return m_FilePath; }
    unsigned int GetTotal() const { return m_Total; }
    unsigned int GetEmpty() const { return m_Empty; }
    unsigned int GetEffective() const { return m_Effective; }
    unsigned int GetComment() const { return m_Comment; }
    unsigned int GetSpendTime() const { return m_SpendTime; }

    void Show() const;

    // 交换两个统计对象
    friend void SwapReport(FileReport& a, FileReport& b);

};

#endif  // _File_Report_H_
