#include "IllusionEngine.h"
void test() {
	std::cout << "test" << std::endl;
}

int main(int argc, char* argv[]) {
	char* s = "hell";
	int test;
	ILSENGINE_INTERNAL_INFO(s, "o");
	illusion::window::Create(1280,720,"IllusionEngine");
	while (!illusion::window::ShouldClose())
	{
		illusion::window::Update();
	}
	illusion::window::Destroy();
}