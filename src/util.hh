#ifndef _PEKWM_DOC_TOOL_UTIL_HH_
#define _PEKWM_DOC_TOOL_UTIL_HH_

#include <string>
#include <vector>

std::string path_basename(const std::string& path);
std::string path_dirname(const std::string& path);
std::string path_join(const std::string& dir, const std::string& file);
std::string path_replace_ext(const std::string& path, const std::string& ext);
bool path_ensure_dir(const std::string& path);
bool path_list(const std::string& path, std::vector<std::string>& files);

bool file_read(const std::string& path, std::string& data);
bool file_copy(const std::string& src, const std::string& dest);

void str_sub(std::string& str,
	     const std::string& search, const std::string& sub);

#endif // _PEKWM_DOC_TOOL_UTIL_HH_
