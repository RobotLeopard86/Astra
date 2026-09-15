R"(
inline astra::TypeId {{name}}::ASTRA__gettypeid() const {
    return astra::TypeId::get<{{name}}>();
}
)"