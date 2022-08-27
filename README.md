# SerializeCPP
Serialize member variables automatically in C++.

This source code implements to serialize(and deserialize) specific class member variables automatically.

In general, when you want to serialize, you may coding something like this.
```
// serialize.
ar << hp << speed

// deserialize.
ar >> hp >> speed
```

In this case, you should write the name of the variables to deserialize.
Is there a way to deserialize automatically without the names?

I thought about using std::vector to access member variables as indexes.
Each classes have a vector to contain member variables pointer.
All member variables pointer would go into the vector, then you can access it by index.
Then you could serialize them using for-loop statement, and could deserialize it the similar way without the writing its names.

## Usage :
```
#include <iostream>

#include <vector>

// include.
#include "Serialization.h"

// namespace.
using namespace AutomaticSerialization;

// some attribute class.
class CharacterAttribute : public AutoSerializer
{
public:
    void SetTestData()
    {
        this->hp.Set(99);
        this->job.Set(7);
        this->speed.Set(150.0f);

        this->isAlive = true;
    }

    void SetHp(int64_t _hp)
    {
        this->hp.Set(_hp);
    }

    void SetSpeed(float _speed)
    {
        this->speed.Set(_speed);
    }

    void SetJob(int32_t _job)
    {
        this->job.Set(_job);
    }

    void SetAlive(bool _isAlive)
    {
        this->isAlive = _isAlive;
    }

    void Print()
    {
        std::cout << "hp:" << hp.Get() << std::endl;
        std::cout << "job:" << job.Get() << std::endl;
        std::cout << "speed:" << speed.Get() << std::endl;

        std::cout << "(non-serializable) isAlive:" << isAlive << std::endl;
    }

private:
    // serializable.
    Serializable<int64_t> hp{ this };
    Serializable<int32_t> job{ this };
    Serializable<float> speed{ this };

    // non-serializable.
    bool isAlive{};
};

int main()
{
    //------------------------------------------------
    std::cout << "----- serailize all -----" << std::endl;
    CharacterAttribute foo{};
    foo.SetTestData();
    // foo -> byte array.
    std::vector<char> bufferFoo(foo.GetSerializationBufferSize(), 0);
    foo.Serialize(bufferFoo.data());
    foo.Print();

    // byte array -> bar.
    std::cout << std::endl;
    std::cout << "----- deseralize all -----" << std::endl;
    CharacterAttribute bar{};
    bar.Deserialize(bufferFoo.data());
    bar.Print();



    //------------------------------------------------
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "----- serailize modified variables only -----" << std::endl;
    CharacterAttribute foo2{};
    foo2.SetHp(123);
    foo2.SetSpeed(99.5f);
    foo2.SetAlive(true);
    // foo2 -> byte array.
    std::vector<char> bufferFoo2(foo2.GetModifiedSerializationBufferSize(), 0);
    foo2.Serialize(bufferFoo2.data());
    foo2.Print();

    // byte array -> bar2.
    std::cout << std::endl;
    std::cout << "----- deserialize modified variables only -----" << std::endl;
    CharacterAttribute bar2{};
    bar2.SetJob(1);
    bar2.Deserialize(bufferFoo2.data());
    bar2.Print();
    
    return 0;
}
```

## Detail implementation
Variables are to be implemented with the Serializable template class.

```
class CharacterAttribute : public AutoSerializer
{
private:
    Serializable<int64_t> hp{ this };
};
```
Your attribute class should inherit from AutoSerializer to serialize automatically.
The AutoSerializer has a vector container to contain member variables.
It implements Serialize and Deserialize using the container.

The Serializable class is a wrapper of a variable to serialize.
The constructor would call AddMember method to add members to the vector.
In this process, the member variables are entered in sequence into there.

The ISerializable allows different types of variables to be put into the vector in a common type and accessible.
It is the interface of the Serializable template.

```
template<typename T>
class Serializable : ISerializable
{
private:
    // Disable using default constructor to prevent declaring variables without ArrayContainer.
    Serializable() {};

public:
    T data{};
    bool isModified{};

    Serializable(AutoSerializer* serializer)
    {
        serializer->AddMember(this);
    }

    T Get()
    {
        return data;
    }

    void Set(T val)
    {
        data = val;
        isModified = true;
    }

    T* GetPtr()
    {
        return &data;
    }

    virtual void* GetDataPtrToVoidPtr()
    {
        return &data;
    }

    virtual size_t GetDataSize()
    {
        return sizeof(data);
    }

    virtual bool IsModified()
    {
        return isModified;
    }

    virtual void ResetState()
    {
        this->isModified = false;
    }
};
```
