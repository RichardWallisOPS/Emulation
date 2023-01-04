//
//  Serialise.cpp
//  NES
//
//  Created by Richard Wallis on 02/01/2023.
//  Copyright © 2023 openplanetsoftware. All rights reserved.
//

#include <cstdint>
#include <string>
#include "Serialise.h"

Archive::Archive(ArchiveMode mode /*= ArchiveMode_History*/)
: m_mode(mode)
, m_pMem(nullptr)
, m_memSize(0)
, m_readHead(0)
, m_writeHead(0)
{
    m_memSize = 1024 * 1024;
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
    FileStack fileLoad(fopen(pPath, "r"));
    if(fileLoad.handle() != nullptr)
    {
        if(fseek(fileLoad.handle(), 0, SEEK_END) == 0)
        {
            m_memSize = (uint32_t)ftell(fileLoad.handle());
        
            if(fseek(fileLoad.handle(), 0, SEEK_SET) == 0)
            {
                m_writeHead = fread(m_pMem, 1, m_memSize, fileLoad.handle());
            }
        }
    }
     
    return m_writeHead > 0 && m_writeHead == m_memSize;
}

bool Archive::Save(const char* pPath)
{
    size_t bytesSaved = 0;
 
    FileStack fileSave(fopen(pPath, "w"));
    if(fileSave.handle() != nullptr)
    {
        bytesSaved = fwrite(m_pMem, 1, m_writeHead, fileSave.handle());
    }
     
    return bytesSaved == m_writeHead;
}
