#include <format>
#include <set>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/nowide/args.hpp>
#include <boost/nowide/filesystem.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/nowide/iostream.hpp>
#include <boost/program_options.hpp>

namespace nw = boost::nowide;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

std::size_t CountLines(const fs::path& filename, bool ignore_empty) noexcept {
  nw::ifstream f(filename);
  if (!f) {
    nw::cerr << "Can't open " << filename << std::endl;
    return 0;
  }

  std::size_t total_lines = 0;
  std::string line;
  while (f) {
    if (std::getline(f, line)) {
      std::string_view line_view{line};
      if (ignore_empty) {
        line_view = boost::algorithm::trim_copy(line_view);
      }
      if (!line_view.empty()) {
        ++total_lines;
      }
    }
  }
  f.close();
  return total_lines;
}

int main(int argc, char* argv[]) try {
  nw::args _(argc, argv);

  bool include_cpp;
  bool ignore_empty;

  po::options_description desc("Usage");
  // clang-format off
  desc.add_options()("help,h", "Produce help message")
    ("cpp",
      po::value<bool>(&include_cpp)->default_value(false),
      "Include C++ file extensions.")
    ("ext",
      po::value<std::vector<std::string>>()->composing()->multitoken(),
      "File extension included. Case sensitive!")
    ("ignore-empty",
      po::value<bool>(&ignore_empty)->default_value(false),
      "Ignore empty lines.")
    ("input,i",
      po::value<std::vector<std::string>>()->composing()->multitoken(),
      "Input path. Can be file or directory.");
  // clang-format on
  po::positional_options_description p;
  p.add("input", -1);
  po::variables_map vm;
  po::parsed_options parsed =
      po::command_line_parser(argc, argv).options(desc).positional(p).run();
  po::store(parsed, vm);
  po::notify(vm);

  if (vm.count("help")) {
    nw::cout << "count_lines @ 2022-12-25\n\n"
             << desc
             << "\nExamples:\n"
                "  count_lines --cpp=1 C:\\cpp\\\n"
                "  count_lines --ext \"\" -i C:\\cpp\\ C:\\js\\\n";
    return EXIT_SUCCESS;
  }

  std::set<std::string> exts;
  if (include_cpp) {
    exts.insert(".C");
    exts.insert(".c++");
    exts.insert(".cc");
    exts.insert(".cpp");
    exts.insert(".cppm");
    exts.insert(".cxx");
    exts.insert(".h");
    exts.insert(".h++");
    exts.insert(".hh");
    exts.insert(".hpp");
    exts.insert(".hxx");
    exts.insert(".inl");
    exts.insert(".ipp");
    exts.insert(".ixx");
    exts.insert(".tlh");
    exts.insert(".tli");
  }

  if (vm.count("ext")) {
    for (const auto& _ext : vm["ext"].as<std::vector<std::string>>()) {
      auto ext = boost::algorithm::trim_copy(_ext);
      // Support --ext "" to count those files without extension
      if (ext.empty() || ext.starts_with('.')) {
        exts.insert(ext);
      } else {
        exts.insert("." + ext);
      }
    }
  }

  nw::cout << std::format("cpp         : {}\n", include_cpp);
  nw::cout << "ext         :";
  for (const auto& ext : exts) {
    nw::cout << " " << ext;
  }
  nw::cout << "\n";
  nw::cout << std::format("ignore-empty: {}\n", ignore_empty);

  nw::nowide_filesystem();

  std::set<fs::path> filenames;

  if (vm.count("input")) {
    for (const auto& input : vm["input"].as<std::vector<std::string>>()) {
      fs::path path{fs::canonical(input)};
      auto status = fs::status(path);
      if (!fs::exists(status)) {
        nw::cerr << "File " << path << " doesn't exist!" << std::endl;
        continue;
      }

      if (fs::is_directory(status)) {
        for (const auto& child : fs::recursive_directory_iterator(path)) {
          if (!fs::is_directory(child.status())) {
            if (exts.contains(child.path().extension().string())) {
              filenames.insert(child.path());
            } else {
#if _DEBUG
              nw::cout << "Skip file " << child.path() << std::endl;
#endif
            }
          }
        }
      } else if (exts.contains(path.extension().string())) {
        filenames.insert(path);
      } else {
#if _DEBUG
        nw::cout << "Skip file " << path << std::endl;
#endif
      }
    }
  }

  std::size_t total_files{0};
  std::size_t total_lines{0};
  for (const auto& filename : filenames) {
    std::size_t lines = CountLines(filename, ignore_empty);
    ++total_files;
    total_lines += lines;
    nw::cout << "File " << filename << " has " << lines
             << (1 < lines ? " lines" : " line") << std::endl;
  }

  if (0 != total_files) {
    nw::cout << "Total files: " << total_files
             << "\nTotal lines: " << total_lines << std::endl;
  }
} catch (const std::exception& e) {
  nw::cerr << "Error: " << e.what() << '\n';
  return EXIT_FAILURE;
}
