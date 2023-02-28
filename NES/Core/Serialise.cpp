//
//  Serialise.cpp
//  NES
//
//  Created by Richard Wallis on 02/01/2023.
//

#include "Serialise.h"

Archive::Archive()
: m_mode(ArchiveMode_Invalid)
, m_pMem(nullptr)
, m_memSize(0)
, m_readHead(0)
, m_writeHead(0)
{}

Archive::Archive(ArchiveMode mode)
: m_mode(mode)
, m_pMem(nullptr)
, m_memSize(0)
, m_readHead(0)
, m_writeHead(0)
{
    m_memSize = kArchiveMemoryIncrement;
    m_pMem = new uint8_t[m_memSize];
}

Archive::~Archive()
{
    if(m_pMem != nullptr)
    {
        delete [] m_pMem;
        m_pMem = nullptr;
    }
}

bool Archive::Load(const char* pPath)
{
    m_writeHead = 0;
    size_t fileSize = 0;
    
    FileStack fileLoad(fopen(pPath, "r"));
    if(fileLoad.handle() != nullptr)
    {
        if(fseek(fileLoad.handle(), 0, SEEK_END) == 0)
        {
            // Fetch seek end file size
            fileSize = (uint32_t)ftell(fileLoad.handle());

            // Seek begin
            if(fseek(fileLoad.handle(), 0, SEEK_SET) == 0)
            {
                // Increase or create new allocation
                if(fileSize > m_memSize || m_pMem == nullptr)
                {
                    if(m_pMem != nullptr)
                    {
                        delete [] m_pMem;
                    }
                    
                    m_memSize = fileSize;
                    m_pMem = new uint8_t[m_memSize];
                }
            
                // Load fileSize count bytes
                m_writeHead = fread(m_pMem, 1, fileSize, fileLoad.handle());
            }
        }
    }

    return m_writeHead > 0 && m_writeHead == fileSize;
}

bool Archive::Save(const char* pPath) const
{
    size_t bytesSaved = 0;
 
    if(m_writeHead > 0)
    {
        FileStack fileSave(fopen(pPath, "w"));
        if(fileSave.handle() != nullptr)
        {
            // Save writeHead count bytes
            bytesSaved = fwrite(m_pMem, 1, m_writeHead, fileSave.handle());
        }
    }
     
    return m_writeHead > 0 && bytesSaved == m_writeHead;
}
