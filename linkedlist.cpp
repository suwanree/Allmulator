#include <iostream>
#include <vector>
#include <emscripten/bind.h> // 필수 헤더
#include "header.h"

using namespace std;
using namespace emscripten;


template <typename T>
class Linkedlist{
    public:
        int node_count;
        Node<T>* head;
        Node<T>* tail;
        Linkedlist() : node_count(0), head(nullptr), tail(nullptr) {}


        bool insert(Node<T>& node, int pos);


        bool erase_by_pos(int pos);
        bool erase_by_value(int value);

        Node<T>* find_by_pos(int pos);
        Node<T>* find_by_value(int value);

        // 시각화를 위해 전체 데이터를 벡터로 반환하는 함수 추가
        std::vector<int> get_all_elements() {
            std::vector<int> result;
            Node<T>* temp = head;
            while(temp != nullptr) {
                result.push_back(temp->value);
                temp = temp->next;
            }
            return result;
        }

        void printlist();
};

template <typename T>
class Node{
    public:
        T value;
        Node<T>* next;

        Node(T value) : value(value), next(nullptr) {};
        
};

template <typename T>
bool Linkedlist<T>::insert(Node<T>& node, int pos){
    if(pos < 1 || pos > node_count + 1)
        return false;

    if(pos == 1){
        node.next = head;
        head = &node;

        if(node_count == 0){
            tail = &node;
        }
    }
    else{

        Node<T>* temp = head;
        for(int i = 0; i<pos-2; i++){
            temp = temp->next;
        }

        node.next = temp->next;
        temp->next = &node;

        // 마지막 자리에 넣는다면
        if(node_count+1 == pos){
            tail = tail->next;
        }
    }
    node_count++;
    return true;
};

template <typename T>
Node<T>* Linkedlist<T>::find_by_pos(int pos){
    if(pos < 1 || pos > node_count)
        return nullptr;


    Node<T>* temp = head;

    for(int i = 1; i<pos; i++){
        temp = temp->next;
    }

    return temp;
};

template <typename T>
Node<T>* Linkedlist<T>::find_by_value(int value){
    Node<T>* temp = head;
    while(temp->value != value && temp != nullptr){
        temp = temp->next;
    }
    
    return temp;
    
};

template <typename T>
bool Linkedlist<T>::erase_by_pos(int pos){
    if(pos < 1 || pos > node_count)
        return false;

    Node<T>* target_prev = nullptr;
    Node<T>* target = head; 

    for(int i = 0; i<pos-1; i++){
        target_prev = target;
        target = target->next;
    }

    // 첫번째 노드를 삭제한다면
    if(target_prev == nullptr){
        head = head->next;
        delete target;
        
        if(node_count == 1){
            tail = nullptr;
        }
    }
    else{       // 두번째 이상 노드 삭제
        target_prev->next = target->next;
        delete target;

        if(node_count == pos){
            tail = target_prev;
        }
    }

    node_count--;
    
    return true;
};

template <typename T>
bool Linkedlist<T>::erase_by_value(int value){
    if(node_count == 0)
        return false;
    
    Node<T>* target_prev = nullptr;
    Node<T>* target = head;

    while(target != nullptr && target->value != value){
        target_prev = target;
        target = target->next;
    }


    // 같은 값을 가진 노드가 없음
    if(target == nullptr)  
        return false;

    // 첫번째 노드를 삭제한다면
    if(target_prev == nullptr){
        head = head->next;
        delete target;
        
        if(node_count == 1){
            tail = nullptr;
        }
    }
    else{       // 두번째 이상 노드 삭제
        target_prev->next = target->next;
        delete target;

        // 마지막 노드를 삭제한 경우
        if(target_prev == nullptr){
            tail = target_prev;
        }
    }
    
    node_count--;
    return true;
};

/*
template <typename T>
void Linkedlist<T>::printlist(){

    Node<T>* loop = head;

    while(loop != nullptr){
        if(loop->next == nullptr)   
            cout << loop->value;
        else
            cout << loop->value << " -> ";
        
        loop = loop->next;
    }
    cout << '\n';
}
*/

// 자바스크립트 바인딩코드
// 바인딩 : C++의 함수포인터, 메모리구조를 자바스크립트 엔진에 등록하는 과정. 요약하면, 메타데이터를 자바스크립트에 제공
void insert_wrapper(Linkedlist<int>& list, int value, int pos) {
    list.insert(*(new Node<int>(value)), pos);
}

EMSCRIPTEN_BINDINGS(my_module) {
    // std::vector<int>를 JS 배열로 변환하기 위한 등록
    register_vector<int>("VectorInt");

    class_<Linkedlist<int>>("Linkedlist")
        .constructor<>()
        .function("insert", &insert_wrapper) // 래퍼 함수 연결
        .function("erase_by_pos", &Linkedlist<int>::erase_by_pos)
        .function("get_all_elements", &Linkedlist<int>::get_all_elements); // 데이터 조회
}