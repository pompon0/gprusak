#include <map>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "gprusak/types.h"
#include "gprusak/read_file.h"
#include "gprusak/string.h"
#include "gprusak/log.h"

using namespace gprusak::types;

ABSL_FLAG(str,output_hdr,"","");
ABSL_FLAG(str,output_src,"","");
ABSL_FLAG(str,namespace,"","");
ABSL_FLAG(vec<str>,inputs,{},"");

//TODO: this is linear, but terribly slow. Improve.
INL inline str generate_array(const Bytes &data) {
  vec<str> x;
  for(auto b : data) x.push_back(gprusak::fmt("%",int(b)));
  return gprusak::join(", ",x);
}

int main(int argc, char **argv) {
  gprusak::StreamLogger _(std::cerr);
  absl::ParseCommandLine(argc,argv);

  auto namespace_ = absl::GetFlag(FLAGS_namespace);
  auto output_hdr_path = absl::GetFlag(FLAGS_output_hdr);
  auto output_src_path = absl::GetFlag(FLAGS_output_src);
  if(output_hdr_path=="") gprusak::error("output_hdr is required");
  if(output_src_path=="") gprusak::error("output_src is required");
  vec<str> hdr_arrays;
  vec<str> src_arrays;

  for(auto i : absl::GetFlag(FLAGS_inputs)) {
    auto p = i.find_first_of(":");
    auto key = i.substr(0,p);
    auto path = i.substr(p+1);
    auto data = gprusak::read_file(path);
    hdr_arrays.push_back(gprusak::fmt("extern uint8_t %[%];",key,data.size()));
    src_arrays.push_back(gprusak::fmt("uint8_t %[%] = { % };",key,data.size(),generate_array(data)));
  }
  auto includes = "#include <cstdint>\n";
  auto hdr = gprusak::fmt("%namespace % {\n%\n}",includes,namespace_,gprusak::join("\n",hdr_arrays));
  auto src = gprusak::fmt("%namespace % {\n%\n}",includes,namespace_,gprusak::join("\n",src_arrays));

  gprusak::write_file(output_hdr_path,str_bytes(hdr));
  gprusak::write_file(output_src_path,str_bytes(src));
  return 0;
}
