#include "IllusionEngine.h"
void test() {
	std::cout << "test" << std::endl;
}

int main(int argc, char* argv[]) {
	char* s = "hell";
	test();
	ILSENGINE_INFO(s, "o");
	glm::vec3 v(0, 0, 0);
	ILSENGINE_INFO("vector : ",glm::to_string(v));
	illusion::log("haha");
	illusion::window::Create(1280,720,"IllusionEngine");
	while (!illusion::window::ShouldClose())
	{
		illusion::window::Update();
	}
	illusion::window::Destroy();
}