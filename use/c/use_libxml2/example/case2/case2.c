#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xmlstring.h"
#include "libxml/xpath.h"
#include "muggle/c/muggle_c.h"

xmlDocPtr get_doc(const char *filepath)
{
	xmlDocPtr doc = xmlParseFile(filepath);
	if (doc == NULL)
	{
		LOG_ERROR("Document parsed failed");
		return NULL;
	}

	return doc;
}

xmlXPathObjectPtr get_nodeset(xmlDocPtr doc, const xmlChar *xpath)
{
	xmlXPathContextPtr context = NULL;
	xmlXPathObjectPtr result = NULL;

	context = xmlXPathNewContext(doc);
	if (context == NULL)
	{
		LOG_ERROR("Error in xmlXPathNewContext");
		return NULL;
	}

	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);

	if (result == NULL)
	{
		LOG_ERROR("Error in xmlXPathEvalExpression");
		return NULL;
	}

	if (xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		xmlXPathFreeObject(result);
		LOG_ERROR("No result");
		return NULL;
	}

	return result;
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

	xmlDocPtr doc = get_doc(argv[1]);
	if (doc == NULL)
	{
		LOG_ERROR("Get NULL doc");
		exit(1);
	}

	xmlXPathObjectPtr result = get_nodeset(doc, (const xmlChar*)"//keyword");
	xmlNodeSetPtr nodeset = NULL;
	xmlChar *keyword = NULL;
	if (result)
	{
		nodeset = result->nodesetval;
		for (int i = 0; i < nodeset->nodeNr; i++)
		{
			keyword = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
			LOG_INFO("keyword: %s", keyword);
			xmlFree(keyword);
		}
		xmlXPathFreeObject(result);
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();

	return 0;
}
