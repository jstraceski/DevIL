//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2001 by Denton Woods
// Last modified: 05/25/2001 <--Y2K Compliant! =]
//
// Filename: openilu/noise.c
//
// Description: Noise generation functions
//
//-----------------------------------------------------------------------------


#include "ilu_internal.h"
#include <math.h>
//#include <time.h>
#include <limits.h>
#include "ilu_alloc.h"


// Very simple right now.
//	This will probably use Perlin noise and parameters in the future.
ILboolean ILAPIENTRY iluNoisify(ILclampf Tolerance)
{
	ILuint		i, c, Factor, Factor2, NumPix;
	ILint		Val;
	ILushort	*ShortPtr;
	ILuint		*IntPtr;

	iCurImage = ilGetCurImage();
	if (iCurImage == NULL) {
		ilSetError(ILU_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	// @TODO:  Change this to work correctly without time()!
	//srand(time(NULL));
	NumPix = iCurImage->SizeOfData / iCurImage->Bpc;

	switch (iCurImage->Bpc)
	{
		case 1:
			Factor = (ILubyte)(Tolerance * (UCHAR_MAX / 2));
			if (Factor == 0)
				return IL_TRUE;
			Factor2 = Factor + Factor;
			for (i = 0; i < NumPix; i += iCurImage->Bpp) {
				Val = (ILint)((ILint)(rand() % Factor2) - Factor);
				for (c = 0; c < iCurImage->Bpp; c++) {
					if ((ILint)iCurImage->Data[i + c] + Val > UCHAR_MAX)
						iCurImage->Data[i + c] = UCHAR_MAX;
					else if ((ILint)iCurImage->Data[i + c] + Val < 0)
						iCurImage->Data[i + c] = 0;
					else
						iCurImage->Data[i + c] += Val;
				}
			}
			break;
		case 2:
			Factor = (ILushort)(Tolerance * (USHRT_MAX / 2));
			if (Factor == 0)
				return IL_TRUE;
			Factor2 = Factor + Factor;
			ShortPtr = (ILushort*)iCurImage->Data;
			for (i = 0; i < NumPix; i += iCurImage->Bpp) {
				Val = (ILint)((ILint)(rand() % Factor2) - Factor);
				for (c = 0; c < iCurImage->Bpp; c++) {
					if ((ILint)ShortPtr[i + c] + Val > USHRT_MAX)
						ShortPtr[i + c] = USHRT_MAX;
					else if ((ILint)ShortPtr[i + c] + Val < 0)
						ShortPtr[i + c] = 0;
					else
						ShortPtr[i + c] += Val;
				}
			}
			break;
		case 4:
			Factor = (ILuint)(Tolerance * (UINT_MAX / 2));
			if (Factor == 0)
				return IL_TRUE;
			Factor2 = Factor + Factor;
			IntPtr = (ILuint*)iCurImage->Data;
			for (i = 0; i < NumPix; i += iCurImage->Bpp) {
				Val = (ILint)((ILint)(rand() % Factor2) - Factor);
				for (c = 0; c < iCurImage->Bpp; c++) {
					if (IntPtr[i + c] + Val > UINT_MAX)
						IntPtr[i + c] = UINT_MAX;
					else if ((ILint)IntPtr[i + c] + Val < 0)
						IntPtr[i + c] = 0;
					else
						IntPtr[i + c] += Val;
				}
			}
			break;
	}

	return IL_TRUE;
}




// Information on Perlin Noise taken from
//	http://freespace.virgin.net/hugo.elias/models/m_perlin.htm


/*ILdouble Noise(ILint x, ILint y)
{
    static ILint n;
	n = x + y * 57;
    n = (n<<13) ^ n;
    return (1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}


ILdouble SmoothNoise(ILint x, ILint y)
{
	ILdouble corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
	ILdouble sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8;
	ILdouble center  =  Noise(x, y) / 4;
    return corners + sides + center;
}


ILdouble Interpolate(ILdouble a, ILdouble b, ILdouble x)
{
	ILdouble ft = x * 3.1415927;
	ILdouble f = (1 - cos(ft)) * .5;

	return  a*(1-f) + b*f;
}


ILdouble InterpolatedNoise(ILdouble x, ILdouble y)
{
	ILint		integer_X, integer_Y;
	ILdouble	fractional_X, fractional_Y, v1, v2, v3, v4, i1, i2;

	integer_X    = (ILint)x;
	fractional_X = x - integer_X;

	integer_Y    = (ILint)y;
	fractional_Y = y - integer_Y;

	v1 = SmoothNoise(integer_X,     integer_Y);
	v2 = SmoothNoise(integer_X + 1, integer_Y);
	v3 = SmoothNoise(integer_X,     integer_Y + 1);
	v4 = SmoothNoise(integer_X + 1, integer_Y + 1);

	i1 = Interpolate(v1, v2, fractional_X);
	i2 = Interpolate(v3, v4, fractional_X);

	return Interpolate(i1, i2, fractional_Y);
}



ILdouble PerlinNoise(ILdouble x, ILdouble y)
{
	ILuint i, n;
	ILdouble total = 0, p, frequency, amplitude;
	//p = persistence;
	//n = Number_Of_Octaves - 1;
	n = 2;
	//p = .5;

	p = (ILdouble)(rand() % 1000) / 1000.0;


	for (i = 0; i < n; i++) {
		frequency = pow(2, i);
		amplitude = pow(p, i);

		total = total + InterpolatedNoise(x * frequency, y * frequency) * amplitude;
	}

	return total;
}



ILboolean ILAPIENTRY iluNoisify()
{
	ILuint x, y, c;
	ILint Val;

	iCurImage = ilGetCurImage();
	if (iCurImage == NULL) {
		ilSetError(ILU_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	for (y = 0; y < iCurImage->Height; y++) {
		for (x = 0; x < iCurImage->Width; x++) {
			Val = (ILint)(PerlinNoise(x, y) * 50.0);

			for (c = 0; c < iCurImage->Bpp; c++) {
				if ((ILint)iCurImage->Data[y * iCurImage->Bps + x * iCurImage->Bpp + c] + Val > 255)
					iCurImage->Data[y * iCurImage->Bps + x * iCurImage->Bpp + c] = 255;
				else if ((ILint)iCurImage->Data[y * iCurImage->Bps + x * iCurImage->Bpp + c] + Val < 0)
					iCurImage->Data[y * iCurImage->Bps + x * iCurImage->Bpp + c] = 0;
				else
					iCurImage->Data[y * iCurImage->Bps + x * iCurImage->Bpp + c] += Val;
			}
		}
	}

	return IL_TRUE;
}*/