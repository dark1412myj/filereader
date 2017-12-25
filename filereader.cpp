#include<iostream>
#include<fstream>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include"filereader.h"

#define MAX_MAP_SIZE  	(128*1024*1024)		//128M

bool FileReader::Open(const char* path)
{
    if( (m_hfile = open(path,O_RDONLY,00777) )<=0 )
        return false;

    struct stat status;
    fstat(m_hfile,&status);
    m_file_size = status.st_size;
    return true;
}

bool FileReader::Seek(size_t offset)
{
    if(offset<0||offset>m_file_size)
        return false;
    m_read_offset=offset;
    return true;
}

int FileReader::Read(char* data,size_t len)
{
    if(len>MAX_MAP_SIZE)
        len=MAX_MAP_SIZE;
    if( len + m_read_offset > m_file_size )
        len = m_file_size - m_read_offset;
    if( ( m_read_offset -  m_map_offset +  len) > m_inter_size  )
    {
        if(m_inter_data)
            munmap(m_inter_data,m_inter_size);

        m_map_offset = m_read_offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

        m_inter_size = (m_file_size - m_map_offset) > MAX_MAP_SIZE ?
                       MAX_MAP_SIZE:(m_file_size - m_map_offset);

        m_inter_size = m_inter_size & ~(sysconf(_SC_PAGE_SIZE) - 1);
        //cnt++;
        m_inter_data = mmap(0,m_inter_size,PROT_READ,MAP_SHARED,m_hfile,m_map_offset );
    }
    memcpy(data,m_inter_data + m_read_offset - m_map_offset,len);

    m_read_offset+=len;
    return len;
}

int FileReader::Read_Lock(char* &data,size_t len)
{
    if(len>MAX_MAP_SIZE)
        len=MAX_MAP_SIZE;
    if( len + m_read_offset > m_file_size )
        len = m_file_size - m_read_offset;
    if( ( m_read_offset -  m_map_offset +  len) > m_inter_size  )
    {
        if(m_inter_data)
            munmap(m_inter_data,m_inter_size);

        m_map_offset = m_read_offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

        m_inter_size = (m_file_size - m_map_offset) > MAX_MAP_SIZE ?
                       MAX_MAP_SIZE:(m_file_size - m_map_offset);

        m_inter_size = m_inter_size & ~(sysconf(_SC_PAGE_SIZE) - 1);
        //cnt++;
        m_inter_data = mmap(0,m_inter_size,PROT_READ,MAP_SHARED,m_hfile,m_map_offset );
    }
    data = (char*)(m_inter_data+m_read_offset - m_map_offset);
    m_read_offset+=len;
    return len;
}

/*
int FileReader::Read_UnLock(char* &data,size_t len)
{
	m_read_offset+=len;
	return len;
}
*/

FileReader::~FileReader()
{
    if(m_inter_data)
        munmap(m_inter_data,m_inter_size);

    if(m_hfile>0)
    {
        close(m_hfile);
        m_hfile = 0;
    }
}

int main()
{
    FileReader reader;
    reader.Open("filename");
    char data[10];
    reader.Read((char*)data,10);

    char* ptr = NULL;
    int len = reader.Read_Lock(ptr,10);

    return 0;
}
