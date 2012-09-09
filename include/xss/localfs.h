#ifndef XSS_LOCAL_FS_HH
#define XSS_LOCAL_FS_HH

#include "object_model.h"

namespace xkp{

class local_filesystem : public IFileSystem
  {
    public:
      virtual fs::path   locate(const str& filename, fs::path base_path);
      virtual DataReader load_data(const str& filename, fs::path base_path);
      virtual DataReader load_data(fs::path file);
      virtual str        load_file(const str& filename, fs::path base_path);
      virtual str        load_file(fs::path file);
    private:
      typedef std::vector<fs::path> path_list;

      path_list search_path_;
  };

}

#endif