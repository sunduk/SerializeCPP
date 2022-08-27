#pragma once

#include <vector>

namespace AutomaticSerialization
{
    // Interface of the Serializable class.
    class ISerializable
    {
    public:
        virtual void* GetDataPtrToVoidPtr() = 0;
        virtual size_t GetDataSize() = 0;
        virtual bool IsModified() = 0;
        virtual void ResetState() = 0;
    };

    /// <summary>
    /// The container of member variables to serialize.
    /// </summary>
    class ArrayContainer
    {
    public:
        std::vector<ISerializable*> members{};

        void AddMember(ISerializable* element)
        {
            members.push_back(element);
        }
    };

    /// <summary>
    /// Serialize member variables in the container.
    /// structure :
    ///   header
    ///     [total size bytes]
    ///     [variables count to serailize]
    ///   body(x count)
    ///     [variable index to serailize]
    ///     [data]
    /// </summary>
    class AutoSerializer
    {
    private:
        ArrayContainer arrayCont{};

    public:
        // This method called automatically in Serailizable constructor.
        void AddMember(ISerializable* member)
        {
            arrayCont.AddMember(member);
        }

        // Get size of total buffer.
        size_t GetSerializationBufferSize()
        {
            size_t size{};
            size += sizeof(size_t);   // total size.
            size += sizeof(int);      // total count size.
            for (int i = 0; i < arrayCont.members.size(); ++i)
            {
                size += sizeof(int);    // index size.
                size += arrayCont.members[i]->GetDataSize();
            }

            return size;
        }

        // Get size of modified members only.
        size_t GetModifiedSerializationBufferSize()
        {
            size_t size{};
            size += sizeof(size_t);   // total size.
            size += sizeof(int);      // total count size.
            for (int i = 0; i < arrayCont.members.size(); ++i)
            {
                if (arrayCont.members[i]->IsModified())
                {
                    size += sizeof(int);    // index size.
                    size += arrayCont.members[i]->GetDataSize();
                }
            }

            return size;
        }

        // Serailize.
        void Serialize(char* buf)
        {
            // total size.
            size_t totalSize{};
            totalSize += sizeof(size_t);
            char* ptrTotalSize = buf;
            buf += sizeof(totalSize);

            // total count.
            int totalCount{};
            char* ptrTotalCount = buf;
            buf += sizeof(totalCount);
            totalSize += sizeof(totalCount);

            // members.
            for (int i = 0; i < arrayCont.members.size(); ++i)
            {
                ISerializable* p = arrayCont.members[i];
                if (p->IsModified())
                {
                    // position of vector.
                    memcpy_s(buf, sizeof(i), &i, sizeof(i));
                    buf += sizeof(i);
                    totalSize += sizeof(i);

                    // data.
                    memcpy_s(buf, p->GetDataSize(), p->GetDataPtrToVoidPtr(), p->GetDataSize());
                    buf += p->GetDataSize();
                    totalSize += p->GetDataSize();

                    ++totalCount;
                }
            }

            // total count.
            memcpy_s(ptrTotalCount, sizeof(totalCount), &totalCount, sizeof(totalCount));

            // total size.
            memcpy_s(ptrTotalSize, sizeof(ptrTotalSize), &totalSize, sizeof(ptrTotalSize));

            // Set 'isModified' state to false after serialized once.
            ResetModifiedState();
        }

        // Deserialize.
        void Deserialize(const char* src)
        {
            // total size.
            size_t totalSize{};
            memcpy_s(&totalSize, sizeof(totalSize), src, sizeof(totalSize));
            src += sizeof(totalSize);

            // total count.
            int totalCount{};
            memcpy_s(&totalCount, sizeof(totalCount), src, sizeof(totalCount));
            src += sizeof(totalCount);

            // members.
            for (int i = 0; i < totalCount; ++i)
            {
                // position of vector.
                int index{};
                memcpy_s(&index, sizeof(index), src, sizeof(index));
                src += sizeof(index);

                // data.
                ISerializable* p = arrayCont.members[index];
                memcpy_s(p->GetDataPtrToVoidPtr(), p->GetDataSize(), src, p->GetDataSize());
                src += p->GetDataSize();
            }
        }

        void ResetModifiedState()
        {
            for (int i = 0; i < arrayCont.members.size(); ++i)
            {
                arrayCont.members[i]->ResetState();
            }
        }
    };

    // Implementation of the ISerializable.
    // usage :
    //    Serializable<int> number;
    //    Serializable<float> power;
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
}
