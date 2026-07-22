#include <exception>
#include <iostream>

using namespace std;

/*
 * 给你单链表的头指针 head 和两个整数 left 和 right ，其中 left <= right 。
 * 请你反转从位置 left 到位置 right 的链表节点，返回 反转后的链表 。
 */

// 将反转位置的节点取下来
// 使用头插法插入left前的位置


struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};


class Solution {
public:
    ListNode* reverseBetween(ListNode* head, int left, int right) {
        ListNode *pPre = head;
        for(int i = 1; i < left; ++i)
        {
            pPre = pPre->next;
        }
        ListNode *pCurr = pPre->next;
        // 头插法反转
        for(int i = 0;i < right-left ;++i)
        {
            ListNode *pNext = pCurr->next;
            // 把next结点从链表中摘下来
            pCurr->next = pNext->next;
            // 把next插入到最前面
            pNext->next = pPre->next;
            // pre指向新的头
            pPre->next = pNext;
        }
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
    ListNode* reversedHead = solution.reverseBetween(head,2,4);

    std::cout << "反转后: ";
    printList(reversedHead);

    // 3. 释放内存
    freeList(reversedHead);

    return 0;
}
