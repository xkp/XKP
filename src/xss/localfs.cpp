
#include <xss/localfs.h>
#include <xss/xml_data_reader.h>

using namespace xkp;

//utils
bool check(const fs::path path, const fs::path file, fs::path& result)
  {
    fs::path res = path / file;
    if (fs::exists(res))
      {
        result = fs::system_complete(res);
        return true;
      }
    return false;
  }

//local_filesystem
fs::path local_filesystem::locate(const str& filename, fs::path base_path)
  {
    fs::path result;
    if (!base_path.empty() && check(base_path, filename, result))
      {
        fs::path result = base_path / filename;
        if (fs::exists(result))
          return result.normalize();
      }

    path_list::iterator it = search_path_.begin();
    path_list::iterator nd = search_path_.end();

    for(; it != nd; it++)
      {
        if (check(*it, filename, result))
          return result;
      }

    check(fs::path(), filename, result);
    return result;
  }

DataReader local_filesystem::load_data(const str& filename, fs::path base_path)
  {
    fs::path df = locate(filename, base_path);
    if (df.empty())
      return DataReader();
    return load_data(df);
  }

DataReader local_filesystem::load_data(fs::path file)
  {
    xml_data_reader* result = new xml_data_reader;
    if (result->load(load_file(file)))
      return DataReader(result);

    delete result;
    return DataReader();
  }

str local_filesystem::load_file(const str& filename, fs::path base_path)
  {
    fs::path df = locate(filename, base_path);
    if (df.empty())
      return str();

    return load_file(df);
  }

str local_filesystem::load_file(fs::path file)
  {
    std::ifstream ifs(file.string().c_str());
		if (!ifs.good())
      return str();

    str result;
    char buffer[2048];
    while(ifs.good())
      {
        ifs.getline(buffer, 1024);
        result += buffer;
        result += '\n';
      }

    return result;
  }
