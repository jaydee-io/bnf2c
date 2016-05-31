////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Streams.h"

#include <sstream>
#include <iostream>
#include <iomanip>

////////////////////////////////////////////////////////////////////////////////
Streams::Streams(const std::string & inputFileName, const std::string & outputFileName)
{
    if(!inputFileName.empty())
    {
        m_inputFileStream.open(inputFileName);

        if(m_inputFileStream.fail())
            throw std::runtime_error("Unable to open input file \"" + inputFileName + "\"");
    }

    if(!outputFileName.empty())
    {
        m_outputFileStream.open(outputFileName);

        if(m_outputFileStream.fail())
            throw std::runtime_error("Unable to open output file \"" + outputFileName + "\"");
    }
}

////////////////////////////////////////////////////////////////////////////////
std::istream & Streams::inputStream(void)
{
    if(m_inputFileStream.is_open())
        return m_inputFileStream;
    else
        return std::cin;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & Streams::outputStream(void)
{
    if(m_outputFileStream.is_open())
        return m_outputFileStream;
    else
        return std::cout;
}

