#pragma once
#include <vector>
#include <string>
namespace osomi{
	class Texture {
		friend class Shader;
		friend class Resources;
	private:
		std::string name;
		unsigned int id;
	public:
		Texture(std::string _name, unsigned int _id): name(_name), id(_id) {}
		std::string getName() { return name; }
		void bind(unsigned int targ);
		void unbind(unsigned int targ);
	protected:
		unsigned int getId() { return id; }
		~Texture();
	};
}