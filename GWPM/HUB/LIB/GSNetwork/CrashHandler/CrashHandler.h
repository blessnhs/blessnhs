#pragma once

#include <map>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <boost/noncopyable.hpp>
#include "StackWalker.h"

class CrashHandler abstract : boost::noncopyable
{
public:
    CrashHandler() = default;
    virtual ~CrashHandler() = default;

    virtual BOOL Install() = 0;

    static std::pair<std::string, std::string> GenerateDumpFilePath();
};

#pragma region OutdatedStackwalker
class OutdatedStackwalker : public StackWalker
{
    explicit OutdatedStackwalker();
    OutdatedStackwalker& operator = (const OutdatedStackwalker&);

public:
    explicit OutdatedStackwalker(const std::string& inPath)
        : StackWalker()
        , _filePath(inPath)
        , _stream(inPath)
    {
    }

    virtual void OnOutput(LPCSTR inText);

private:
    const std::string  _filePath;
    std::ofstream      _stream;
};
#pragma endregion

