#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <vector>

#include "html.hh"
#include "md.hh"
#include "util.hh"

class PekwmDocTool {
public:
	PekwmDocTool(const std::string& template_path,
		     const std::string& toc_path,
		     const std::string& dest)
		: _doc(path_basename(toc_path)),
		  _toc_path(toc_path),
		  _template_path(template_path),
		  _src(path_dirname(toc_path)),
		  _dest(dest)
	{
		load_template(template_path);
	}

	void load_template(const std::string& path) {
		file_read(path_join(path, "title"), _title);
		file_read(path_join(path, "header.tpl"), _html_header);
		file_read(path_join(path, "footer.tpl"), _html_footer);
	}

	int generate_doc() {
		std::string toc_path = path_join(_dest, "index.html");
		std::ofstream ofs(toc_path);
		if (ofs.is_open()) {
			convert_file(_toc_path, ofs, _title);
			ofs.close();
		} else {
			error("failed to open " + toc_path + " for writing");
			return 1;
		}

		// copy linked_files as it will mutate when processing files,
		// only process the linked files.
		std::vector<HtmlDocLink> md_files(_doc.linked_files());
		std::vector<HtmlDocLink>::const_iterator it = md_files.begin();
		for (it = md_files.begin(); it != md_files.end(); ++it) {
			std::string src = path_join(_src, it->href());

			std::string html_file = path_basename(src);
			html_file = path_replace_ext(html_file, "html");
			std::string html_path = path_join(_dest, html_file);

			log(src + " -> " + html_path);
			std::string title = _title + ": " + it->title();
			convert_file(src, html_path, title);
		}

		return 0;
	}

	int copy_static() {
		std::string static_path(path_join(_template_path, "static"));

		// treat no static file ok
		std::vector<std::string> files;
		if (! path_list(static_path, files)) {
			return 0;
		}

		std::vector<std::string>::const_iterator it;
		for (it = files.begin(); it != files.end(); ++it) {
			std::string src(path_join(static_path, *it));
			std::string dst(path_join(_dest, *it));
			log(src + " -> " + dst);
			if (! file_copy(src, dst)) {
				return 1;
			}
		}
		return 0;
	}

	void convert_file(const std::string& src, const std::string& dest,
			  const std::string& title) {
		std::ofstream ofs(dest);
		if (ofs.is_open()) {
			convert_file(src, ofs, title);
			ofs.close();
		} else {
			error("failed to open " + dest + " for writing");
		}
	}

	void convert_file(const std::string& src, std::ostream& os,
			  const std::string& title) {
		std::string data;
		if (! file_read(src, data)) {
			error("failed to load " + src);
			return;
		}
	
		output_header(os, title);
	
		MD_PARSER c_parser = md_init_parser();
		MdHtmlParser parser(_doc, os);
		md_parse(data.c_str(), data.size(), &c_parser, 
			 static_cast<void*>(&parser));
		output_footer(os);
	}

	void output_header(std::ostream& os, const std::string& title) {
		std::string header(_html_header);
		str_sub(header, "<TITLE>", title);
		os << header;
	}

	void output_footer(std::ostream& os) {
		os << _html_footer;
	}

	void log(const std::string& msg) {
		std::cout << msg << std::endl;
	}

	void error(const std::string& msg) {
		std::cerr << msg << std::endl;
	}

private:
	HtmlDocInfo _doc;

	std::string _title;
	std::string _toc_path;
	std::string _template_path;
	std::string _src;
	std::string _dest;

	std::string _html_header;
	std::string _html_footer;
};

int
main(int argc, char* argv[])
{
	if (argc != 4) {
		std::cerr << "usage: " << argv[0] << " template toc.md dest"
			  << std::endl;
		return 1;
	}

	PekwmDocTool tool(argv[1], argv[2], argv[3]);

	int ret = tool.generate_doc();
	if (! ret) {
		ret = tool.copy_static();
	}
	return ret;
}
