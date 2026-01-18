/*
实现一个单向链表类，元素类型为如下所示Data，要求：
1. 支持插入节点、遍历节点和删除节点的功能
2. 插入操作支持拷贝语义和移动语义
3. 非必要不允许使用裸指针
4. 以下述的Data类型作为载荷数据
*/

#include <string>

class Data {
public:
    Data() = default;
    
    // 从字符串构造
    Data(const std::string& str) : some_data_(str) {}
    Data(std::string&& str) : some_data_(std::move(str)) {}
    Data(const char* str) : some_data_(str) {}  // 支持字符串字面量
    
    // 拷贝构造函数
    Data(const Data& other) : some_data_(other.some_data_) {}
    
    // 移动构造函数
    Data(Data&& other) noexcept : some_data_(std::move(other.some_data_)) {}
    
    // 拷贝赋值
    Data& operator=(const Data& other) {
        if (this != &other) {
            some_data_ = other.some_data_;
        }
        return *this;
    }
    
    // 移动赋值
    Data& operator=(Data&& other) noexcept {
        if (this != &other) {
            some_data_ = std::move(other.some_data_);
        }
        return *this;
    }
    
    const std::string& getData() const { return some_data_; }
    void setData(const std::string& data) { some_data_ = data; }
    void setData(std::string&& data) { some_data_ = std::move(data); }

private:
    std::string some_data_;
};

template<typename T>
class ListNode {
    T data;
    std::unique_ptr<ListNode<T>> next;
}

template<typename T>
class SinglyLinkedList {
private:
    size_t size_;
    std::unique_ptr<ListNode<T>> head_;

public:
    // 构造函数
    SinglyLinkedList() {}
    ~SinglyLinkedList() = default;
    // 禁用拷贝
    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    // 移动构造
    SinglyLinkedList(SinglyLinkedList&& other) : head_(std::move(other.head_)), size_(other.size_) {
        other.size_ = 0;
        auto node = other.head_->next;
        while(node != nullptr) {
            auto curNode = node;
            node = node->next;
            delete curNode;
        }
    }

    // 移动赋值
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        size_ = other.size_;
        head_ = std::move(other.head_);
    }
    
    // 插入
    template<typename DataType>
    void insert(DataType&& data) {
        ListNode<DataType>* node = new ListNode<DataType>();
        node->data = data;
        auto next = head_->next;
        node->next = next;
        head_->next = node;
        size_++;
    }
    
    // 遍历
    template<typename Func>
    void traverse(Func&& func) const {
        auto curNode = head_->next;
        while (curNode != nullptr) {
            func(curNode);
            curNode = curNode->next;
        }
    }
    
    // 删除
    bool remove(const std::string& data) {
        auto curNode = head_->next;
        auto preNode = head_;
        while (curNode != nullptr) {
            if (curNode->data == data) {
                preNode->next = curNode->next;
                delete curNode;
                size_--;
                return true;
            }
            curNode = curNode->next;
            preNode = preNode->next;
        }
        return false;
    }
    
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
};

// 测试
int main() {
    SinglyLinkedList list;
    
    // 测试不同的插入方式
    
    // 1. 使用左值
    Data data1("Hello");
    list.insert(data1);  // 拷贝
    
    // 2. 使用右值
    Data data2("World");
    list.insert(std::move(data2));  // 移动
    
    // 3. 使用字符串构造
    list.insert(Data("C++"));  // 临时对象
    
    // 4. 使用字符串字面量
    list.insert(Data{"Modern C++"});  // 利用 const char* 构造函数
    
    // 5. 默认构造后设置
    Data data3;
    data3.setData("Programming");
    list.insert(std::move(data3));
    
    // 遍历并打印
    std::cout << "List contents:" << std::endl;
    list.traverse([](const Data& data) {
        std::cout << data.getData() << std::endl;
    });
    
    std::cout << "Size: " << list.size() << std::endl;
    
    return 0;
}