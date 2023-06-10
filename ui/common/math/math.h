#ifndef _UI_MATH_H_
#define _UI_MATH_H_

namespace UI
{

// webkit  Source\WebCore\platform\graphics\color.cpp
// inline uint16_t fastDivideBy255(uint16_t value)
// {
//     // This is an approximate algorithm for division by 255, but it gives accurate results for 16bit values.
//     uint16_t approximation = value >> 8;
//     uint16_t remainder = value - (approximation * 255) + 1;
//     return approximation + (remainder >> 8);
// }

	// 非常小的数
#define EPSILON_E4  (float)(1E-4)
#define EPSILON_E5  (float)(1E-5)
#define EPSILON_E6  (float)(1E-6)

// 定点数宏
// pi defines
#define PI         ((float)3.141592654f)
#define PI2        ((float)6.283185307f)
#define PI_DIV_2   ((float)1.570796327f)
#define PI_DIV_4   ((float)0.785398163f) 
#define PI_INV     ((float)0.318309886f) 

#ifndef M_PI
const double piDouble = 3.14159265358979323846;
const float piFloat = 3.14159265358979323846f;
#else
const double piDouble = M_PI;
const float piFloat = static_cast<float>(M_PI);
#endif

// fixed point mathematics constants
#define FIXP16_SHIFT     16
#define FIXP16_MAG       65536
#define FIXP16_DP_MASK   0x0000ffff
#define FIXP16_WP_MASK   0xffff0000
#define FIXP16_ROUND_UP  0x00008000

#define FIXP20_SHIFT     20  // 22则代表了纹理的大小不能超过512，否则会溢出，这不行吧。至少得2048。因此修改为20
#define FIXP28_SHIFT     28
#define FIXP8_SHIFT      8  // (28 - 20)

// 从16.16格式的定点数中提取整数部分和小数部分
#define FIXP16_WP(fp) ((fp) >> FIXP16_SHIFT)
#define FIXP16_DP(fp) ((fp) && FIXP16_DP_MASK)

// 将整数和浮点数转换为16.16格式的数
#define INT_TO_FIXP16(i) ((i) <<  FIXP16_SHIFT)
#define FLOAT_TO_FIXP16(fv) (((float)(fv) * (float)FIXP16_MAG))

// 将定点数转换为浮点数
#define FIXP16_TO_FLOAT(fp) ( ((float)fp)/FIXP16_MAG)

// 通用宏
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (b) : (a))

#define SWAP(a, b, t) { t = a; a = b; b = t; }

#define DEG_2_RAD(ang) ((ang)*PI/180.0)
#define RAD_2_DEG(rads) (rads)*180.0/PI)

inline double deg2rad(double d)  { return d * piDouble / 180.0; }
inline double rad2deg(double r)  { return r * 180.0 / piDouble; }
inline float  deg2rad(float d)   { return d * piFloat / 180.0f; }
inline float  rad2deg(float r)   { return r * 180.0f / piFloat; }

#define RAND_RANGE(x, y) ((x) + (rand()%((y)-(x)+1)))

// storage for our lookup tables
extern float cos_look[361]; // 1 extra so we can store 0-360 inclusive
extern float sin_look[361]; // 1 extra so we can store 0-360 inclusive

void  Build_Sin_Cos_Tables(void);
void  Init_Sin_Cos_Tables();

// trig functions
float Fast_Sin(float theta);
float Fast_Cos(float theta);

// math functions
int   Fast_Distance_2D(int x, int y);
float Fast_Distance_3D(float x, float y, float z);

bool  fequ(float f1, float f2);
bool  IsEmptyDegree(float degree);

}

#endif // _UI_MATH_H_