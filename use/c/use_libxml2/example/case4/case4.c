#include "libxml/globals.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xmlstring.h"
#include "libxml/xmlversion.h"
#include "muggle/c/muggle_c.h"

xmlDocPtr parse_doc(const char *filepath, const char *uri)
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

	xmlNodePtr newnode = xmlNewTextChild(
			cur, NULL, (const xmlChar*)"reference", NULL);
	xmlAttrPtr newattr = xmlNewProp(
			newnode, (const xmlChar*)"uri", (const xmlChar*)uri);
	LOG_INFO("insert attr: %s", newattr->name);

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
		LOG_ERROR("Usage: %s <xml filepath> <uri>", argv[0]);
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
