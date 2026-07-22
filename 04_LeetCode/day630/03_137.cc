#include <iostream>
#include <vector>
using namespace std;

/*
 * 只出现一次的数字 II
 * 给你一个整数数组 nums ，除某个元素仅出现一次外，其余每个元素都恰出现三次。
 * 请你找出并返回那个只出现了一次的元素。
 * 你必须设计并实现线性时间复杂度的算法且使用常数级空间来解决此问题。
 */

//

class Solution {
public:
    int singleNumber(vector<int>& nums) {
        int result = 0;
        for(int i = 0; i < 32 ;++i)
        {
            // 制造掩码，和每一位相与
            int digit = 1 << i;
            // 统计当前位有多少个1
            int count = 0;
            for(auto num : nums)
            {
                // 判断num的第i位是否为1
                if(num & digit)
                {
                    ++count;
                }
            }
            // 去掉出现3次的贡献，余1是唯一出现一次的数字在这位为1
            int flag = count % 3;
            if(flag)
            {
                // 把这一位加入答案
                // 也可以写成 result |= digit
                result += digit;
            }
        }
        return result;
    }
};

int main(int argc,char *argv[])
{
    vector<int> nums = {0,1,0,1,0,1,99};
    Solution s;
    cout << s.singleNumber(nums)<< endl;

    return 0;
}
