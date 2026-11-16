#include <godot_cpp/core/class_db.hpp>

#include "gdexample.hpp"

using namespace godot;


GDExample::GDExample() {
    enabled = false;
}

GDExample::~GDExample() {}


bool GDExample::is_enabled() const {
    return enabled;
}

void GDExample::_bind_methods() {
    ClassDB::bind_method(D_METHOD("is_enabled"), &GDExample::is_enabled);
}
