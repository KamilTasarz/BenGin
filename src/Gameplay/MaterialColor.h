#pragma once

#include "Script.h"

class MaterialColor final : public Script
{
public:
	using SelfType = MaterialColor;

	VARIABLE(float, red);
	VARIABLE(float, green);
	VARIABLE(float, blue);
	VARIABLE(float, alpha);

	MaterialColor() = default;
	~MaterialColor() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;

	std::vector<Variable*> getFields() const override {
		static Variable redVar = getField_red();
		static Variable greenVar = getField_green();
		static Variable blueVar = getField_blue();
		static Variable alphaVar = getField_alpha();
		return { &redVar, &greenVar, &blueVar, &alphaVar };
	}
};