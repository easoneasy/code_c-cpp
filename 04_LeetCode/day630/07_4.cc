#include <iostream>
#include <vector>
using namespace std;

/* 寻找两个正序数组的中位数
 *
 * 给定两个大小分别为 m 和 n 的正序（从小到大）数组 nums1 和 nums2。
 * 请你找出并返回这两个正序数组的 中位数 。
 * 算法的时间复杂度应该为 O(log (m+n))
 */

// i nums1当前起点
// j nums2当前起点
// k 当前要找第k小
// 不断循环
// 判断是否有数组已经空了
// 判断k == 1
// 比较两个数组中第 k/2 个元素
// 淘汰一部分
// 更新 i 、j 、k
// 继续循环

class Solution {
public:
    // 找两个有序数组中的第k小元素，k从1开始
    int findKth(vector<int> &nums1,vector<int> &nums2,int k)
    {
        // i 、j 分别表示两个数组当前的起始位置
        int i = 0;
        int j = 0;
        while(true)
        {
            // nums1 已经没有元素了
            if(i == nums1.size())
            {
                return nums2[j + k - 1];
            }

            // nums2 已经没有元素了
            if(j == nums2.size())
            {
                return nums1[i + k - 1];
            }
            // 只需要找第1小
            if(k == 1)
            {
                return min(nums1[i],nums2[j]);
            }
            // 每次准备淘汰k/2个元素
            int half = k/2;
            // nums1 本次比较的位置
            // 找nums1当前区间的第half个元素
            // 数组下标从0开始，需要-1
            int newI = min(i + half,(int)nums1.size()) - 1;
            // nums2 本次比较的位置
            int newJ = min(j + half,(int)nums2.size()) - 1;
            // 两个待比较的元素
            int pivot1 = nums1[newI];
            int pivot2 = nums2[newJ];
            // num1 的 pivot 更小
            // 因此nums1当前区间前半部分一定不可能包含第k小元素
            if(pivot1 <= pivot2)
            {
                // nums1[i-newI]的元素全部淘汰
                // 淘汰人数
                int removed = newI - i + 1;
                // 更新k
                // 前removed个元素已经被淘汰了
                // 因此寻找的目标由第k小变成了剩余元素中的第k-removed小
                k -= removed;
                // nums1起点后移
                i= newI +1;
            }else{
                // nums2[j-newJ]的元素全部淘汰
                // 被淘汰元素的个数
                // 例如：i = 2, newI = 4
                // 淘汰：2、3、4
                // 共 4-2+1 = 3 个
                int removed = newJ - j + 1;
                k -= removed;
                j = newJ + 1;
            }
        }
    }

    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
        int total = nums1.size() + nums2.size();
        // 奇数
        if(total & 1)
        {
            return findKth(nums1, nums2, total/2+1);
        }
        // 偶数
        int left = findKth(nums1, nums2, total/2);
        int right = findKth(nums1, nums2, total/2 + 1);
        return (left + right) /2.0;
    }
};

int main(int argc,char *argv[])
{
    vector<int> nums1 = {1, 3, 5, 7, 9};
    vector<int> nums2 = {2, 4, 6, 8, 10};
    Solution s;
    cout << s.findMedianSortedArrays(nums1, nums2) << endl;

    return 0;
}
