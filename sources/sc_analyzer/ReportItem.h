#pragma once

namespace sc
{
    class ReportItem
    {
    protected:

        unsigned int nLines{ 0 };       // 物理行数
        unsigned int nCodes{ 0 };       // 有效行数
        unsigned int nComments{ 0 };    // 注释行数
        unsigned int nBlanks{ 0 };      // 空白行数

    public:

        unsigned int Lines() const { return nLines; }
        unsigned int Codes() const { return nCodes; }
        unsigned int Blanks() const { return nBlanks; }
        unsigned int Comments() const { return nComments; }

        void AddLines(unsigned int n) { nLines += n; }
        void AddCodes(unsigned int n) { nCodes += n; }
        void AddBlanks(unsigned int n) { nBlanks += n; }
        void AddComments(unsigned int n) { nComments += n; }

        void AddLines() { ++nLines; }
        void AddCodes() { ++nCodes; }
        void AddBlanks() { ++nBlanks; }
        void AddComments() { ++nComments; }

        ReportItem& operator += (const ReportItem& item)
        {
            AddLines(item.Lines());
            AddCodes(item.Codes());
            AddBlanks(item.Blanks());
            AddComments(item.Comments());

            return *this;
        }

    };  // class ReportItem

    /*
    class FileReport
    {
    private:

        std::string m_FilePath;     // 文件路径
        std::string m_Type;         // 语言类型
        ReportItem m_Report;        // 统计数据

    public:

        FileReport() = default;

        FileReport(const std::string& path, const std::string& type, const ReportItem& item)
            : m_FilePath(path), m_Type(type), m_Report(item)
        { }

        void SetFilePath(const std::string& path) { m_FilePath = path; }
        void SetType(const std::string& type) { m_Type = type; }

        const std::string& GetFilePath() const { return m_FilePath; }
        const std::string& GetType() const { return m_Type; }
        const ReportItem& GetReport() const { return m_Report; }
        ReportItem& GetReport() { return m_Report; }
    };  // class FileReport
    */
}   // namespace sc
