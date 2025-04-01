#ifdef __cplusplus
enum class texture_format 
#else
enum stylizer_api_texture_format
#endif
{
	Undefined,

	Ru8_Normalized,
	Ri8_Normalized,
	Ru8,
	Ri8,
	Ru16,
	Ri16,
	Rf16,
	RGu8_Normalized,
	RGi8_Normalized,
	RGu8,
	RGi8,
	Rf32,
	Ru32,
	Ri32,
	RGu16,
	RGi16,
	RGf16,
	RGBAu8_Normalized,
	RGBAu8_NormalizedSRGB,
	RGBAi8_Normalized,
	RGBAu8,
	RGBAi8,
	BGRAu8_Normalized,
	BGRAu8_NormalizedSRGB,
	RGBu10Au2,
	RGBu10Au2_Normalized,
	RGf11Bf10,
	// RGB9E5Ufloat, // ?
	RGf32,
	RGu32,
	RGi32,
	RGBAu16,
	RGBAi16,
	RGBAf16,
	RGBAf32,
	RGBAu32,
	RGBAi32,

	Stencil_u8,
	Depth_u16,
	Depth_u24,
	Depth_u24Stencil_u8,
	Depth_f32,
	Depth_f32Stencil_u8,

	BC1RGBA_Normalized,
	BC1RGBA_NormalizedSRGB,
	BC2RGBA_Normalized,
	BC2RGBA_NormalizedSRGB,
	BC3RGBA_Normalized,
	BC3RGBA_NormalizedSRGB,

	// BC4RUnorm,
	// BC4RSnorm,
	// BC5RGUnorm,
	// BC5RGSnorm,
	// BC6HRGBUfloat,
	// BC6HRGBFloat,

	BC7RGBA_Normalized,
	BC7RGBA_NormalizedSRGB,

	ETC2RGB8_Normalized,
	ETC2RGB8_NormalizedSRGB,
	ETC2RGB8A1_Normalized,
	ETC2RGB8A1_NormalizedSRGB,
	ETC2RGBA8_Normalized,
	ETC2RGBA8_NormalizedSRGB,

	EACRu11_Normalized,
	EACRi11_Normalized,
	EACRGu11_Normalized,
	EACRGi11_Normalized,

	ASTC4x4_Normalized,
	ASTC4x4_NormalizedSRGB,
	ASTC5x4_Normalized,
	ASTC5x4_NormalizedSRGB,
	ASTC5x5_Normalized,
	ASTC5x5_NormalizedSRGB,
	ASTC6x5_Normalized,
	ASTC6x5_NormalizedSRGB,
	ASTC6x6_Normalized,
	ASTC6x6_NormalizedSRGB,
	ASTC8x5_Normalized,
	ASTC8x5_NormalizedSRGB,
	ASTC8x6_Normalized,
	ASTC8x6_NormalizedSRGB,
	ASTC8x8_Normalized,
	ASTC8x8_NormalizedSRGB,
	ASTC10x5_Normalized,
	ASTC10x5_NormalizedSRGB,
	ASTC10x6_Normalized,
	ASTC10x6_NormalizedSRGB,
	ASTC10x8_Normalized,
	ASTC10x8_NormalizedSRGB,
	ASTC10x10_Normalized,
	ASTC10x10_NormalizedSRGB,
	ASTC12x10_Normalized,
	ASTC12x10_NormalizedSRGB,
	ASTC12x12_Normalized,
	ASTC12x12_NormalizedSRGB,

	Ru16_Normalized,
	RGu16_Normalized,
	RGBAu16_Normalized,

	Ri16_Normalized,
	RGi16_Normalized,
	RGBAi16_Normalized,

	R8BG8Biplanar420_Normalized,
	R10X6BG10X6Biplanar420_Normalized,
	R8BG8A8Triplanar420_Normalized,
	R8BG8Biplanar422_Normalized,
	R8BG8Biplanar444_Normalized,
	R10X6BG10X6Biplanar422_Normalized,
	R10X6BG10X6Biplanar444_Normalized,

	// Some convenience overloads
	RGBA8srgb = RGBAu8_NormalizedSRGB,
	RGBA32 = RGBAf32,
	Depth24 = Depth_u24,

	Gray32 = Rf32,
	Gray16 = Rf16,
	Gray8 = Ru8_Normalized,
};