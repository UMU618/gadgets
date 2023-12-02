add_requires("fmt")

target("count_lines_fmt")
  set_kind("binary")
  add_defines("USE_FMTLIB")
  add_files("../../src/count_lines/count_lines.cpp")
  add_packages("fmt")
  if is_plat("windows") then
    add_links("shell32")
  end