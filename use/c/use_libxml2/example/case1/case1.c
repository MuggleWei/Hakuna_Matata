#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xinclude.h"
#include "libxml/xmlIO.h"
#include "libxml/xmlstring.h"
#include "muggle/c/muggle_c.h"

void parse_storyinfo(xmlDocPtr doc, xmlNodePtr cur)
{
	xmlChar *key = NULL;
	cur = cur->children;
	while (cur)
	{
		if (xmlStrcmp(cur->name, (const xmlChar*)"keyword") == 0)
		{
			key = xmlNodeListGetString(doc, cur->children, 1);
			LOG_INFO("keyword: %s", key);
			xmlFree(key);
		}
		cur = cur->next;
	}
}

void parse_doc(const char *filepath)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;

	doc = xmlParseFile(filepath);
	if (doc == NULL)
	{
		LOG_ERROR("Document parsed failed");
		return;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL)
	{
		LOG_ERROR("Empty document");
		goto parse_doc_exit;
	}

	if (xmlStrcmp(cur->name, (const xmlChar*)"story") != 0)
	{
		LOG_ERROR("Document of the wrong type, root node != story");
		goto parse_doc_exit;
	}

	cur = cur->children;
	while (cur)
	{
		if (xmlStrcmp(cur->name, (const xmlChar*)"storyinfo") == 0)
		{
			parse_storyinfo(doc, cur);
		}

		cur = cur->next;
	}

parse_doc_exit:
	if (doc)
	{
		xmlFreeDoc(doc);
		doc = NULL;
	}
}

int main(int argc, char *argv[])
{
	LIBXML_TEST_VERSION
	
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	if (argc <= 1)
	{
		LOG_ERROR("Usage: %s <xml filepath>", argv[0]);
		exit(1);
	}

	parse_doc(argv[1]);

	return 0;
}
