#include <iostream>
#include <utfcpp/utf8.h>
using namespace std;


int main(int argc,char *argv[])
{
    string s = "臣是嘉靖二十一年的进士，是天子的门生.";

    const char* curr = s.c_str();
    const char* end = s.c_str()+s.size();

    while(curr != end)
    {
        auto start = curr;
        // 将it移动到下一个utf8字符所在位置
        utf8::next(curr,end);
        // 一个汉字会占用多个字节，需要string表示一个汉字
        string charater = string(start,curr);
        cout << charater << endl;
    }

    return 0;
}
