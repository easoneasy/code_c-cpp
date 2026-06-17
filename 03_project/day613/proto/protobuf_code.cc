#include "person.pb.h"
#include <bitset> // 位图：位的数组
#include <iostream>
#include <string>

using namespace std;
using namespace test;

int main()
{
    Person p;
    p.set_id(128);
    p.set_name("aaa");

    string data;
    p.SerializeToString(&data);

    cout << "size: " << data.size() << endl;
    // 把每个字节按二进制打印
    for (const auto& c : data) {
        cout << bitset<8>(c) << " ";
    }
    cout << endl;
}
