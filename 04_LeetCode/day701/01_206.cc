#include <exception>
#include <iostream>

using namespace std;

/*
 * 给你单链表的头节点 head ，请你反转链表，并返回反转后的链表。
 */

// 使用三个指针  prev 、curr 、 next
// 保存后面的结点
// 当前结点反转
// prev前进一步
// curr前进一步

struct ListNode {
     int val;
     ListNode *next;
     ListNode() : val(0), next(nullptr) {}
     ListNode(int x) : val(x), next(nullptr) {}
     ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        // 已经反转好的链表头
        ListNode *prev = nullptr;
        // 当前正在处理的结点
        ListNode *curr = head;
        // 保存当前结点后面的结点
        ListNode *next = nullptr;
        // 当前结点不为空，就一直反转
        while(curr)
        {
            // 保存后面的链表
            next = curr->next;
            // 当前结点指向前一个结点
            curr->next = prev;
            // prev 向前移动
            prev = curr;
            // curr向前移动
            curr = next;
        }
        return prev;
    }
};

// 辅助函数：打印链表
void printList(ListNode* head) {
    ListNode* curr = head;
    while (curr != nullptr) {
        std::cout << curr->val;
        if (curr->next != nullptr) {
            std::cout << " -> ";
        }
        curr = curr->next;
    }
    std::cout << std::endl;
}

// 辅助函数：释放链表内存
void freeList(ListNode* head) {
    ListNode* curr = head;
    while (curr != nullptr) {
        ListNode* temp = curr;
        curr = curr->next;
        delete temp;
    }
}

int main() {
    // 1. 创建链表: 1 -> 2 -> 3 -> 4 -> 5
    ListNode *head = nullptr;
    ListNode *curr = nullptr;
    for(int i = 1; i <= 5 ; ++i)
    {
        ListNode *newNode = new ListNode(i);
        if(head == nullptr)
        {
            head = newNode;
            curr = head;
        }else{
            curr->next = newNode;
            curr = curr->next;
        }
    }

    std::cout << "原链表: ";
    printList(head);

    // 2. 实例化 Solution 并调用反转函数
    Solution solution;
    ListNode* reversedHead = solution.reverseList(head);

    std::cout << "反转后: ";
    printList(reversedHead);

    // 3. 释放内存
    freeList(reversedHead);

    return 0;
}
