#include <iostream>
#include <vector>
#include <dirent.h>

using namespace std;

std::vector<string> scan(const string& dir)
{
    vector<string> files;
    DIR *dp = opendir(dir.c_str());
    // 打不开，报错
    if(dp == nullptr)
    {
        perror("opendir");
        return files;
    }
    // 打开了
    struct dirent *entry;
    while((entry = readdir(dp)) != nullptr)
    {
        string name = entry->d_name;
        if(name == "." || name == "..")
        {
            continue;
        }
        if(entry->d_type == DT_REG)
        {
            files.push_back(dir + "/" + name);
        }
    }
    closedir(dp);
    return files;
}

int main(int argc,char *argv[])
{
    auto dir = scan("dic");
    for(auto &d : dir)
    {
        cout << d << endl;
    }

    return 0;
}
