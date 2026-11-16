#ifndef GDEXAMPLE_HPP
#define GDEXAMPLE_HPP

#include <godot_cpp/classes/node.hpp>

using namespace godot;


class GDExample : public Node {
	GDCLASS(GDExample, Node);

	private:
		bool enabled;

	protected:
		static void _bind_methods();

	public:
		GDExample();
		~GDExample();

		bool is_enabled() const;
};

#endif
