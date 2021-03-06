#pragma once

#include "../Common/DreamObject.h"
#include "File.h"

namespace Dream
{

    class Directory : public DreamObject
    {
    public:
        Directory(string dir);
        ~Directory();

        vector<string> list(string regex = "");
        vector<string> listSubdirectories(string regex = "");

        string getPath() const;
        void setPath(string path);
        string getName();

        bool exists();
        bool create();
        bool deleteDirectory();
        File file(string fileName);
        bool isDirectory();

    private:
        string mPath;
    };

}
