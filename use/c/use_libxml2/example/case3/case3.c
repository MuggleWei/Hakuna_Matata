#include "libxml/globals.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xmlstring.h"
#include "libxml/xmlversion.h"
#include "muggle/c/muggle_c.h"

xmlDocPtr parse_doc(const char *filepath, const char *keyword)
{
	xmlDocPtr doc = xmlParseFile(filepath);
	if (doc == NULL)
	{
		LOG_ERROR("Document parsed failed");
		return NULL;
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (cur == NULL)
	{
		LOG_ERROR("Empty document");
		goto parse_doc_exit;
	}

	if (xmlStrcmp(cur->name, (const xmlChar*)"story"))
	{
		LOG_ERROR("Document of the wrong type, root node != story");
		goto parse_doc_exit;
	}

	cur = cur->children;
	while (cur)
	{
		if (xmlStrcmp(cur->name, (const xmlChar*)"storyinfo") == 0)
		{
			LOG_INFO("insert <keyword>%s</keyword> into %s", keyword, cur->name);
			xmlNewTextChild(cur, NULL, (xmlChar*)"keyword", (xmlChar*)keyword);
		}

		cur = cur->next;
	}

	return doc;

parse_doc_exit:
	if (doc)
	{
		xmlFreeDoc(doc);
		doc = NULL;
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	LIBXML_TEST_VERSION

	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	if (argc <= 2)
	{
		LOG_ERROR("Usage: %s <xml filepath> <keyword>", argv[0]);
		exit(1);
	}

	xmlDocPtr doc = parse_doc(argv[1], argv[2]);

	if (doc)
	{
		xmlSaveFormatFile("new_story.xml", doc, 1);
		xmlFreeDoc(doc);
	}

	return 0;
}
