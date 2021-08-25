export void AddByElement(uniform const float a[], uniform const float b[], uniform float result[], uniform const size_t count)
{
    foreach(index = 0 ... count)
    {
        result[index] = a[index] + b[index];
    }

    return;
}

export void SubByElement(uniform const float a[], uniform const float b[], uniform float result[], uniform const size_t count)
{
    foreach(index = 0 ... count)
    {
        result[index] = a[index] - b[index];
    }

    return;
}

export void MulByElement(uniform const float a[], uniform const float b[], uniform float result[], uniform const size_t count)
{
	foreach (index = 0 ... count) {
		result[index] = a[index] * b[index];
	}
}

export void MulByElementi32(uniform const int32 a[], uniform const int32 b[], uniform int32 result[], uniform const size_t count)
{
	foreach (index = 0 ... count) {
		result[index] = a[index] * b[index];
	}
}

export void MulByElementi16(uniform const int16 a[], uniform const int16 b[], uniform int16 result[], uniform const size_t count)
{
	foreach (index = 0 ... count) {
		result[index] = a[index] * b[index];
	}
}

export void DivByElement(uniform const float a[], uniform const float b[], uniform float result[], uniform const size_t count)
{
	foreach (index = 0 ... count) {
		result[index] = a[index] / b[index];
	}
}

export void DivByElementi32(uniform const int32 a[], uniform const int32 b[], uniform int32 result[], uniform const size_t count)
{
	foreach (index = 0 ... count) {
		result[index] = a[index] / b[index];
	}
}

export void DivByElementi16(uniform const int16 a[], uniform const int16 b[], uniform int16 result[], uniform const size_t count)
{
	foreach (index = 0 ... count) {
		result[index] = a[index] / b[index];
	}
}

export void Normalize(uniform const size_t count, uniform float v[], uniform float length)
{
    if (!length) return;
    length = 1.0f / length;
    foreach (index = 0 ... count) {
        v[index] *= length;
    }
}

export void Absolute(uniform float result[], uniform const float a[], uniform const size_t count)
{
    foreach (index = 0 ... count)
    {
        result[index] = abs(a[index]);
    }
}

export void CrossProduct2(uniform const float a[2], uniform const float b[2], uniform float result[2]) {
	foreach(index = 0 ... 3) {
		int index_a = ((index + 1 == 3) ? 0 : index + 1);
		int index_b = ((index == 0) ? 2 : index - 1);
		result[index] = a[index_a] * b[index_b]
				-a[index_b] * b[index_a];
	}
}

export void CrossProduct3(uniform const float a[3], uniform const float b[3], uniform float result[3]) {
	foreach(index = 0 ... 3) {
		int index_a = ((index + 1 == 3) ? 0 : index + 1);
		int index_b = ((index == 0) ? 2 : index - 1);
		result[index] = a[index_a] * b[index_b]
				-a[index_b] * b[index_a];
	}
}