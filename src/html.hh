#ifndef _PEKWM_DOC_TOOL_HTML_HH_
#define _PEKWM_DOC_TOOL_HTML_HH_

#include <ostream>
#include <sstream>
#include <regex>
#include <vector>

#include "md.hh"
#include "util.hh"

class MdHtmlParser : public MdParser {
public:
	MdHtmlParser(MdDocInfo& doc, std::ostream& os)
		: _doc(doc),
		  _os(os),
		  _in_span(false)
	{
	}

	virtual ~MdHtmlParser()
	{
	}

	virtual void enter_block_quote() override
	{
		_os << "<blockquote>";
	}

	virtual void leave_block_quote() override
	{
		_os << "</blockquote>";
	}

	virtual void enter_block_ul() override
	{
		_os << "<ul>" << std::endl;
	}

	virtual void leave_block_ul() override
	{
		_os << "</ul>" << std::endl;
	}

	virtual void enter_block_ol() override
	{
		_os << "<ol>" << std::endl;
	}

	virtual void leave_block_ol() override
	{
		_os << "</ol>" << std::endl;
	}
	
	virtual void enter_block_li() override
	{
		_os << "<li>";
	}

	virtual void leave_block_li() override
	{
		_os << "</li>" << std::endl;
	}

	virtual void enter_block_hr() override
	{
		_os << "<hr/>" << std::endl;
	}

	virtual void enter_block_h(unsigned int level) override
	{
		enter_span();
	}
	
	virtual void leave_block_h(unsigned int level) override
	{
		std::string id = _doc.transform_id(_span_buf.str());
		_os << "<h" << level << " id=\"" << id << "\">"
		    << _span_buf.str()
		    << "</h" << level << ">" << std::endl;
		leave_span();
	}
	
	virtual void enter_block_code(const MdBlockCode& detail) override
	{
		_os << std::endl << "<pre>";
	}
	
	virtual void leave_block_code() override
	{
		_os << "</pre>" << std::endl;
	}
	
	virtual void enter_block_p() override
	{
		_os << "<p>";
	}

	virtual void leave_block_p() override
	{
		_os << "</p>" << std::endl;
	}

	virtual void enter_block_table() override
	{
		_os << "<table>" << std::endl;
	}

	virtual void leave_block_table() override
	{
		_os << "</table>" << std::endl;
	}

	virtual void enter_block_thead() override
	{
		_os << "<thead>" << std::endl;
	}

	virtual void leave_block_thead() override
	{
		_os << "</thead>" << std::endl;
	}

	virtual void enter_block_tbody() override
	{
		_os << "<tbody>" << std::endl;
	}

	virtual void leave_block_tbody() override
	{
		_os << "</tbody>" << std::endl;
	}

	virtual void enter_block_tr() override
	{
		_os << "<tr>" << std::endl;
	}

	virtual void leave_block_tr() override
	{
		_os << "</tr>" << std::endl;
	}

	virtual void enter_block_th() override
	{
		_os << "<th>";
	}

	virtual void leave_block_th() override
	{
		_os << "</th>" << std::endl;
	}

	virtual void enter_block_td() override
	{
		_os << "<td>";
	}

	virtual void leave_block_td() override
	{
		_os << "</td>" << std::endl;
	}

	virtual void enter_span() override
	{
		_in_span = true;
		_span_buf.clear();
	}

	virtual void leave_span() override
	{
		_in_span = false;
		_span_buf.str("");
	}

	virtual void enter_span_em() override
	{
		_os << "<em>";
	}

	virtual void leave_span_em() override
	{
		_os << _span_buf.str() << "</em>";
	}

	virtual void enter_span_strong() override
	{
		_os << "<strong>";
	}

	virtual void leave_span_strong() override
	{
		_os << _span_buf.str() << "</strong>";
	}

	virtual void enter_span_a(const MdSpanA& detail) override
	{
		_span_a_href = detail.href();
	}

	virtual void leave_span_a() override
	{
		std::string href = _doc.transform_link(_span_a_href,
						       _span_buf.str());
		_os << "<a href=\"" << href << "\">" << _span_buf.str()
		    << "</a>";
	}

	virtual void text(MD_TEXTTYPE type, const std::string& text) override
	{
		switch (type) {
		case MD_TEXT_NORMAL:
		case MD_TEXT_NULLCHAR:
		case MD_TEXT_BR:
		case MD_TEXT_SOFTBR:
		case MD_TEXT_ENTITY:
		case MD_TEXT_CODE:
			// FIXME: html encode
			if (_in_span) {
				_span_buf << text;
			} else {
				_os << text;
			}
			break;
		// disabled types below, flags not set
		case MD_TEXT_HTML:
		case MD_TEXT_LATEXMATH:
			break;
		}
	}

private:
	MdDocInfo& _doc;
	std::ostream& _os;

	bool _in_span;
	std::ostringstream _span_buf;
	std::string _span_a_href;
};

class HtmlDocLink {
public:
	HtmlDocLink(const std::string& href, const std::string& title)
		: _href(href),
		  _title(title) { }

	const std::string& href() const { return _href; }
	const std::string& title() const { return _title; }

private:
	std::string _href;
	std::string _title;
};

class HtmlDocInfo : public MdDocInfo {
public:
	HtmlDocInfo(const std::string& toc_file)
		: _toc_file(toc_file),
		  _re_doc_link("^([a-zA-Z0-9_-]+\\.md)#?(.*)") { }
	virtual ~HtmlDocInfo() { }

	const std::vector<HtmlDocLink>& linked_files() const {
		return _linked_files;
	}

	virtual std::string transform_id(const std::string& id) override {
		std::string transformed_id;
		for (size_t i = 0; i < id.size(); i++) {
			if (id[i] == ' ') {
				transformed_id += '-';
			} else {
				transformed_id += tolower(id[i]);
			}
		}
		return transformed_id;
	}

	virtual std::string transform_link(const std::string& link,
					   const std::string& title) override {
		if (link.empty()) {
			return "";
		}

		if (link[0] == '#') {
			// link to section in the same .md file, all sections
			// are prefixed with a transformed file name to make
			// the globally "unique"
			return link;
		}

		if (link == _toc_file) {
			// TOC file is output as index.html, simple translation
			return "index.html";
		}

		std::smatch results;
		if (std::regex_match(link, results, _re_doc_link)) {
			std::vector<HtmlDocLink>::iterator it =
				_linked_files.begin();
			for (; it != _linked_files.end(); ++it) {
				if (it->href() == results[1]) {
					break;
				}
			}

			if (it == _linked_files.end()) {
				_linked_files.push_back(
						HtmlDocLink(results[1], title));
			}

                        // link to section in other .md file, convert to
                        // transforme/ prefix.
			std::string transformed_link =
				path_replace_ext(results[1].str(), "html");
			if (! results[2].str().empty()) {
				transformed_link += "#";
				transformed_link += results[2].str();
			}
                        return transformed_link;
		} else {
			// standard link, keep as is
			return link;
		}
	}

private:
	std::string _toc_file;
	std::regex _re_doc_link;
	std::vector<HtmlDocLink> _linked_files;
};

#endif // _PEKWM_DOC_TOOL_HTML_HH_
