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
   }
}
build = {
   type = "builtin",
   modules = {
      creole = "creole.lua",
      xml = {
         incdirs = {
            "$(XML2_INCDIR)"
         },
         libdirs = {
            "$(XML2_LIBDIR)"
         },
         libraries = {
            "xml2"
         },
         sources = "xslt.c"
      },
      ["xml-test"] = "xml-test.lua"
   }
}
