#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

extern "C" {
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
}

#include "util.hh"

/**
 * Get filename part of path.
 */
std::string
path_basename(const std::string& path)
{
	char *buf = new char[path.size() + 1];
	memcpy(buf, path.c_str(), path.size());
	buf[path.size()] = '\0';
	std::string file = basename(buf);
	delete[] buf;
	return file;
}

/**
 * Get directory part of path.
 */
std::string
path_dirname(const std::string& path)
{
	char *buf = new char[path.size() + 1];
	memcpy(buf, path.c_str(), path.size());
	buf[path.size()] = '\0';
	std::string dir = dirname(buf);
	delete[] buf;
	return dir;
}

/**
 * Join two paths together, if path2 is absolute path2 will be used.
 */
std::string
path_join(const std::string& path1, const std::string& path2)
{
	if (path1.empty()) {
		return path2;
	} else if (path2.empty()) {
		return path1;
	}

	if (path2[0] == '/') {
		return path2;
	} else if (path1[path1.size() - 1] == '/') {
		return path1 + path2;
	} else {
		return path1 + '/' + path2;
	}
}

/**
 * Replace (or add) file extension on path.
 */
std::string
path_replace_ext(const std::string& path, const std::string& ext)
{
	std::string::size_type pos = path.rfind('.');
	if (pos == std::string::npos) {
		return path + "." + ext;
	}
	return path.substr(0, pos + 1) + ext;
}

/**
 * Ensure path exists and is directory.
 */
bool
path_ensure_dir(const std::string& path)
{
	return false;
}

/**
 * List content of directory and store entries in files.
 */
bool
path_list(const std::string& path, std::vector<std::string>& files)
{
	DIR* dirp = opendir(path.c_str());
	if (! dirp) {
		return false;
	}

	struct dirent* dirent = readdir(dirp);
	while (dirent != nullptr) {
		if (dirent->d_name[0] != '.'
		    && (dirent->d_type == DT_REG
			|| dirent->d_type == DT_UNKNOWN)) {
			files.push_back(dirent->d_name);
		}
		dirent = readdir(dirp);
	}

	return true;
}

/**
 * Read content of path into data.
 */
bool
file_read(const std::string& path, std::string& data)
{
	std::ifstream ifs(path);
	if (! ifs.is_open()) {
		return false;
	}

	std::stringstream buf;
	buf << ifs.rdbuf();
	data = buf.str();
	return true;
}

/**
 * Copy file content from src to dest.
 */
bool
file_copy(const std::string& src, const std::string& dest)
{
	bool status = false;
	std::ifstream ifs(src);
	if (ifs.is_open()) {
		std::ofstream ofs(dest);
		if (ofs.is_open()) {
			status = true;
			ofs << ifs.rdbuf();
			ofs.close();
		}
		ifs.close();
	}
	return status;
}

/**
 * Replace all occurances of search in str with sub
 */
void
str_sub(std::string& str, const std::string& search, const std::string& sub)
{
	ssize_t diff = sub.size() - search.size();

	std::string::size_type pos = str.find(search, 0);
	while (pos != std::string::npos) {
		str.replace(pos, search.size(), sub);
		pos = str.find(search, pos + diff);
	}
}
