/*  
题目：用C++实现最小栈，支持以下四个接口:
push：入栈
pop：出栈
top：取栈顶
getMin： 取栈内最小元素
要求：
1. getMin接口时间复杂度O(1)
2. 所有接口时间复杂度O(1),辅助空间复杂度O(1)
*/
#include <iostream>
#include <vector>
#include <stack>
class MinStack {
    // 1,2,3,4,-1,3,2,0
    // 1,2,3,4   ---> stack
    // <1,4>   ---> minStack
    
    // 4,3,2,1
    // 4,3,2,1

public:
    MinStack() {};

    void push(int x) {
        stack.push(x);
        if (minStack.empty()) {
            minStack.emplace(x, 1);
            return;
        }
        auto& pair = minStack.top();
        if (x == pair.first) {
            pair.second += 1;
        } else if (x < pair.first) {
            minStack.emplace(x, 1);
        }
    }

    void pop() {
        if (stack.empty()) {
            throw std::runtime_error("stack is empty");
        }
        int value = stack.top();
        stack.pop();
        auto& pair = minStack.top();
        if (value == pair.first) {
            pair.second -= 1;
            if (pair.second == 0) {
                minStack.pop();
            }
        }
    }

    int top() {
        if (stack.empty()) {
            throw std::runtime_error("stack is empty");
        }
        return stack.top();
    }

    int getMin() const {
        if (minStack.empty()) {
            throw std::runtime_error("stack is empty");
        }
        return minStack.top().first;
    }
private:
    std::stack<int> stack;
    std::stack<std::pair<int, int> > minStack;
};


int main() {
    // push 1,2,-1,3,3,2,0  : top:0 min:-1
    // pop  : top:2 min:-1
    // pop  : top:3 min:-1
    // pop  : top:3 min:-1
    // pop  : top:-1 min:-1
    // pop  : top:2 min:1
    MinStack minStack;
    minStack.push(1);
    minStack.push(2);
    minStack.push(-1);
    std::cout << minStack.getMin() << std::endl;
    minStack.push(3);
    std::cout << minStack.getMin() << std::endl;
    minStack.pop();
    minStack.pop();
    std::cout << minStack.getMin() << std::endl;
    return 0;
}