#include <string>
#include <iostream>

#include "counter/FileReport.h"

void FileReport::Show() const
{
    std::cout << "file:"        << m_FilePath   << ' ';
    std::cout << "total:"       << m_Total      << ' ';
    std::cout << "empty:"       << m_Empty      << ' ';
    std::cout << "effective:"   << m_Effective  << ' ';
    std::cout << "comment:"     << m_Comment    << std::endl;
}

void SwapReport(FileReport & a, FileReport & b)
{
    if (&a != &b)
    {
        FileReport x = std::move(a);
        a = std::move(b);
        b = std::move(x);
    }
}
