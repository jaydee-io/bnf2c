////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef STREAMS_H
#define STREAMS_H
#include <string>
#include <istream>
#include <ostream>
#include <fstream>

class Streams
{
    public :
        Streams(const std::string & inputFileName, const std::string & outputFileName);

        std::istream & inputStream(void);
        std::ostream & outputStream(void);

    protected :
        std::ifstream m_inputFileStream;
        std::ofstream m_outputFileStream;
};

#endif /* STREAMS_H */
