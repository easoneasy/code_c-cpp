#include <iostream>
#include <vector>
using namespace std;

/*
 * 二分查找：
 * 给定一个 n 个元素有序的（升序）整型数组 nums 和一个目标值 target ，
 * 写一个函数搜索 nums 中的 target，如果 target 存在返回下标，否则返回 -1。
 * 你必须编写一个具有 O(log n) 时间复杂度的算法。
 */

// 使用二分查找法，每次都找索引的中间值，与target比较
// 如果比target大就取左边，比target小就取右边，相等返回mid

class Solution {
public:
    int search(vector<int>& nums, int target) {
        int left = 0;
        int right = nums.size();  // 左闭右开区间

        while(left < right)  // 循环条件--判断left和right的范围
        {
            int mid = left + (right - left ) / 2;
            if(nums[mid] < target)
            {
                left = mid + 1;
            }else if (nums[mid] > target)
            {
                right = mid;
            }else{
                return mid;
            }
        }
        return  -1;
    }
};

int main(int argc,char *argv[])
{
    vector<int> nums = {-1,0,3,5,9,12};
    Solution s;
    int result = s.search(nums, 9);
    cout << result << endl;

    return 0;
}
