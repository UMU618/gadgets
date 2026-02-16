// You can compile this code, and run it under other OS, but it will only
// process Windows' .dmp files properly.

#include <set>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/nowide/args.hpp>
#include <boost/nowide/filesystem.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/nowide/iostream.hpp>

namespace fs = boost::filesystem;

using boost::nowide::cerr;
using boost::nowide::cout;
using boost::nowide::ifstream;

bool IsDumpFile(const fs::path& path) {
  return boost::algorithm::iequals(".dmp", path.extension().string());
}

void CollectDumpFile(const fs::path& path, std::set<fs::path>& filenames) {
  auto status = fs::status(path);
  if (!fs::exists(status)) {
    cerr << "File " << path << " doesn't exist!" << '\n';
    return;
  }

  if (fs::is_directory(status)) {
    for (const auto& child : fs::recursive_directory_iterator(path)) {
      if (!fs::is_directory(child.status())) {
        if (IsDumpFile(child.path().extension().string())) {
          filenames.insert(child.path());
        } else {
#if _DEBUG
          cout << "Skip file " << child.path() << '\n';
#endif
        }
      }
    }
  } else if (IsDumpFile(path.extension().string())) {
    filenames.insert(path);
  } else {
#if _DEBUG
    cout << "Skip file " << path << '\n';
#endif
  }
}

bool IsKernelDump(const fs::path& filename) {
  struct MinidumpHeader {
    std::uint32_t Signature;
    std::uint32_t Version;
    std::uint32_t NumberOfStreams;
    std::uint32_t StreamDirectoryRva;
    std::uint32_t CheckSum;
    union {
      std::uint32_t Reserved;
      std::uint32_t TimeDateStamp;
    };
    std::uint64_t Flags;
  };
  constexpr std::uint32_t kMinidumpSignature = 'PMDM';

  auto size = fs::file_size(filename);
  if (size < sizeof(MinidumpHeader)) {
    cerr << "Corrupted dump file: " << filename << '\n';
    return false;
  }

  ifstream f(filename);

  MinidumpHeader header;
  f.read(reinterpret_cast<char*>(&header), sizeof(header));
  if ('EGAP' == header.Signature && '46UD' == header.Version) {
    return true;
  }
  if (kMinidumpSignature != header.Signature) {
    cerr << "Invalid dump file: " << filename << '\n';
    return false;
  }
  cout << "Minidump file: " << filename << " with flags " << std::hex
       << header.Flags << '\n';
  return false;
}

void ProcessDumpFile(const fs::path& filename) {
  if (IsKernelDump(filename)) {
    cout << "Kernel dump file: " << filename << '\n';
  }
}

int main(int argc, char* argv[]) try {
  boost::nowide::args _(argc, argv);
  boost::nowide::nowide_filesystem();

  cout << std::showbase;

  std::set<fs::path> filenames;
  for (int i = 1; i < argc; ++i) {
    CollectDumpFile(fs::path{argv[i]}, filenames);
  }

  for (const auto& filename : filenames) {
    ProcessDumpFile(filename);
  }
} catch (const std::exception& e) {
  cerr << "Error: " << e.what() << '\n';
} catch (...) {
  cerr << "Unknown error\n";
}
