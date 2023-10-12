#include "StringUtils.h"

std::vector<std::string> SplitString(std::string str, std::string saperator)
{
	std::vector<std::string> parts_reversed_order;

	auto previousPartEnd = str.end();
	int spIndex = saperator.size() - 1;
	for (int i = str.size() - 1; i >= 0 ; i--)
	{
		if (str[i] == saperator[spIndex])
			spIndex--;
		else
			spIndex = saperator.size() - 1;

		if (spIndex < 0)
		{
			spIndex = saperator.size() - 1;
			auto startIt = str.begin() + i + saperator.size();
			parts_reversed_order.push_back(std::string(startIt, previousPartEnd));
			previousPartEnd = str.begin() + i;
		}
	}

	std::vector<std::string> parts = std::vector<std::string>(parts_reversed_order.size());
	for (int i = 0; i < parts.size(); i++)
		parts[i] = parts_reversed_order[parts.size() - 1 - i];

	return parts;
}
