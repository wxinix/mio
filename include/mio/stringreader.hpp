/* Copyright 2019 https://github.com/wxinix
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _MIO_STRING_READER_H_
#define _MIO_STRING_READER_H_

#include <mio/mio.hpp>

namespace mio {

/**
   A fast line reader based on memory mapped file. It is ~ 15x faster than
   std::getline.

   Example:
     std::filesystem::path file_path = std::filesystem::current_path()/"test.txt"; 
     assert(std::filesystem::exist(file_path);
     mio::LineReader line_reader(file_path.string());
        
     for(std::string line; line_reader.getline(line); ) {
          // do something with line
     }
 */
class StringReader
{
public:

    /**
       Constructs a reader to read from a disk file line by line. If the 
       specified file does not exist, std::system_error will be thrown with 
       error code describing the nature of the error.
    
       \param   a_file  The file to read. Must exists.
     */
    StringReader(const std::string &a_file): m_mmap {a_file}
    {
        m_begin = m_mmap.begin();
    }

    StringReader(const std::string &&a_file): m_mmap {a_file}
    {
        m_begin = m_mmap.begin();
    }

    StringReader() = delete;
    StringReader(const StringReader &) = delete;
    StringReader(StringReader &&) = delete;
    StringReader &operator =(StringReader &) = delete;
    StringReader &operator =(StringReader &&) = delete;

    ~StringReader()
    {
    }

    /**
       Checks whether the reader has reached end of file.
    
       \returns True if end of line, false otherwise.
     */
    bool eof() const
    {
        return (m_begin == nullptr);
    }

    /**
       Reads a new line into a string.
    
       \param [in,out]  a_line  The line that has been read from the file.
    
       \returns True if it reads a new line, false if it has reached end of file.
     */
    bool getline(std::string &a_line)
    {        
        if (!m_mmap.is_mapped())
            return false;

        const gsl::not_null<const char *> l_begin = m_begin;
        const gsl::not_null<const char *> l_find = std::find(l_begin.get(), m_mmap.end(), '\n');

        bool result;

        if (result = (l_find.get() != m_mmap.end())) {
            a_line.assign(l_begin.get(), std::prev(l_find.get()));
            m_begin = std::next(l_find.get());
        };

        if ((!result) && m_mmap.is_mapped()) {
            m_mmap.unmap();
            m_begin = nullptr;
        }

        return result;
    }
private:
    mmap_source m_mmap;
    const char *m_begin;
};

}

#endif