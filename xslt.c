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


#define XML_META_TABLE "XMLLib.xml"

struct xml_document {
	xmlDocPtr doc;
};

static int xml_gc(lua_State *L)
{
	struct xml_document *user = (struct xml_document *)lua_touserdata(L, 1);
	if(user->doc != NULL)
		xmlFreeDoc(user->doc);
	user->doc = NULL;
	return 0;
}

//static int l_parse_xml(lua_State *L)
int l_parse_xml(lua_State *L)
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
	luaL_getmetatable(L, XML_META_TABLE);
	lua_setmetatable(L, -2);
	//xmlFreeDoc(doc);
	//doc = NULL;
	user->doc = doc;

	return 1;
}

static int l_parse_xml_file(lua_State *L)
{
	const char *filename = luaL_checklstring(L, 1, NULL);
	struct xml_document *user;

	printf("parse_xml_file()\n");

	user = (struct xml_document *)lua_newuserdata(L, sizeof(*user));
	luaL_getmetatable(L, XML_META_TABLE);
	lua_setmetatable(L, -2);

	user->doc = xmlParseFile(filename);
	if(user->doc == NULL) {
		luaL_error(L, "failed to load XML document: %s", filename);
		return 0;
	}
	printf("Parsed.\n");

	return 1;
}

static int l_dump_xml(lua_State *L)
{
	const struct xml_document *user = (const struct xml_document *)luaL_checkudata(L, 1, XML_META_TABLE);
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
	xmlFree(dump);
	lua_pushliteral(L, "Return value");
	printf("Dumped.\n");

	return 2;
}

//static int l_transform_xml(lua_State *L)
int l_transform_xml(lua_State *L)
{
	/* TODO check userdata type */
	const struct xml_document *user = (const struct xml_document *)luaL_checkudata(L, 1, XML_META_TABLE);
	const struct xml_document *transform2 = (const struct xml_document *)luaL_checkudata(L, 2, XML_META_TABLE);
	xsltStylesheetPtr transform;
	struct xml_document *output;

	printf("transform_xml()\n");
	luaL_argcheck(L, user != NULL, 1, "'xml document' expected");
	luaL_argcheck(L, transform2 != NULL, 2, "'xml document' expected");

	output = (struct xml_document *)lua_newuserdata(L, sizeof(*user));
	output->doc = NULL;
	luaL_getmetatable(L, XML_META_TABLE);
	lua_setmetatable(L, -2);

	transform = xsltParseStylesheetDoc(transform2->doc);
	output->doc = xsltApplyStylesheet(transform, user->doc, NULL);
	//xsltFreeStylesheet(transform);
	if(output->doc == NULL) {
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
	{"parse_xml_file", l_parse_xml_file},
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
	luaL_newmetatable(L, XML_META_TABLE);
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, xml_gc);
	lua_settable(L, -3);
	luaL_newlib(L, xmllib);
	return 1;
}
