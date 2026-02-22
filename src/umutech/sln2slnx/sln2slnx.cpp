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
std::string ReplaceBackslashWithSlash(const std::string& str) noexcept {
  return al::replace_all_copy(str, "\\", "/");
}

struct ConfigurationInfo {
  std::set<std::string> build_types;
  std::set<std::string> platforms;
};

struct ProjectInfo {
  std::string path;
  std::string id;
};

class SolutionParser {
 public:
  SolutionParser(std::string content) : content_(std::move(content)) {
    // Skip UTF-8 BOM if present
    if (content_.size() >= 3 && content_[0] == static_cast<char>(0xEF) &&
        content_[1] == static_cast<char>(0xBB) &&
        content_[2] == static_cast<char>(0xBF)) {
      pos_ = 3;
    }
  }

  // Line type enumeration for parsing
  enum class LineType {
    kProject,
    kProjectSection,
    kEndProjectSection,
    kEndProject,
    kGlobal,
    kGlobalSection,
    kEndGlobalSection,
    kEndGlobal,
    kVisualStudioVersion,
    kMinimumVisualStudioVersion,
    kCommentLine,
    kEmpty,
    kProperty
  };

  ConfigurationInfo ExtractConfigurations() {
    ConfigurationInfo info;
    std::size_t section_start =
        FindSectionStart("SolutionConfigurationPlatforms");
    if (section_start == std::string::npos) {
      return info;
    }

    std::size_t section_end = FindSectionEnd(section_start);
    if (section_end == std::string::npos) {
      return info;
    }

    std::string section =
        content_.substr(section_start, section_end - section_start);
    std::size_t section_pos = 0;

    for (;;) {
      std::size_t line_start = section.find('\n', section_pos);
      if (line_start == std::string::npos) {
        break;
      }
      line_start++;

      std::size_t line_end = section.find('\n', line_start);
      if (line_end == std::string::npos) {
        line_end = section.size();
      }

      // Handle CRLF by removing trailing '\r'
      std::size_t actual_end = line_end;
      if (actual_end > line_start && section[actual_end - 1] == '\r') {
        actual_end--;
      }

      std::string line = section.substr(line_start, actual_end - line_start);
      section_pos = line_end;

      ProcessConfigurationLine(line, info);
    }

    return info;
  }

  std::vector<ProjectInfo> ExtractProjects() {
    std::vector<ProjectInfo> projects;

    if (!SkipFormatLine()) {
      tarnished_ = true;
    }

    while (pos_ < content_.size()) {
      std::size_t line_start = pos_;
      std::size_t line_end = content_.find('\n', pos_);
      if (line_end == std::string::npos) {
        line_end = content_.size();
      }
      pos_ = line_end + 1;

      // UMU: Accept LF and CRLF as line endings, in case the solution file is
      // not well formatted. For example, CRLF is replace with LF by Git.
      if (line_end > line_start && content_[line_end - 1] == '\r') {
        line_end--;
      }

      std::string line = content_.substr(line_start, line_end - line_start);
      LineType line_type = GetLineType(line);

      if (line_type == LineType::kProject) {
        ProjectInfo info;
        if (ParseProjectLine(line, info)) {
          projects.emplace_back(info);
        }
      }
    }

    return projects;
  }

  bool IsTarnished() const { return tarnished_; }

 private:
  LineType GetLineType(const std::string& line) {
    std::string trimmed = al::trim_copy(line);
    if (trimmed.empty()) {
      return LineType::kEmpty;
    }
    if (trimmed[0] == '#') {
      return LineType::kCommentLine;
    }
    if (trimmed == "Global") {
      return LineType::kGlobal;
    }
    if (trimmed == "EndGlobal") {
      return LineType::kEndGlobal;
    }
    if (trimmed == "EndProject") {
      return LineType::kEndProject;
    }
    if (trimmed == "EndProjectSection") {
      return LineType::kEndProjectSection;
    }
    if (trimmed == "EndGlobalSection") {
      return LineType::kEndGlobalSection;
    }
    if (trimmed.substr(0, 8) == "Project(") {
      return LineType::kProject;
    }
    if (trimmed.substr(0, 14) == "ProjectSection(") {
      return LineType::kProjectSection;
    }
    if (trimmed.substr(0, 13) == "GlobalSection(") {
      return LineType::kGlobalSection;
    }
    if (trimmed.substr(0, 19) == "VisualStudioVersion") {
      return LineType::kVisualStudioVersion;
    }
    if (trimmed.substr(0, 25) == "MinimumVisualStudioVersion") {
      return LineType::kMinimumVisualStudioVersion;
    }
    return LineType::kProperty;
  }

