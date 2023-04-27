#include "libxml/globals.h"
#include "libxml/tree.h"
#include "libxml/xmlstring.h"
#include "libxml/xmlversion.h"
#include "muggle/c/muggle_c.h"

void get_reference(xmlNodePtr cur)
{
	xmlChar *uri = NULL;
	cur = cur->children;
	while (cur)
	{
		if (xmlStrcmp(cur->name, (const xmlChar*)"reference") == 0)
		{
			uri = xmlGetProp(cur, (const xmlChar*)"uri");
			LOG_INFO("uri: %s", uri);
			xmlFree(uri);
		}
		cur = cur->next;
	}
}

void parse_doc(const char *filepath)
{
	xmlDocPtr doc = xmlParseFile(filepath);
	if (doc == NULL)
	{
		LOG_ERROR("Document parsed failed");
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

	get_reference(cur);

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
		LOG_ERROR("Usage: %s <xml filepath>\n"
			"\tNOTE: You can use the xml file that example case4 generated",
			argv[0]);
		exit(1);
	}

	parse_doc(argv[1]);

	return 0;
}
