#include "JSerializer.h"
#include <iostream>

void JSerializable::AddValidation(std::function<void()> validationFunction)
{
	Validation.push_back(validationFunction);
}
