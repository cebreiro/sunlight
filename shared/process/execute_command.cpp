#include "execute_command.h"

#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>

namespace sunlight
{
    auto ExecuteCommand(const std::string& cmd) -> std::vector<std::string>
    {
        boost::process::ipstream pipeStream;
        boost::process::child child(cmd, boost::process::std_out > pipeStream);

        std::vector<std::string> result;

        std::string line;
        while (pipeStream && std::getline(pipeStream, line) && !line.empty())
        {
            if (std::ranges::all_of(line, [](char c) -> bool
                {
                    return std::isspace(c);
                }))
            {
                continue;
            }

            boost::algorithm::trim_right(line);

            result.push_back(std::move(line));
        }

        child.wait();

        return result;
    }
}
