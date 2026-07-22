#include <bitset>
#include <cstdint>
#include <ios>
#include <iostream>
#include <simhash/Simhasher.hpp>
#include <string>

using namespace std;

// 创建Simhasher对象
simhash::Simhasher hasher;

int hamming_distance(uint64_t x,uint64_t y)
{
    int distance = 0;
    uint64_t z = x ^ y;
    while(z)
    {
        z &= (z -1);
        distance++;
    }
    return distance;
}

void print_similarity(const string &text1, const string &text2)
{
    uint64_t h1,h2;
    int topN = 5;
    hasher.make(text1,topN,h1);
    hasher.make(text2,topN,h2);
    cout << bitset<64>(h1) << endl;
    cout << bitset<64>(h2) << endl;

    cout << "海明距离 ： " << hamming_distance(h1, h2) << endl;
    cout << "是否相等 ： " << boolalpha << simhash::Simhasher::isEqual(h1,h2) << endl;
}

int main(int argc,char *argv[])
{

    string doc1 = "臣是嘉靖二十一年的进士，是天子的门生";
    string doc2 = "臣是共和国二十一年的学士，是国家的门生";
    print_similarity(doc1,doc2);

    string doc3 = "人工智能";
    string doc4 = "人工智障";
    print_similarity(doc3, doc4);

    return 0;
}
