#include <iostream>
#include <vector>
using namespace std;

/* 找到 K 个最接近的元素
 *
 * 给定一个 排序好 的数组 arr ，两个整数 k 和 x ，从数组中找到最靠近 x（两数之差最小）的 k 个数。
 * 返回的结果必须要是按升序排好的。
 * 整数 a 比整数 b 更接近 x 需要满足：
 * |a - x| < |b - x| 或者 |a - x| == |b - x| 且 a < b
 */

class Solution {
public:
    vector<int> findClosestElements(vector<int>& arr, int k, int x) {
        // 二分的是窗口的起始位置
        int left = 0;
        int right = arr.size() - k;

        while(left < right)
        {
            // mid 是当前窗口的起点
            int mid = left + (right - left) / 2;
            // 比较两个相邻窗口中唯一不同的两个元素
            if(x - arr[mid] > arr[mid + k] -x)
            {
                // 右边元素更接近x，窗口向右移动
                left = mid + 1;
            }else{
                // 左边元素更接近x / 距离相等时保留较小元素
                right = mid;
            }
        }
        // left 就是最佳窗口的起点
        return vector<int> (arr.begin() + left,arr.begin() + left + k);
    }
};

int main(int argc,char *argv[])
{
    vector<int> arr = {1,2,3,4,5};
    int k = 4;
    int x = 3;
    Solution s;
    auto result = s.findClosestElements(arr, k,  x);
    for(auto r : result)
    {
        cout << r << endl;
    }

    return 0;
}
