#include "md.hh"

static std::string
attr_to_string(MD_ATTRIBUTE& attr)
{
	return std::string(attr.text, attr.size);
}

static int
c_enter_block(MD_BLOCKTYPE type, void* detail, void* userdata)
{
	MdParser* parser = reinterpret_cast<MdParser*>(userdata);
	switch (type) {
        case MD_BLOCK_DOC:
		// skip, document will span multiple input files
		break;
        case MD_BLOCK_QUOTE:
		parser->enter_block_quote();
		break;
        case MD_BLOCK_UL:
		parser->enter_block_ul();
		break;
        case MD_BLOCK_OL:
		parser->enter_block_ol();
		break;
        case MD_BLOCK_LI:
		parser->enter_block_li();
		break;
        case MD_BLOCK_HR:
		parser->enter_block_hr();
		break;
        case MD_BLOCK_H: {
		MD_BLOCK_H_DETAIL* c_block =
			static_cast<MD_BLOCK_H_DETAIL*>(detail);
		parser->enter_block_h(c_block->level);
		break;
	}
        case MD_BLOCK_CODE: {
		MD_BLOCK_CODE_DETAIL* c_block=
			static_cast<MD_BLOCK_CODE_DETAIL*>(detail);
		MdBlockCode block(attr_to_string(c_block->info),
				  attr_to_string(c_block->lang),
				  c_block->fence_char);
		parser->enter_block_code(block);
		break;
	}
        case MD_BLOCK_HTML:
		throw("html");
        case MD_BLOCK_P:
		parser->enter_block_p();
		break;
        case MD_BLOCK_TABLE:
		parser->enter_block_table();
		break;
        case MD_BLOCK_THEAD:
		parser->enter_block_thead();
		break;
        case MD_BLOCK_TBODY:
		parser->enter_block_tbody();
		break;
        case MD_BLOCK_TR:
		parser->enter_block_tr();
		break;
        case MD_BLOCK_TH:
		parser->enter_block_th();
		break;
        case MD_BLOCK_TD:
		parser->enter_block_td();
		break;
	}
	return 0;
}

static int
c_leave_block(MD_BLOCKTYPE type, void* detail, void* userdata)
{
	MdParser* parser = reinterpret_cast<MdParser*>(userdata);
	switch (type) {
        case MD_BLOCK_DOC:
		break;
        case MD_BLOCK_QUOTE:
		parser->leave_block_quote();
		break;
        case MD_BLOCK_UL:
		parser->leave_block_ul();
		break;
        case MD_BLOCK_OL:
		parser->leave_block_ol();
		break;
        case MD_BLOCK_LI:
		parser->leave_block_li();
		break;
        case MD_BLOCK_HR:
		// no leave of HR required, skipping
		break;
        case MD_BLOCK_H: {
		MD_BLOCK_H_DETAIL* c_block_h =
			static_cast<MD_BLOCK_H_DETAIL*>(detail);
		parser->leave_block_h(c_block_h->level);
		break;
	}
        case MD_BLOCK_CODE:
		 parser->leave_block_code();
		break;
        case MD_BLOCK_HTML:
		break;
        case MD_BLOCK_P:
		parser->leave_block_p();
		break;
        case MD_BLOCK_TABLE:
		parser->leave_block_table();
		break;
        case MD_BLOCK_THEAD:
		parser->leave_block_thead();
		break;
        case MD_BLOCK_TBODY:
		parser->leave_block_tbody();
		break;
        case MD_BLOCK_TR:
		parser->leave_block_tr();
		break;
        case MD_BLOCK_TH:
		parser->leave_block_th();
		break;
        case MD_BLOCK_TD:
		parser->leave_block_td();
		break;
	}
	return 0;
}

static int
c_enter_span(MD_SPANTYPE type, void* detail, void* userdata)
{
	MdParser* parser = reinterpret_cast<MdParser*>(userdata);
	parser->enter_span();
	switch (type) {
	case MD_SPAN_EM:
		parser->enter_span_em();
		break;
	case MD_SPAN_STRONG:
		parser->enter_span_strong();
		break;
	case MD_SPAN_A: {
		MD_SPAN_A_DETAIL* c_span_a =
			static_cast<MD_SPAN_A_DETAIL*>(detail);
		MdSpanA span_a(attr_to_string(c_span_a->title),
			       attr_to_string(c_span_a->href));
		parser->enter_span_a(span_a);
		break;
	}
	case MD_SPAN_IMG:
		break;
	case MD_SPAN_CODE:
		break;
	case MD_SPAN_DEL:
		break;
	// disabled spans below, flags not set
	case MD_SPAN_LATEXMATH:
	case MD_SPAN_LATEXMATH_DISPLAY:
	case MD_SPAN_WIKILINK:
	case MD_SPAN_U:
		break;
	};
	return 0;
}

static int
c_leave_span(MD_SPANTYPE type, void* detail, void* userdata)
{
	MdParser* parser = reinterpret_cast<MdParser*>(userdata);
	switch (type) {
	case MD_SPAN_EM:
		parser->leave_span_em();
		break;
	case MD_SPAN_STRONG:
		parser->leave_span_strong();
		break;
	case MD_SPAN_A:
		parser->leave_span_a();
		break;
	case MD_SPAN_IMG:
		break;
	case MD_SPAN_CODE:
		break;
	case MD_SPAN_DEL:
		break;
	// disabled spans below, will not actually be called due to flags
	// not set
	case MD_SPAN_LATEXMATH:
	case MD_SPAN_LATEXMATH_DISPLAY:
	case MD_SPAN_WIKILINK:
	case MD_SPAN_U:
		break;
	};
	parser->leave_span();
	return 0;
}

static int
c_text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata)
{
	MdParser* parser = reinterpret_cast<MdParser*>(userdata);
	parser->text(type, std::string(text, size));
	return 0;
}

static void
c_debug_log(const char*, void*)
{
}

/**
 * Initialize new parser for use with md4c and the MdParser interface.
 */
MD_PARSER
md_init_parser()
{
	MD_PARSER c_parser = {0};
	c_parser.flags =
		MD_FLAG_COLLAPSEWHITESPACE|
		MD_FLAG_TABLES|
		MD_FLAG_TASKLISTS|
		MD_FLAG_STRIKETHROUGH|
		MD_FLAG_PERMISSIVEURLAUTOLINKS|
		MD_FLAG_PERMISSIVEEMAILAUTOLINKS|
		MD_FLAG_PERMISSIVEWWWAUTOLINKS|
		MD_FLAG_NOHTMLSPANS|
		MD_FLAG_NOHTMLBLOCKS;
	c_parser.enter_block = c_enter_block;
	c_parser.leave_block = c_leave_block;
	c_parser.enter_span = c_enter_span;
	c_parser.leave_span = c_leave_span;
	c_parser.text = c_text;
	c_parser.debug_log = c_debug_log;
	return c_parser;
}
