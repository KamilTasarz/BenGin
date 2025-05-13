#pragma once

#include "Virus.h"

class BasicVirus : public Virus
{
public:
    //GravityVirus() = default;
    //virtual ~GravityVirus() = default;

    void onAttach(Node* owner) override;
    void onDetach() override;

    //void VirusEffect(Node* target) override;
};

