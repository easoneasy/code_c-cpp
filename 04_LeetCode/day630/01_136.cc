#include <iostream>
#include <vector>
using namespace std;
/*
 * 只出现一次的数字
 * 给你一个 非空 整数数组 nums ，除了某个元素只出现一次以外，其余每个元素均出现两次。
 * 找出那个只出现了一次的元素。
 */

// 异或
// a ^ a = 0
// a ^ 0 = a
// 异或满足交换律结合律

class Solution {
public:
    int singleNumber(vector<int>& nums) {
        int ans = 0;
        for(auto num : nums)
        {
            ans ^= num;
        }
        return ans;
    }
};


int main(int argc,char *argv[])
{
    vector<int> nums = {4,1,2,1,2};
    Solution s;
    cout << s.singleNumber(nums) << endl;

    return 0;
}
