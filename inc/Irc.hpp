// B's part

#ifndef IRC_HPP
# define IRC_HPP

# include <sstream>
# include <string>

# define SERVER_NAME "ircserv"
# define SERVER_VERSION "1.0"

inline std::string toLower(const std::string& text)
{
	std::string result(text);
	for (size_t i = 0; i < result.size(); ++i)
		if (result[i] >= 'A' && result[i] <= 'Z')
			result[i] = static_cast<char>(result[i] + 32);
	return (result);
}

inline std::string toString(int value)
{
	std::ostringstream stream;
	stream << value;
	return (stream.str());
}

#endif
