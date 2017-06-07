#include <stdio.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <libxslt/xslt.h>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>


int main(int argc, char** argv)
{
	char *docname, *stylesheet;
	xmlDocPtr doc, output;
	xmlNodePtr cur;
	xsltStylesheetPtr transform;
	const char *params[] = {
		NULL,
	};

	if(argc < 3) {
		fprintf(stderr, "%s xml stylesheet\n", argv[0]);
		return -1;
	}

	docname = argv[1];
	stylesheet = argv[2];

	/* Recommended by libxslt tutorial */
	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;

	//doc = xmlParseDoc((const xmlChar *)string);
	doc = xmlParseFile(docname);
	if(doc == NULL) {
		fprintf(stderr, "Failed to parse document: %s\n", docname);
		return -1;
	}

	cur = xmlDocGetRootElement(doc);
	if(cur == NULL) {
		fprintf(stderr, "Empty document: %s\n", docname);
		xmlFreeDoc(doc);
		return -1;
	}

	if(xmlStrcmp(cur->name, (const xmlChar *)"hello-world")) {
		fprintf(stderr, "Wrong top-level element: %s\n", cur->name);
		xmlFreeDoc(doc);
		return -1;
	}

	transform = xsltParseStylesheetFile((const xmlChar *)stylesheet);

	output = xsltApplyStylesheet(transform, doc, params);

	xsltSaveResultToFile(stdout, output, transform);

	cur = NULL;

	xsltFreeStylesheet(transform);
	xmlFreeDoc(output);
	xmlFreeDoc(doc);
	transform = NULL;
	output = NULL;
	doc = NULL;

	xsltCleanupGlobals();
	xmlCleanupParser();

	return 0;
}
