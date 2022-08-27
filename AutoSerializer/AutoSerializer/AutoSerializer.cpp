// AutoSerializer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>

#include <vector>
#include "Serialization.h"

using namespace AutomaticSerialization;

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

class WeaponAttribute : public AutoSerializer
{
public:
    void SetTestData()
    {
        durability.Set(999);
        power.Set(77.7f);
    }

private:
    Serializable<int32_t> durability{ this };
    Serializable<float> power{ this };
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


    // Tes code to write to file.
    //FILE* fp2{};
    //size_t totalSize{};
    //::fopen_s(&fp2, "e:\\AutoSerializer\\weapon.dat", "rb");
    //if (fp2 != nullptr)
    //{
    //    ::fread_s(&totalSize, sizeof(totalSize), 1, sizeof(totalSize), fp2);
    //}

    //std::vector<char> readBuffer(totalSize, 0);
    //if (fp2 != nullptr)
    //{
    //    ::fseek(fp2, 0, 0);
    //    ::fread_s(readBuffer.data(), totalSize, 1, totalSize, fp2);
    //    ::fclose(fp2);
    //}

    //WeaponAttribute smallgun2{};
    //smallgun2.DeserializeAuto(readBuffer.data());
    //std::cout << "read from file." << std::endl;
    //std::cout << smallgun.power.get() << std::endl;
    //std::cout << smallgun.durability.get() << std::endl;
}
