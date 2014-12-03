float makePositive(float x)
{
	return x * 0.5 + 0.5;
}

float psin(float x)
{
	return makePositive(sin(x));
}

float pcos(float x)
{
	return makePositive(cos(x));
}

// TODO: dalsi funkce
