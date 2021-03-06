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
	xsltStylesheetPtr stylesheet;
};

static int xml_gc(lua_State *L)
{
	struct xml_document *user = (struct xml_document *)lua_touserdata(L, 1);
	if(user->stylesheet != NULL)
		xsltFreeStylesheet(user->stylesheet);
	else if(user->doc != NULL)
		xmlFreeDoc(user->doc);
	user->doc = NULL;
	user->stylesheet = NULL;
	return 0;
}

static int l_parse_xml(lua_State *L)
{
	const char *string = luaL_checkstring(L, 1);
	struct xml_document *user;

	printf("parse_xml()\n");

	user = (struct xml_document *)lua_newuserdata(L, sizeof(*user));
	luaL_getmetatable(L, XML_META_TABLE);
	lua_setmetatable(L, -2);
	user->doc = NULL;
	user->stylesheet = NULL;

	user->doc = xmlParseDoc((const xmlChar *)string);
	if(user->doc == NULL) {
		luaL_error(L, "failed to load XML document string: %s", string);
		return 0;
	}
	printf("Parsed.\n");

	return 1;
}

static int l_parse_xml_file(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);
	struct xml_document *user;

	printf("parse_xml_file()\n");

	user = (struct xml_document *)lua_newuserdata(L, sizeof(*user));
	luaL_getmetatable(L, XML_META_TABLE);
	lua_setmetatable(L, -2);
	user->doc = NULL;
	user->stylesheet = NULL;

	user->doc = xmlParseFile(filename);
	if(user->doc == NULL) {
		luaL_error(L, "failed to load XML document file: %s", filename);
		return 0;
	}
	printf("Parsed.\n");

	return 1;
}

static int l_dump_xml(lua_State *L)
{
	const struct xml_document *user = (const struct xml_document *)luaL_checkudata(L, 1, XML_META_TABLE);
	xmlChar *dump;
	int size;

	printf("dump_xml()\n");

	luaL_argcheck(L, user != NULL, 1, "'xml document' expected");

	xmlDocDumpMemory(user->doc, &dump, &size);
	if(dump == NULL) {
		luaL_error(L, "failed to load XML document: %s", "bad");
		return 0;
	}
	lua_pushlstring(L, (const char *)dump, size);
	xmlFree(dump);
	dump = NULL;
	printf("Dumped.\n");

	return 1;
}

static int l_transform_xml(lua_State *L)
{
	const struct xml_document *user = (const struct xml_document *)luaL_checkudata(L, 1, XML_META_TABLE);
	struct xml_document *transform = (struct xml_document *)luaL_checkudata(L, 2, XML_META_TABLE);
	struct xml_document *output;

	printf("transform_xml()\n");
	luaL_argcheck(L, user != NULL, 1, "'xml document' expected");
	luaL_argcheck(L, transform != NULL, 2, "'xml document' expected");

	output = (struct xml_document *)lua_newuserdata(L, sizeof(*user));
	output->doc = NULL;
	output->stylesheet = NULL;
	luaL_getmetatable(L, XML_META_TABLE);
	lua_setmetatable(L, -2);

	if(transform->stylesheet == NULL)
		transform->stylesheet = xsltParseStylesheetDoc(transform->doc);
	output->doc = xsltApplyStylesheet(transform->stylesheet, user->doc, NULL);
	if(output->doc == NULL) {
		//luaL_error(L, "failed to load XML document: %s", filename);
		//return 0;
	}
	printf("Transformed.\n");

	return 1;
}

static const struct luaL_Reg xmllib[] = {
	{"parse_xml", l_parse_xml},
	{"parse_xml_file", l_parse_xml_file},
	{"dump_xml", l_dump_xml},
	{"transform_xml", l_transform_xml},
	{NULL, NULL},
};

int luaopen_xml(lua_State *L)
{
	luaL_newmetatable(L, XML_META_TABLE);

	lua_pushliteral(L, "__gc");
	lua_pushcfunction(L, xml_gc);
	lua_settable(L, -3);

	luaL_newlib(L, xmllib);
	return 1;
}
