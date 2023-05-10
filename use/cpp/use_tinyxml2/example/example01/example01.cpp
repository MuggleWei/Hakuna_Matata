#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdexcept>
#include "tinyxml2.h"

#define TINYXML2_GET_ELEMENT(parent, name, element)                  \
	tinyxml2::XMLElement *element = parent->FirstChildElement(name); \
	if (element == nullptr) {                                        \
		fprintf(stderr, "failed found element '" name "'\n");        \
		return false;                                                \
	}

#define TINYXML2_GET_ATTR(element, attr, desc, target)            \
	{                                                             \
		const char *tmp_##target = element->Attribute(attr);      \
		if (tmp_##target == nullptr) {                            \
			fprintf(stderr, "failed get attribute '" desc "'\n"); \
			return false;                                         \
		}                                                         \
		target = tmp_##target;                                    \
	}

#define TINYXML2_GET_ATTR_BOOL(element, attr, desc, target)              \
	{                                                                    \
		std::string tmp_##target;                                        \
		TINYXML2_GET_ATTR(element, attr, desc, tmp_##target);            \
		if (tmp_##target == "True" || tmp_##target == "true" ||          \
			tmp_##target == "1") {                                       \
			target = true;                                               \
		} else if (tmp_##target == "False" || tmp_##target == "false" || \
				   tmp_##target == "0") {                                \
			target = false;                                              \
		} else {                                                         \
			fprintf(stderr, "failed convert '%s' to boolean\n",          \
					tmp_##target.c_str());                               \
			return false;                                                \
		}                                                                \
	}

#define TINYXML2_GET_ATTR_INT(element, attr, desc, target)              \
	{                                                                   \
		std::string tmp_##target;                                       \
		TINYXML2_GET_ATTR(element, attr, desc, tmp_##target);           \
		try {                                                           \
			target = std::stoi(tmp_##target);                           \
		} catch (std::invalid_argument const &ex) {                     \
			fprintf(stderr, "failed convert '%s' to int, except: %s\n", \
					tmp_##target.c_str(), ex.what());                   \
			return false;                                               \
		} catch (std::out_of_range const &ex) {                         \
			fprintf(stderr,                                             \
					"failed convert '%s' to int, "                      \
					"value out of range: %s\n",                         \
					tmp_##target.c_str(), ex.what());                   \
			return false;                                               \
		}                                                               \
	}

bool parse_xml_file(const char *filepath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filepath) != 0) {
		fprintf(stderr, "failed load config file: %s\n", filepath);
		return false;
	}

	tinyxml2::XMLElement *root = doc.FirstChildElement("Hello");
	if (root == nullptr) {
		fprintf(stderr, "failed found element 'Hello'");
		return false;
	}

	TINYXML2_GET_ELEMENT(root, "items", element_items);
	TINYXML2_GET_ELEMENT(element_items, "item", element_item);
	while (element_item) {
		int id = 0;
		bool hide = false;

		TINYXML2_GET_ATTR_INT(element_item, "id", "item.id", id);
		TINYXML2_GET_ATTR_BOOL(element_item, "hide", "item.hide", hide);

		fprintf(stdout, "id=%d, hide=%s\n", id, hide ? "true" : "false");

		element_item = element_item->NextSiblingElement("item");
	}

	return true;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <xml file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (!parse_xml_file(argv[1])) {
		fprintf(stderr, "failed parse xml file: %s", argv[1]);
		exit(EXIT_FAILURE);
	}

	return 0;
}
