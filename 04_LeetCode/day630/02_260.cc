#include <climits>
#include <iostream>
#include <vector>
using namespace std;

/* 只出现一次的数字 3
 * 给你一个整数数组 nums，其中恰好有两个元素只出现一次，其余所有元素均出现两次。
 * 找出只出现一次的那两个元素。你可以按 任意顺序 返回答案。
 * 你必须设计并实现线性时间复杂度的算法且仅使用常量额外空间来解决此问题。
 */

// 异或的方法解决
// 但是会剩下两个单独的数异或的结果 xorSum
// 异或是相同为0，不同为1，异或结果不为0，说明这俩某位不同
// 可以使用 x & -x 来找到 xorSum的最低有效位，两个值在这一位肯定不同
// ---但是会有负数溢出的问题--- 需要考虑到
// (xorSum == INT_MIN) ? INT_MIN : (xorSum & -xorSum)
// 可以依此来将两个元素分组
// 其他两两相同的元素会根据zorSum的最低有效位分成两组+这俩单独的元素
// 最后两组分别异或，可以得到这俩组的落单的元素，也就是只出现一次的元素

class Solution {
public:
    vector<int> singleNumber(vector<int>& nums) {
        unsigned int  xorSum = 0;

        for(auto num : nums)
        {
            xorSum ^= num;
        }
        // 找到最低有效位
        // int divider = xorSum & (-xorSum);
        int divider = (xorSum == INT_MIN) ? INT_MIN : (xorSum & -xorSum);
        // 根据最低有效位分类
        int x1 = 0,x2 = 0;
        for(auto num : nums)
        {
            if(num & divider)
            {
                x1 ^= num;
            }else{
                x2 ^= num;
            }
        }
        return {x1,x2};
    }
};

int main(int argc,char *argv[])
{
    vector<int> nums = {1,2,1,3,2,5};
    Solution s;
    vector<int> result = s.singleNumber(nums);

    for(auto &r : result)
    {
        cout << r << endl;
    }

    return 0;
}
