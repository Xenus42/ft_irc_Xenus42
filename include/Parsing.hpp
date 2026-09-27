#ifndef PARSING_HPP
# define PARSING_HPP

# include <stdexcept>
# include <string>
# include <vector>

class Parsing
{
  public:
	static const std::string::size_type MAX_LINE_LENGTH = 512;
	static const std::string::size_type MAX_PARAMS = 15;

	Parsing();
	Parsing(const Parsing &other);
	Parsing &operator=(const Parsing &other);
	~Parsing();
	struct	Command
	{
		std::string prefix;
		std::string name;
		std::vector<std::string> params;
	};

	Command parse(const std::string &line) const;

  private:
	bool isSeparator(char c) const;
	std::string removeSpaceBeginningAndEndString(const std::string &str) const;
	std::string toUpperCase(const std::string &str) const;
	std::string::size_type findFirstSeparator(const std::string &str,
		std::string::size_type from) const;
	std::string extractPrefix(const std::string &input,
		std::string &prefix) const;
	std::string::size_type findTextParamsStart(const std::string &input) const;
	std::vector<std::string> splitAndPutSimpleWordsIntoArray(const std::string &input) const;
	std::vector<std::string> splitCommandAndArgsToArray(const std::string &input) const;
};

#endif
