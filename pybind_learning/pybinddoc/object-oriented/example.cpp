// 创建C++数据结构
#include <pybind11/pybind11.h>

namespace py = pybind11;

class Pet {
public:
    Pet(const std::string &name) : name(name) { }
    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }
    static int add(int a, int b){ return a+b;}

private:
    std::string name;
};


PYBIND11_MODULE(example, m) {
    py::class_<Pet>(m, "Cat")
        .def(py::init<const std::string &>())
        // .def_property_readonly("name", &Pet::getName, &Pet::setName)
        .def_property_readonly("name", &Pet::getName, &Pet::setName)
        // .def("setName", &Pet::setName)
        // .def("getName", &Pet::getName)
        .def_static("add", &Pet::add);
}

//  class_ 创建c++ bindings class 或sturce .def 构造函数 py::init()实现 包装对应的构造器