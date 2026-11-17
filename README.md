# webm-in-godot

A program aiming to add additional webm video support to Godot.

**NOTE:** This project is currently a work-in-progress with only a standalone VP9 decoder for WEBM outside of Godot which is in the process of being ported to GDExtension.

## External dependencies

This program uses the `godot-cpp` GDExtension bindings which are licensed under the MIT License. The license can be found under the `godot-cpp` submodule or on the [project's repository](https://github.com/godotengine/godot-cpp/tree/godot-4.2.2-stable).

This program depends on the `libwebm` and `libvpx` libraries which are licensed under the BSD-3-Clause License.

Licensing information for `libwebm` can be found on the [project's repository](https://github.com/webmproject/libwebm).

Licensing information for `libvpx` can be found on the [project's repository](https://github.com/webmproject/libvpx).

External libraries are not provided in this repository but should be installed on the system or placed under `thirdparty/include` (headers) and `thirdparty/lib` (shared libraries).

## Building

### Standalone Program

CMake is used to build a standalone decoder program.

Assuming that the needed libraries have already been installed on the system or are placed under `thirdparty`, the following commands can be ran to compile the program.

```sh
mkdir build
cmake -B build
cmake --build build
```

### GDExtension (Incomplete)

**NOTE:** The GDExtension portion of this project currently does not work. For now, these instructions are provided only for reference.

SCons is used to build the main GDExtension using godot-cpp.

First, make sure that the `godot-cpp` submodule has been initialized.

If it hasn't already been initialized, then run:

```sh
git submodule update --init
```

Then, run `scons` to compile the extension. Once compiling has finished, the extension should appear under the `project` folder.

Open the provided Godot project and verify that the GDExtension loads and functions properly before using it in other projects.
