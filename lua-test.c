#include <stdio.h>
#include <string.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <libxslt/xslt.h>
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>


static int l_circumference(lua_State *L)
{
	//double r = lua_tonumber(L, 1);
	printf("circumference()\n");
	double r = luaL_checknumber(L, 1);
	printf("r = %f\n", r);
	lua_pushnumber(L, 2*3.141592654*r);
	printf("c = %f\n", 2*3.141592654*r);
	return 1;
}


extern int luaopen_xml(lua_State *L);
//extern int l_parse_xml(lua_State *L);
//extern int l_transform_xml(lua_State *L);

int main(int argc, char** argv)
{
	char *docname, *stylesheet;
	xmlDocPtr doc, output;
	xmlNodePtr cur;
	xsltStylesheetPtr transform;
	const char *params[] = {
		NULL,
	};
	const char *buf = "print(\"hello, lua!\", circumference(5)) print(circumference(\"2\")) print(circumference(nil))";
	//const char *buf2 = "print(\"hello, lua!";
	const char *buf3 = "xml.parse_xml(\"<a/>\")xml.parse_xml(\">\")";

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

	lua_State *L = luaL_newstate();
	if(L == NULL) {
		fprintf(stderr, "Failed to initialize Lua interpreter\n");
		return -1;
	}
	luaL_openlibs(L);

	lua_pushcfunction(L, l_circumference);
	lua_setglobal(L, "circumference");
	//lua_pushcfunction(L, l_parse_xml);
	//lua_setglobal(L, "parse_xml");
	//lua_pushcfunction(L, l_transform_xml);
	//lua_setglobal(L, "transform_xml");

	luaopen_xml(L);
	lua_setglobal(L, "xml");

	int error;
#if 0
	int error = luaL_loadbuffer(L, buf, strlen(buf), "body") || lua_pcall(L, 0, 0, 0);
	if(error) {
		fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	error = luaL_loadbuffer(L, buf2, strlen(buf2), "body") || lua_pcall(L, 0, 0, 0);
	if(error) {
		fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
#endif

	error = luaL_loadbuffer(L, buf3, strlen(buf3), "body") || lua_pcall(L, 0, 0, 0);
	if(error) {
		fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	error = luaL_dofile(L, "xml-test.lua");
	if(error) {
		fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	lua_close(L);

	xsltCleanupGlobals();
	xmlCleanupParser();

	fflush(stdout);
	fclose(stdout);

	return 0;
}
