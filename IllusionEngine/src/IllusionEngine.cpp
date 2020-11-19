#include "IllusionEngine.h"
void test() {
	std::cout << "test" << std::endl;
}


int main(int argc, char* argv[]) {
	char* s = "hell";
	int test;
	ILSENGINE_INTERNAL_INFO(s, "o");
	IllusionEngine::Window::Create(1280,720,"IllusionEngine");
	while (!IllusionEngine::Window::ShouldClose())
	{
		IllusionEngine::Window::Update();
	}
	IllusionEngine::Window::Destroy();
}