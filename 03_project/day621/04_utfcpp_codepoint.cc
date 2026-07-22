#include <iostream>
#include <utfcpp/utf8.h>
#include <utfcpp/utf8/checked.h>

using namespace std;


int main(int argc,char *argv[])
{
    string s = "臣是嘉靖二十一年的进士，是天子的门生.";

    // 获取utf8的起始迭代器和末尾迭代器
    auto it = utf8::iterator<string::const_iterator>(s.begin(),s.begin(),s.end());
    auto end = utf8::iterator<string::const_iterator>(s.end(),s.begin(),s.end());

    for(;it != end;++it)
    {
        char32_t codepoint = *it;
        cout << "U+" << hex<< codepoint << endl;
    }

    return 0;
}
