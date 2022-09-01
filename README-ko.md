//TODO: Need to translate.

# SerializeCPP
이 소스 코드는 C++로 원하는 멤버 변수들만 선택적으로 직렬화/역직렬화 하는 방법에 대해서 구현한 내용입니다.

어떤 클래스 멤버 변수가 있을 때 직렬화/역직렬화를 하기 위해서 일반적으로 아래와 같은 코드를 사용합니다.
```
// serialize.
ar << hp << speed

// deserialize.
ar >> hp >> speed
```

이 경우 직렬화 한 변수를 역직렬화 할 때 어떤 변수에 값을 넣을 것인지 명시적으로 기입해야 하죠.
하지만 변수명을 쓰지 않고도 자동적으로 역직렬화가 되게 하려면 어떻게 해야 할까요?

제가 생각한 한가지 방법은 std::vector를 이용하는 것입니다.
클래스 멤버 변수로 vector를 들고 있고 그 vector안에 모든 멤버 변수들의 포인터를 순차적으로 담아 놓습니다.
이렇게 하면 직렬화를 수행 할 때 멤버 변수에 직접 접근하지 않고 index를 통해서 접근할 수 있게 됩니다.
역직렬화 할 때도 마찬가지로 index를 통해서 접근하면 직렬화 할 때 사용한 변수들 모르더라도 어떤 변수들을 역직렬화 해야 하는지 자동적으로 알 수 있게 됩니다.

## 구현 예 :
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
    
    return 0;
}
```

## 구현 디테일
멤버 변수들은 Serializable이라는 템플릿 클래스를 이용하여 선언됩니다.

```
class CharacterAttribute : public AutoSerializer
{
private:
    Serializable<int64_t> hp{ this };
};
```
Attribute클래스는 AutoSerializer에서 상속됩니다.
AutoSerializer는 vector 하나를 멤버 변수로 갖고 있는데 이를 통해 자동적인 직렬화/역직렬화가 가능해 집니다.

The Serializable class is a wrapper of a variable to serialize.
The constructor would call AddMember method to add members to the vector.
In this process, the member variables are entered in sequence into there.

Serializable클래스는 직렬화 할 변수들을 래핑한 클래스 입니다.
생성자에서 AddMember매소드를 호출하여 AutoSerializer클래스가 갖고 있는 vector에 포인터를 등록 합니다.
결국 Serializable로 선언된 모든 멤버 변수들의 포인터가 vector에 순차적으로 들어가게 됩니다.

ISerializable클래스는 각각 다른 변수들을 하나의 공통된 타입으로 접근할 수 있는 인터페이스 입니다.
Serializable템플릿 클래스는 이 인터페이스를 상속받아 ISerializable의 매소드를 구현하게 됩니다.

```
template<typename T>
class Serializable : ISerializable
{
public:
    T data{};
    bool isModified{};

    explicit Serializable(AutoSerializer* serializer)
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

좀 더 자세한 설명 : 
https://gamecodi.com/8010/c%2B%2B%EB%A1%9C-%EC%9B%90%ED%95%98%EB%8A%94-%EB%B3%80%EC%88%98%EB%93%A4%EB%A7%8C-%EC%84%A0%ED%83%9D%EC%A0%81%EC%9C%BC%EB%A1%9C-%EC%A7%81%EB%A0%AC%ED%99%94-%EC%97%AD%EC%A7%81%EB%A0%AC%ED%99%94-%ED%95%98%EB%8A%94-%EB%B0%A9%EB%B2%95
