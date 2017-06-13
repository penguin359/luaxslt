package = "xml"
version = "0.1-1"
source = {
   url = "git://github.com/penguin359/luaxslt.git"
}
description = {
   homepage = "https://github.com/penguin359/luaxslt",
   license = "MIT"
}
dependencies = {
   "lua ~> 5.2"
}
external_dependencies = {
   XML2 = {
      library = "xml2"
   },
   XSLT = {
      library = "xslt"
   }
}
build = {
   type = "builtin",
   modules = {
      creole = "creole.lua",
      xml = {
         incdirs = {
            "$(XML2_INCDIR)", "$(XSLT_INCDIR)"
         },
         libdirs = {
            "$(XML2_LIBDIR)", "$(XSLT_LIBDIR)"
         },
         libraries = {
            "xml2", "xslt"
         },
         sources = "xslt.c"
      },
      ["xml-test"] = "xml-test.lua"
   }
}
