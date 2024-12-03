#include <iostream>
#include <cstdlib>
#include <vector>
#include <math.h>
using namespace std;


class VectorExeption: public exception {
protected:
    const char * string;
public:
    VectorExeption(const char * value): exception(), string(value) {}
    const char* what() const _NOEXCEPT override {
        return string;
    }
};

class PopError: public VectorExeption {
public:
    PopError(const char * string): VectorExeption(string) {}
    const char * what() const _NOEXCEPT override {
        std::cout << "Pop error :(" << std::endl;
        return string;
    }
};

class PushError: public VectorExeption {
public:
    PushError(const char * string): VectorExeption(string) {}
    const char * what() const _NOEXCEPT override {
        std::cout << "Push error :(" << std::endl;
        return string;
    }
};

class IndexError: public VectorExeption {
public:
    IndexError(const char * string): VectorExeption(string) {}
    const char * what() const _NOEXCEPT override {
        std::cout << "Index error :(" << std::endl;
        return string;
    }
};


template<typename T>
class subvector {

T *mas; 
unsigned int top;
unsigned int capacity; 

private:
    void swap(subvector<T> & rhs) {
        std::swap(mas, rhs.mas);
        std::swap(top, rhs.top);
        std::swap(capacity, rhs.capacity);
    }

public:
    //конструктор
    subvector(): mas(nullptr), top(0), capacity(0) {};

    //конструктор, заполняющий value
    subvector(unsigned n, T value): top(n), capacity(2*n) {
        try {
            T* temp = new T[capacity];
            for (int i = 0; i < top; i++) {
                temp[i] = value;
            }
            mas = temp;
        } catch (bad_alloc& e) {
            throw VectorExeption("Memory allocation failed in constructor :(");
        }

    }

    //деструктор
    ~subvector() noexcept {
        delete[]mas;
    }

    T* copy (const T* source, int top, int capacity) {
        T* mas = new T[capacity];
        try {
            for (int i = 0; i < top; i++) {
                mas[i] = source[i];
            }
        }
        catch (...) {
            delete[] mas;
            throw VectorExeption("Failed to copy :(");
        }
        return mas;
    }

    //конструктор копирования
    subvector(const subvector<T> &rhs): mas(copy(rhs.mas, rhs.top, rhs.capacity)), top(rhs.top), 
        capacity(rhs.capacity) {}

    //оператор присваивания копированием
    subvector& operator= (const subvector<T> &rhs) {
        subvector<T> tmp(rhs);
        // ----------------------------- //
        swap(tmp);
        return *this;
    }

    //конструктор перемещенея 
    subvector (subvector<T>&& rhs) noexcept {
        mas = rhs.mas;
        top = rhs.top;
        capacity = rhs.capacity;
        delete[]rhs.mas;
        rhs.mas = nullptr;
        rhs.top = 0;
        rhs.capacity = 0;
    }

    //оператор присваивания перемещением
    subvector& operator= (subvector<T>&& rhs) noexcept {
        if (this != &rhs) {
            swap(rhs);
        }
        return *this;
    }

    unsigned int get_top() const noexcept {
        return top;
    }

    unsigned int get_capacity() const noexcept {
        return capacity;
    }

    bool push_back(const T& d) {
        try {
            if (mas) {
                if (top + 1 <= capacity) {
                    mas[top++] = d;
                } else {
                    capacity = capacity * 2;
                    T *new_mas;
                    try {
                        new_mas = copy(mas, top, capacity);
                        // ----------------------------- //
                        new_mas[top++] = d;
                        T *temp = mas;
                        mas = new_mas;
                        delete[]temp;
                    } catch(...) {
                        throw PushError("Failed to push_back :(");
                    }
                }
            } else {
                capacity = (capacity + 10) * 2;
                T *new_mas = new T[capacity];
                try {
                    new_mas[top++] = d;
                } catch (...) {
                    throw PushError("Failed to push_back in empty vector :(");
                }
                mas = new_mas;
            }
            return 1;
        } catch (bad_alloc& e) {
            throw VectorExeption("Memory allocation failed in push_back :(");
        }

    } // добавление элемента в конец недовектора с выделением дополнительной памяти при необходимости

    T pop_back() {
        if (top) {
            top--;
            return mas[top];
        } else {
            throw PopError("Pop back from empty vector :(");
        }
    } // удаление элемента с конца недовектора, значение удаленного элемента вернуть 

    bool resize(unsigned int new_capacity) {
        try {
            if (new_capacity > 0) {
                capacity = new_capacity;
                try {
                    T *new_mas;
                    if (top > capacity) {
                        top = capacity;
                    }
                    new_mas = copy(mas);
                    // ----------------------------- //
                    T *temp = mas;
                    mas = new_mas;
                    delete[]temp;
                    return 1;
                } catch (...) {
                    throw;
                }

            } else {
                capacity = 0;
                top = 0;
                T *temp = mas;
                mas = nullptr;
                delete[]temp;
                return 0;
            } 
        } catch (bad_alloc& e) {
            throw VectorExeption("Memory allocation failed during resize :(");
        }
    } // увеличить емкость недовектора 

    void shrink_to_fit() {
        resize(top);     
    } // очистить неиспользуемую память, переехав на новое место с уменьшением capacity до top
    void clear() noexcept{
        top = 0;
    } // очистить содержимое недовектора, занимаемое место при этом не меняется

    T& operator[](int i) {
        if (i >= 0 && i < top) {
            return mas[i];
        } else{
            throw IndexError("Index out of bounds in const operator :(");
        }
    }

    T operator[](int i) const{
        if (i >= 0 && i < top) {
            return mas[i];
        } else{
            throw IndexError("Index out of bounds :(");
        }
    }
};

template<typename T>
ostream& operator<< (ostream &os, const subvector<T> &v) {
    for (int i = 0; i < v.get_top(); i++) {
        os << v[i] << " ";
    }
    os << endl;
    return os;
}   


int main() {
    try {
        subvector<int> a(5, 5);
        subvector<int> b(a);
        subvector<int> c(2, 7);
        c = b;

        //Pop Error test
        for (int i = 0; i < 7; i++) {
            cout << a.pop_back() << endl;
        }

        // Index Error test
        // cout << a[10];

    
    } catch (PopError& e){
        cout << e.what() << endl;
    } catch (PushError& e){
        cout << e.what() << endl;
    } catch (IndexError& e){
        cout << e.what() << endl;
    } catch (VectorExeption& e) {
        cout << e.what() << endl;
    }


}