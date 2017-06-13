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

struct xml_document {
	xmlDocPtr doc;
};

static int l_parse_xml(lua_State *L)
{
	const char *filename = luaL_checklstring(L, 1, NULL);
	xmlDocPtr doc;
	struct xml_document *user;

	printf("parse_xml()\n");
	doc = xmlParseDoc((const xmlChar *)filename);
	if(doc == NULL) {
		luaL_error(L, "failed to load XML document: %s", filename);
		return 0;
	}
	printf("Parsed.\n");
	user = (struct xml_document *)lua_newuserdata(L, sizeof(*user));
	//xmlFreeDoc(doc);
	//doc = NULL;
	user->doc = doc;

	return 1;
}

static int l_dump_xml(lua_State *L)
{
	const struct xml_document *user = (const struct xml_document *)lua_touserdata(L, 1);
	xmlDocPtr doc;
	xmlChar *dump;
	int size;

	printf("dump_xml()\n");
	luaL_argcheck(L, user != NULL, 1, "'xml document' expected");
	doc = user->doc;
	xmlDocDumpMemory(doc, &dump, &size);
	if(dump == NULL) {
		luaL_error(L, "failed to load XML document: %s", "bad");
		//return 0;
	}
	lua_pushstring(L, (const char *)dump);
	lua_pushliteral(L, "Return value");
	//xmlFree(dump);
	printf("Dumped.\n");

	return 2;
}

static int l_transform_xml(lua_State *L)
{
	/* TODO check userdata type */
	const struct xml_document *user = (const struct xml_document *)lua_touserdata(L, 1);
	const struct xml_document *transform2 = (const struct xml_document *)lua_touserdata(L, 2);
	xsltStylesheetPtr transform;
	xmlDocPtr doc, output;

	printf("transform_xml()\n");
	luaL_argcheck(L, user != NULL, 1, "'xml document' expected");
	luaL_argcheck(L, transform2 != NULL, 2, "'xml document' expected");
	transform = xsltParseStylesheetFile((const xmlChar *)"");
	doc = user->doc;
	output = xsltApplyStylesheet(transform, doc, NULL);
	if(output == NULL) {
		//luaL_error(L, "failed to load XML document: %s", filename);
		//return 0;
	}
	printf("Transformed.\n");

	return 1;
}

#if 0
static const struct {
	const char *name;
	const lua_CFunction func;
} xmllib[] = {
#endif
static const struct luaL_Reg xmllib[] = {
	{"parse_xml", l_parse_xml},
	{"dump_xml", l_dump_xml},
	{"transform_xml", l_transform_xml},
	{NULL, NULL},
};

int luaopen_xml(lua_State *L)
{
#if 0
	//lua_newtable(L);
	lua_createtable(L, 0, sizeof(xmllib)/sizeof(xmllib[0]));
	for(int i = 0; i < sizeof(xmllib)/sizeof(xmllib[0]); i++) {
		lua_pushstring(L, xmllib[i].name);
		lua_pushcfunction(L, xmllib[i].func);
		lua_settable(L, -3);
	}
#endif
	luaL_newlib(L, xmllib);
	return 1;
}

int main(int argc, char** argv)
{
	char *docname, *stylesheet;
	xmlDocPtr doc, output;
	xmlNodePtr cur;
	xsltStylesheetPtr transform;
	const char *params[] = {
		NULL,
	};
	char *buf = "print(\"hello, lua!\", circumference(5)) print(circumference(\"2\")) print(circumference(nil))";
	//char *buf2 = "print(\"hello, lua!";
	char *buf3 = "parse_xml(\"<a/>\")parse_xml(\">\")";

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

	lua_State *L = luaL_newstate();
	if(L == NULL) {
		fprintf(stderr, "Failed to initialize Lua interpreter\n");
		return -1;
	}
	luaL_openlibs(L);

	lua_pushcfunction(L, l_circumference);
	lua_setglobal(L, "circumference");
	lua_pushcfunction(L, l_parse_xml);
	lua_setglobal(L, "parse_xml");
	lua_pushcfunction(L, l_transform_xml);
	lua_setglobal(L, "transform_xml");

	luaopen_xml(L);
	lua_setglobal(L, "xml");

	int error = luaL_loadbuffer(L, buf, strlen(buf), "body") || lua_pcall(L, 0, 0, 0);
	if(error) {
		fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

#if 0
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

	return 0;
}
