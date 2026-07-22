#include <iostream>
#include <vector>
using namespace std;

/* 34. 在排序数组中查找元素的第一个和最后一个位置
 *
 * 给你一个按照非递减顺序排列的整数数组 nums，和一个目标值 target。
 * 请你找出给定目标值在数组中的开始位置和结束位置。
 * 如果数组中不存在目标值 target，返回 [-1, -1]。
 * 你必须设计并实现时间复杂度为 O(log n) 的算法解决此问题。
 */



class Solution {
public:
    // 查找target左侧的位置
    int findLeft(vector<int> &nums ,int target)
    {
        int left = 0;
        int right = nums.size() -1;
        int lower = -1; // 记录所有满足条件的mid的最小值
        while(left <= right)
        {
            int mid = left + (right - left) / 2;
            if(nums[mid] == target)
            {
                // 记录当前位置
                lower = mid;
                // 向左找
                right = mid - 1;
            }else if(nums[mid] < target)
            {
                // target在右侧
                left = mid + 1;
            }else
            {
                // target在左侧
                right = mid - 1;
            }
        }
        return lower;
    }
    // 查找target右侧的位置
    int findRight(vector<int> &nums,int target)
    {
        int left = 0;
        int right = nums.size()-1;
        int upper = -1;
        while(left <= right)
        {
            int mid = left + (right - left) / 2;
            if(nums[mid] == target)
            {
                upper = mid;
                left = mid + 1;
            }else if(nums[mid] < target)
            {
                left = mid +1;
            }else{
                right = mid -1;
            }
        }
        return upper;
    }
    vector<int> searchRange(vector<int>& nums, int target) {
        int first = findLeft(nums, target);
        int last = findRight(nums, target);
        return {first,last};
    }
};

int main(int argc,char *argv[])
{
    vector<int> nums = {5,7,7,8,8,10};
    Solution s;
    auto result = s.searchRange(nums, 8);
    for(auto r : result)
    {
        cout << r << endl;
    }

    return 0;
}
