#pragma once


struct StoragePerson
{
	void Calculate(float Height)
	{
		this->Height =             Height;
		this->Step   =   0.25f  *  Height;
		this->Eyes   =   1.1f   *  Height / 2;
		this->Mass   = 100.0f   * (Height - 1);
		this->Radius =             Height / 3;
		this->Step   =   0.15f *   Height;
	}

	float Mass;
	float Radius, Height;
	float Eyes; // distance from center to eyes
	float Step; // maximum height difference for a step
};
