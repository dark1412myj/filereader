#pragma once

class FileReader
{
    void* m_inter_data;
    long long m_file_size;
    long long m_inter_size;
    int m_hfile;
    long long m_read_offset;
    long long m_map_offset;
public:
    FileReader():
        m_inter_data(NULL),m_file_size(0),m_inter_size(0),
        m_hfile(0),m_read_offset(0),m_map_offset(0) {}
    bool Open(const char* path);

    bool Seek(size_t offset);

    inline int Read(char* data,size_t len);

    inline int Read_Lock(char* &data,size_t len);

    /*
    int Read_UnLock(char* &data,size_t len)
    {
    	m_read_offset+=len;
    	return len;
    }
    */

    ~FileReader();
};
