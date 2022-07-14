#pragma once
#include <fstream>

bool DirectoryExist(const wchar_t* directory_);
void ReadDirectory(const wchar_t* directory_, void** buffer_out_, unsigned int* size_out_);