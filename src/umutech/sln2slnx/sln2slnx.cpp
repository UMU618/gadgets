// You can compile this code, and run it under other OS, but it will only
// process MSVC's .sln files.

#include <set>
#include <string>
#include <string_view>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/nowide/args.hpp>
#include <boost/nowide/cstdio.hpp>
#include <boost/nowide/filesystem.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/nowide/iostream.hpp>

namespace al = boost::algorithm;
namespace fs = boost::filesystem;
namespace nw = boost::nowide;

using nw::cerr;
using nw::cout;
using nw::ifstream;
using nw::ofstream;

namespace {

#pragma region CollectTargetFiles
bool IsTargetFile(const fs::path& path) {
  return al::iequals(".sln", path.extension().string());
}

void CollectTargetFile(const fs::path& path, std::set<fs::path>& filenames) {
  auto status = fs::status(path);
  if (!fs::exists(status)) {
    cerr << "File " << path << " doesn't exist!" << '\n';
    return;
  }

  if (fs::is_directory(status)) {
    for (const auto& child : fs::recursive_directory_iterator(path)) {
      if (!fs::is_directory(child.status())) {
        if (IsTargetFile(child.path().extension().string())) {
          filenames.insert(child.path());
        } else {
#if _DEBUG
          cout << "Skip file " << child.path() << '\n';
#endif
        }
      }
    }
  } else if (IsTargetFile(path.extension().string())) {
    filenames.insert(path);
  } else {
#if _DEBUG
    cout << "Skip file " << path << '\n';
#endif
  }
}
#pragma endregion

#pragma region ProcessTargetFiles
// AI generated codes, may be ugly.
std::string EscapeXml(const std::string& data) {
  std::string buffer;
  buffer.reserve(data.size());
  for (char c : data) {
    switch (c) {
      case '&':
        buffer.append("&amp;");
        break;
      case '<':
        buffer.append("&lt;");
        break;
      case '>':
        buffer.append("&gt;");
        break;
      case '\"':
        buffer.append("&quot;");
        break;
      case '\'':
        buffer.append("&apos;");
        break;
      default:
        buffer.append(1, c);
        break;
    }
  }
  return buffer;
}

// UMU: sln uses backslash as path separator, while slnx uses slash.
std::string ReplaceBackslashWithSlash(const std::string& str) {
  return al::replace_all_copy(str, "\\", "/");
}

// AI generated codes, may be ugly.
struct ConfigurationInfo {
  std::set<std::string> build_types;
  std::set<std::string> platforms;
};

ConfigurationInfo ExtractConfigurations(const std::string& content) {
  ConfigurationInfo info;

  std::size_t section_start =
      content.find("GlobalSection(SolutionConfigurationPlatforms)");
  if (section_start == std::string::npos) {
    return info;
  }

  std::size_t section_end = content.find("EndGlobalSection", section_start);
  if (section_end == std::string::npos) {
    return info;
  }

  std::string section =
      content.substr(section_start, section_end - section_start);

  std::size_t pos = 0;
  for (;;) {
    std::size_t line_start = section.find('\n', pos);
    if (line_start == std::string::npos) {
      break;
    }
    line_start++;

    std::size_t line_end = section.find('\n', line_start);
    if (line_end == std::string::npos) {
      line_end = section.size();
    }

    std::string line = section.substr(line_start, line_end - line_start);

    std::size_t pipe = line.find('|');
    if (pipe == std::string::npos) {
      pos = line_end;
      continue;
    }

    std::size_t eq = line.find('=', pipe);
    if (eq == std::string::npos) {
      pos = line_end;
      continue;
    }

    // Extract build type
    std::string build_type = line.substr(0, pipe);
    // Remove leading whitespace
    while (!build_type.empty() &&
           (build_type.front() == ' ' || build_type.front() == '\t')) {
      build_type.erase(build_type.begin());
    }
    // Remove trailing whitespace
    while (!build_type.empty() &&
           (build_type.back() == ' ' || build_type.back() == '\t')) {
      build_type.pop_back();
    }
    if (!build_type.empty()) {
      info.build_types.insert(build_type);
    }

    // Extract platform
    std::size_t platform_start = pipe + 1;
    std::string platform = line.substr(platform_start, eq - platform_start);
    while (!platform.empty() &&
           (platform.back() == ' ' || platform.back() == '\t')) {
      platform.pop_back();
    }
    if (platform == "Win32") {
      platform = "x86";
    }
    if (!platform.empty()) {
      info.platforms.insert(platform);
    }

    pos = line_end;
  }

  return info;
}

struct ProjectInfo {
  std::string path;
  std::string id;
};

// AI generated codes, may be ugly.
std::vector<ProjectInfo> ExtractProjects(const std::string& content) {
  std::vector<ProjectInfo> projects;
  std::size_t pos = 0;

  for (;;) {
    std::size_t start = content.find("Project(", pos);
    if (start == std::string::npos) {
      break;
    }

    std::size_t first_quote = content.find("\"", start);
    if (first_quote == std::string::npos) {
      break;
    }

    std::size_t second_quote = content.find("\"", first_quote + 1);
    if (second_quote == std::string::npos) {
      break;
    }

    std::size_t third_quote = content.find("\"", second_quote + 1);
    if (third_quote == std::string::npos) {
      break;
    }

    std::size_t fourth_quote = content.find("\"", third_quote + 1);
    if (fourth_quote == std::string::npos) {
      break;
    }

    std::size_t fifth_quote = content.find("\"", fourth_quote + 1);
    if (fifth_quote == std::string::npos) {
      break;
    }

    std::size_t sixth_quote = content.find("\"", fifth_quote + 1);
    if (sixth_quote == std::string::npos) {
      break;
    }

    std::size_t seventh_quote = content.find("\"", sixth_quote + 1);
    if (seventh_quote == std::string::npos) {
      break;
    }

    std::size_t eighth_quote = content.find("\"", seventh_quote + 1);
    if (eighth_quote == std::string::npos) {
      break;
    }

    std::string project_id =
        content.substr(seventh_quote + 1, eighth_quote - seventh_quote - 1);

    if (!project_id.empty() && project_id.front() == '{') {
      project_id = project_id.substr(1);
    }
    if (!project_id.empty() && project_id.back() == '}') {
      project_id.pop_back();
    }

    // UMU: sln uses uppercase Id, while slnx uses lowercase Id.
    al::to_lower(project_id);

    std::string project_path =
        content.substr(fifth_quote + 1, sixth_quote - fifth_quote - 1);

    ProjectInfo info;
    info.path = ReplaceBackslashWithSlash(project_path);
    info.id = std::move(project_id);
    projects.emplace_back(info);

    pos = fourth_quote;
  }

  return projects;
}

std::string BuildSlnx(const ConfigurationInfo& config_info,
                      const std::vector<ProjectInfo>& projects) {
  if (config_info.platforms.empty() && projects.empty()) {
    return "<Solution />\r\n";
  }

  std::string xml;
  xml.reserve(1024);  // UMU: mostly enough for a typical slnx file.
  xml = "<Solution>\r\n  <Configurations>\r\n";

  // Only add BuildType elements if there are non-standard build types
  // Standard build types are Debug and Release
  bool has_non_standard_build_types = false;
  for (const auto& build_type : config_info.build_types) {
    if (build_type != "Debug" && build_type != "Release") {
      has_non_standard_build_types = true;
      break;
    }
  }

  if (has_non_standard_build_types) {
    for (const auto& build_type : config_info.build_types) {
      xml += "    <BuildType Name=\"" + EscapeXml(build_type) + "\" />\r\n";
    }
  }

  for (const auto& platform : config_info.platforms) {
    xml += "    <Platform Name=\"" + EscapeXml(platform) + "\" />\r\n";
  }
  xml += "  </Configurations>\r\n";

  for (const auto& project : projects) {
    xml += "  <Project Path=\"" + EscapeXml(project.path) + "\"";
    if (!project.id.empty()) {
      xml += " Id=\"" + EscapeXml(project.id) + "\"";
    }
    xml += " />\r\n";
  }
  xml += "</Solution>\r\n";
  return xml;
}

bool ProcessTargetFile(const fs::path& filename) {
  cout << "Processing " << filename << '\n';

  ifstream input(filename, std::ios::binary);
  if (!input.is_open()) {
    cerr << "  Failed to open " << filename << '\n';
    return false;
  }

  std::string content((std::istreambuf_iterator<char>(input)),
                      std::istreambuf_iterator<char>());
  input.close();

  ConfigurationInfo config_info = ExtractConfigurations(content);
  std::vector<ProjectInfo> projects = ExtractProjects(content);
  std::string slnx_content = BuildSlnx(config_info, projects);

  fs::path output_path = filename;
  output_path.replace_extension(".slnx");
  ofstream output(output_path, std::ios::binary);
  if (!output.is_open()) {
    cerr << "  Failed to create " << output_path << '\n';
    return false;
  }
  output << slnx_content;
  output.close();
  cout << "  Successfully converted to " << output_path.filename() << '\n';

  return true;
}
#pragma endregion

}  // namespace

int main(int argc, char* argv[]) try {
  boost::nowide::args _(argc, argv);
  boost::nowide::nowide_filesystem();

  if (argc != 2) {
    cout << "Convert .sln files to .slnx\n"
            "Usage: "
         << fs::path{argv[0]}.stem().string()
         << " <file_or_directory>\n"
            "Note: This is an AI generated program, it's NOT perfect!\n"
            "      You should use `dotnet sln migrate`.\n";
    return EXIT_SUCCESS;
  }

  std::set<fs::path> filenames;
  for (int i = 1; i < argc; ++i) {
    CollectTargetFile(fs::path{argv[i]}, filenames);
  }

  for (const auto& filename : filenames) {
    ProcessTargetFile(filename);
  }
} catch (const std::exception& e) {
  cerr << "Error: " << e.what() << '\n';
  return EXIT_FAILURE;
} catch (...) {
  cerr << "Unknown error\n";
  return EXIT_FAILURE;
}
