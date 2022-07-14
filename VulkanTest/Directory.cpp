#include "Directory.h"

bool DirectoryExist(const wchar_t* directory_)
{
	std::ifstream f(directory_);
	return f.good();
}

void ReadDirectory(const wchar_t* directory_, void** buffer_out_, unsigned int* size_out_)
{
	std::ifstream f(directory_, std::ios_base::binary);

	if (!f.good())
	{
		std::wstring w_directory_ = directory_;
		throw std::runtime_error("ReadDirectory Error - File " + std::string(w_directory_.begin(), w_directory_.end()) + "Not found");
		return;
	}

	std::string* buffer = new std::string((std::istreambuf_iterator<char>(f)), (std::istreambuf_iterator<char>()));

	*buffer_out_ = buffer->data();
	*size_out_ = buffer->size();

	f.close();
}

