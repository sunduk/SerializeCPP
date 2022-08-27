#include "Test.h"

/*
class CharacterAttribute : public AutoSerializer
{
public:
    const int32_t VERSION = 1;

    Serializable<int64_t> hp{ this };
    Serializable<int32_t> job{ this };
    Serializable<float> speed{ this };


    // manual.
    void Serialize(char* buf)
    {
        memcpy_s(buf, sizeof(hp.get()), hp.getPtr(), sizeof(hp.get()));
        buf += sizeof(hp.get());

        memcpy_s(buf, sizeof(job.get()), job.getPtr(), sizeof(job.get()));
        buf += sizeof(job.get());

        memcpy_s(buf, sizeof(speed.get()), speed.getPtr(), sizeof(speed.get()));
        buf += sizeof(speed.get());
    }

    void Deserialize(const char* buf)
    {
        memcpy_s(hp.getPtr(), sizeof(hp.get()), buf, sizeof(hp.get()));
        buf += sizeof(hp.get());

        memcpy_s(job.getPtr(), sizeof(job.get()), buf, sizeof(job.get()));
        buf += sizeof(job.get());

        memcpy_s(speed.getPtr(), sizeof(speed.get()), buf, sizeof(speed.get()));
        buf += sizeof(speed.get());
    }
};
*/