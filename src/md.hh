#ifndef _PEKWM_DOC_TOOL_MD_HH_
#define _PEKWM_DOC_TOOL_MD_HH_

#include <string>
extern "C" {
#include <md4c.h>
}

class MdDocInfo {
public:
	virtual std::string transform_id(const std::string& id) = 0;
	virtual std::string transform_link(const std::string& link,
					   const std::string& title) = 0;
};

class MdBlockCode {
public:
	MdBlockCode(const std::string& lang, const std::string& info,
		    char fence_char)
		: _lang(lang),
		  _info(info),
		  _fence_char(fence_char)
	{
	}

	const std::string& lang() const { return _lang; }
	const std::string& info() const { return _info; }
	const char fence_char() const { return _fence_char; }

private:
	std::string _lang;
	std::string _info;
	char _fence_char;
};

class MdSpanA {
public:
	MdSpanA(const std::string& title, const std::string& href)
		: _title(title),
		  _href(href)
	{
	}

	const std::string& title() const { return _title; }
	const std::string& href() const { return _href; }

private:
	std::string _title;
	std::string _href;
};

class MdParser {
public:
	virtual void enter_block_quote() = 0;
	virtual void leave_block_quote() = 0;
	virtual void enter_block_ul() = 0;
	virtual void leave_block_ul() = 0;
	virtual void enter_block_ol() = 0;
	virtual void leave_block_ol() = 0;
	virtual void enter_block_li() = 0;
	virtual void leave_block_li() = 0;
	virtual void enter_block_hr() = 0;
	virtual void enter_block_h(unsigned int level) = 0;
	virtual void leave_block_h(unsigned int level) = 0;
	virtual void enter_block_code(const MdBlockCode& detail) = 0;
	virtual void leave_block_code() = 0;
	virtual void enter_block_p() = 0;
	virtual void leave_block_p() = 0;
	virtual void enter_block_table() = 0;
	virtual void leave_block_table() = 0;
	virtual void enter_block_thead() = 0;
	virtual void leave_block_thead() = 0;
	virtual void enter_block_tbody() = 0;
	virtual void leave_block_tbody() = 0;
	virtual void enter_block_tr() = 0;
	virtual void leave_block_tr() = 0;
	virtual void enter_block_th() = 0;
	virtual void leave_block_th() = 0;
	virtual void enter_block_td() = 0;
	virtual void leave_block_td() = 0;

	virtual void enter_span() = 0;
	virtual void leave_span() = 0;

	virtual void enter_span_em() = 0;
	virtual void leave_span_em() = 0;
	virtual void enter_span_strong() = 0;
	virtual void leave_span_strong() = 0;
	virtual void enter_span_a(const MdSpanA& detail) = 0;
	virtual void leave_span_a() = 0;
	virtual void text(MD_TEXTTYPE type, const std::string& text) = 0;
};

MD_PARSER md_init_parser();


#endif // _PEKWM_DOC_TOOL_MD_HH_
