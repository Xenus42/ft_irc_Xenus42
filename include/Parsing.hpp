#pragma once

#include <string>
#include <vector>

class Parsing
{
  public:
	Parsing();
	~Parsing();

	struct	Command
	{
		std::string name;
		std::vector<std::string> params;
	};

	Command parse(const std::string &line) const;

  private:
	void trimSpaces(std::string &str) const;
};
