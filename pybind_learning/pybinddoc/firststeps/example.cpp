#include <pybind11/pybind11.h>

// 这里使用m绑定时可以自己设置参数，也可以设置无参数
using namespace pybind11::literals;
namespace py = pybind11;
int add(int i, int j=1) {
    return i + j;
}

PYBIND11_MODULE(example, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

    m.def("add", &add, "A function which adds two numbers",
      "i"_a=1, py::arg("j2")=3);
}
