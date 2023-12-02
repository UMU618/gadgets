set_project("gadgets")

set_config("buildir", "tmp")
set_languages("cxx20")
set_targetdir("bin")

add_rules("mode.debug", "mode.release")
if is_plat("windows") then
  add_cxflags("/utf-8")
  add_includedirs(os.getenv("BOOST_ROOT"))
  add_linkdirs(path.join(os.getenv("BOOST_ROOT"), "stage", "lib"))
else
  add_defines("Boost_USE_STATIC_LIB")
  add_requires("boost", {configs={program_options=true}})
  add_packages("boost")
end

includes(
  "count_lines"
)