  bool SkipFormatLine() {
    // Skip empty lines
    while (pos_ < content_.size()) {
      std::size_t line_start = pos_;
      std::size_t line_end = content_.find('\n', pos_);
      if (line_end == std::string::npos) {
        line_end = content_.size();
      }

      // Handle CRLF by removing trailing '\r'
      std::size_t actual_end = line_end;
      if (actual_end > line_start && content_[actual_end - 1] == '\r') {
        actual_end--;
      }

      std::string line = content_.substr(line_start, actual_end - line_start);
      al::trim(line);
      if (!line.empty()) {
        // Check if it's a format line
        if (line.find(
                "Microsoft Visual Studio Solution File, Format Version") ==
            std::string::npos) {
          return false;
        }

        pos_ = line_end + 1;
        return true;
      }
      pos_ = line_end + 1;
    }
    return false;
  }

  std::size_t FindSectionStart(const std::string& section_name) {
    std::string search_pattern = "GlobalSection(" + section_name + ")";
    return content_.find(search_pattern);
  }

  std::size_t FindSectionEnd(std::size_t section_start) {
    std::size_t end_pos = content_.find("EndGlobalSection", section_start);
    if (end_pos == std::string::npos) {
      return std::string::npos;
    }
    return end_pos;
  }

  void ProcessConfigurationLine(const std::string& line,
                                ConfigurationInfo& info) {
    std::string trimmed = al::trim_copy(line);
    if (trimmed.empty()) {
      return;
    }

    std::size_t pipe_pos = trimmed.find('|');
    if (pipe_pos == std::string::npos) {
      return;
    }

    std::size_t eq_pos = trimmed.find('=', pipe_pos);
    if (eq_pos == std::string::npos) {
      return;
    }

    // Extract build type
    std::string build_type = trimmed.substr(0, pipe_pos);
    al::trim(build_type);
    if (!build_type.empty()) {
      info.build_types.insert(build_type);
    }

    // Extract platform
    std::string platform = trimmed.substr(pipe_pos + 1, eq_pos - pipe_pos - 1);
    al::trim(platform);
    if (platform == "Win32") {
      platform = "x86";
    }
    if (!platform.empty()) {
      info.platforms.insert(platform);
    }
  }

  bool ParseProjectLine(const std::string& line, ProjectInfo& info) {
    // Find the quotes around the project path and id
    std::vector<std::size_t> quote_positions;
    for (std::size_t i = 0; i < line.size(); ++i) {
      if (line[i] == '"') {
        quote_positions.push_back(i);
      }
    }

    // We need at least 8 quotes for a valid project line
    if (quote_positions.size() < 8) {
      tarnished_ = true;
      return false;
    }

    // Extract project path (between 5th and 6th quote)
    std::size_t path_start = quote_positions[4] + 1;
    std::size_t path_end = quote_positions[5];
    std::string project_path = line.substr(path_start, path_end - path_start);

    // Extract project id (between 7th and 8th quote)
    std::size_t id_start = quote_positions[6] + 1;
    std::size_t id_end = quote_positions[7];
    std::string project_id = line.substr(id_start, id_end - id_start);

    // Process project id
    if (!project_id.empty() && project_id.front() == '{') {
      project_id = project_id.substr(1);
    }
    if (!project_id.empty() && project_id.back() == '}') {
      project_id.pop_back();
    }

    // UMU: sln uses uppercase Id, while slnx uses lowercase Id.
    al::to_lower(project_id);

    // Process project path
    info.path = ReplaceBackslashWithSlash(project_path);
    info.id = std::move(project_id);

    return true;
  }

 private:
  const std::string content_;
  std::size_t pos_{};
  int line_number_{1};
  bool tarnished_{};
};

// AI generated codes, may be ugly.
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

  SolutionParser parser(std::move(content));

  ConfigurationInfo config_info = parser.ExtractConfigurations();
  std::vector<ProjectInfo> projects = parser.ExtractProjects();
  std::string slnx_content = BuildSlnx(config_info, projects);

  if (parser.IsTarnished()) {
    cerr << "  Warning: Solution file may have formatting issues." << '\n';
  }

  fs::path output_path = filename;
  output_path.replace_extension(".slnx");
  ofstream output(output_path, std::ios::binary);
  if (!output.is_open()) {
    cerr << "  Failed to create " << output_path << '\n';
    return false;
  }
  output << slnx_content;
  output.close();
  cout << "  Successfully converted to " << output_path << '\n';

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
