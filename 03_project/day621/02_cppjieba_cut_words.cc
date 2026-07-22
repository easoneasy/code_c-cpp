#include <boost/fusion/support/tag_of.hpp>
#include <iostream>
#include <vector>
#include <cppjieba/Jieba.hpp>

using namespace std;

void print_words(const string &title,const vector<string> &words)
{
    cout << "[" << title << "]";
    for(const auto &w : words)
    {
        cout << w << "/";
    }
    cout << endl;
}

int main(int argc,char *argv[])
{

    // 创建对象，读取词典文件，比较耗时-> 只创建一个对象
    cppjieba::Jieba tokenizer;

    string s = "搅吧搅吧，你们就搅吧，搅得胡宗宪前方打仗没了军需吃了败仗，搅得东南大乱把大明朝亡了，老子无非陪着你们一起玩命就是";
    vector<string> words;

    // MP
    tokenizer.Cut(s,words,false);
    print_words("MP",words);
    // HMM
    tokenizer.CutHMM(s,words);
    print_words("MP",words);
    // MIX
    tokenizer.Cut(s,words);
    print_words("MIX",words);

    return 0;
}
