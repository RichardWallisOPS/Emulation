//
//  Serialise.h
//  NES
//
//  Created by Richard Wallis on 02/01/2023.
//

#ifndef Serialise_h
#define Serialise_h

#ifdef __cplusplus
    #include <cstdint>
    #include <string>
#endif

class FileStack
{
public:
    FileStack(FILE* pFile)
    : m_pHandle(pFile)
    {}
    ~FileStack()
    {
        if(m_pHandle != nullptr)
        {
            fclose(m_pHandle);
            m_pHandle = nullptr;
        }
    }
    
    FILE* handle() {return m_pHandle;}

private:
    FILE* m_pHandle;
};

const uint8_t kArchiveSentinelNoData = 0x00;
const uint8_t kArchiveSentinelHasData = 0xFF;
const size_t kArchiveMemoryIncrement = 1024 * 128;

enum ArchiveMode
{
    ArchiveMode_Invalid = -1,
    
    // Only save items that are required for prev frame in current loaded cart etc
    ArchiveMode_History = 0,
    
    // Save all items for a cold start
    ArchiveMode_Persistent,
};

class Archive
{
public:
    Archive();
    Archive(ArchiveMode mode);
    ~Archive();
    
    ArchiveMode GetArchiveMode() const
    {
        return m_mode;
    }
    void Reset()
    {
        m_readHead = m_writeHead = 0;
    }
    void ResetRead()
    {
        m_readHead = 0;
    }
    
    size_t ByteCount() const {return m_writeHead;}
    
    bool Load(const char* pPath);
    bool Save(const char* pPath) const;
    
    // Saving
    template<typename T>
    void operator<<(T const& object)
    {
        const size_t objSize = sizeof(object);
        
        if(objSize + m_writeHead >= m_memSize)
        {
            IncreaseAllocation(objSize);
        }
        
        T* pWriteObjectPtr = (T*)(&m_pMem[m_writeHead]);
        *pWriteObjectPtr = object;
        
        m_writeHead += objSize;
    }
    
    void WriteBytes(void const* pBytes, size_t count)
    {
        if(count + m_writeHead >= m_memSize)
        {
            IncreaseAllocation(count);
        }
        
        memcpy(&m_pMem[m_writeHead], pBytes, count);
        
        m_writeHead += count;
    }
    
    // Loading
    template<typename T>
    void operator>>(T& object)
    {
        const size_t objSize = sizeof(object);
        
        if(objSize + m_readHead <= m_writeHead)
        {
            T* pReadObjectPtr = (T*)(&m_pMem[m_readHead]);
            object = *pReadObjectPtr;
            m_readHead += objSize;
        }
    }
    
    void ReadBytes(void* pBytes, size_t count)
    {
        if(count + m_readHead <= m_writeHead)
        {
            memcpy(pBytes, &m_pMem[m_readHead], count);
            m_readHead += count;
        }
    }
    
private:

    void IncreaseAllocation(size_t IncByteCount)
    {
        const size_t minRequiredSize = m_memSize + IncByteCount;
        
        while(m_memSize < minRequiredSize)
        {
            m_memSize += kArchiveMemoryIncrement;
        }
        
        uint8_t* pNewMem = new uint8_t[m_memSize];
        
        if(m_pMem != nullptr)
        {
            memcpy(pNewMem, m_pMem, m_writeHead);
            delete [] m_pMem;
        }
        
        m_pMem = pNewMem;
    }

private:
    ArchiveMode m_mode;
    
    uint8_t* m_pMem;
    size_t m_memSize;
    
    size_t m_readHead;
    size_t m_writeHead;
};

class Serialisable
{
public:
    virtual void Load(Archive& rArchive)        {}
    virtual void Save(Archive& rArchive) const  {}
};

#define SERIALISABLE_DECL   virtual void Load(Archive& rArchive) override; \
                            virtual void Save(Archive& rArchive) const override; \

#endif /* Serialise_h */
