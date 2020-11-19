#include "IllusionEngine.h"

int main(int argc, char* argv[]) {
	illusion::window::Create(1280, 720, "IllusionEngine");

	while (!illusion::window::ShouldClose())
	{
		illusion::window::Update();
	}

	illusion::window::Destroy();
}