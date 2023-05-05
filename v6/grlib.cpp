#define PI 3.14
struct float2{
	uint8_t type = 0;
	float x;
	float y;
};
struct float3{
	uint8_t type = 1;
	float x;
	float y;
	float z;
};
struct float4{
	uint8_t type = 2;
	float x;
	float y;
	float z;
	float w;
};
namespace vector_op
{
	float scalar_product_float4(float4 v1,float4 v2){
		return (v1.x*v2.x)+(v1.y*v2.y)+(v1.z*v2.z);
	}
	float scalar_product_float2(float2 v1,float2 v2){
		return (v1.x*v2.x)+(v1.y*v2.y);
	}
};
namespace float_op
{	
	struct ftype{
		uint8_t type;
	};
	uint8_t type(void *v0){
		ftype t0 = ((ftype *)v0)[0];
		if(t0.type == 0){
			return 0;
		}
		if(t0.type == 1){
			return 1;
		}
		if(t0.type == 2){
			return 2;
		}
		return -1;
	}
	void *add(void *v0,void *v1){
		uint8_t t0 = type(v0);
		uint8_t t1 = type(v1);
		if(t0 != t1){
			return 0;
		}
		if(t0 == 0){
			float2 a0 = ((float2 *)(v0))[0];
			float2 a1 = ((float2 *)(v1))[0];
			float2 o;
			o.x = a0.x + a1.x;
			o.y = a0.y + a1.y;
			return &o;
		}
		if(t0 == 1){
			float3 a0 = ((float3 *)(v0))[0];
			float3 a1 = ((float3 *)(v1))[0];
			float3 o;
			o.x = a0.x + a1.x;
			o.y = a0.y + a1.y;
			o.z = a0.z + a1.z;
			return &o;
		}
		if(t0 == 2){
			float4 a0 = ((float4 *)(v0))[0];
			float4 a1 = ((float4 *)(v1))[0];
			float4 o;
			o.x = a0.x + a1.x;
			o.y = a0.y + a1.y;
			o.z = a0.z + a1.z;
			o.w = a0.w + a1.w;
			return &o;
		}
	}
	void *sub(void *v0,void *v1){
		uint8_t t0 = type(v0);
		uint8_t t1 = type(v1);
		if(t0 != t1){
			return 0;
		}
		if(t0 == 0){
			float2 a0 = ((float2 *)(v0))[0];
			float2 a1 = ((float2 *)(v1))[0];
			float2 o;
			o.x = a0.x - a1.x;
			o.y = a0.y - a1.y;
			return &o;
		}
		if(t0 == 1){
			float3 a0 = ((float3 *)(v0))[0];
			float3 a1 = ((float3 *)(v1))[0];
			float3 o;
			o.x = a0.x - a1.x;
			o.y = a0.y - a1.y;
			o.z = a0.z - a1.z;
			return &o;
		}
		if(t0 == 2){
			float4 a0 = ((float4 *)(v0))[0];
			float4 a1 = ((float4 *)(v1))[0];
			float4 o;
			o.x = a0.x - a1.x;
			o.y = a0.y - a1.y;
			o.z = a0.z - a1.z;
			o.w = a0.w - a1.w;
			return &o;
		}
	}
};
namespace GMatrix
{
	//The final float4 returned by matrices...
	float4 scale_matrix(float4 in,float4 scale){
		/*
		 * ix 0 0 0
		 * 0 iy 0 0
		 * 0 0 iz 0
		 * 0 0 0  1
		 */
		 float4 res;
		 res.x = in.x * scale.x;
		 res.y = in.y * scale.y;
		 res.z = in.z * scale.z;
		 res.w = in.w;
		 return res;
	}
	float4 rotate_x(float4 in,float ox){
		float4 res;
		res.x = in.x;
		res.y = in.y*cos(ox) - in.z*sin(ox);
		res.z = in.y*sin(ox) + in.z*cos(ox);
		res.w = in.w;
		return res;
	}
	float4 rotate_y(float4 in,float oy){
		float4 res;
		res.x = in.x*cos(oy) + in.z*sin(oy);
		res.y = in.y;
		res.z = in.x*(-sin(oy)) + in.z*cos(oy);
		res.w = in.w;
		return res;
	}
	float4 rotate_z(float4 in,float oz){
		float4 res;
		res.x = in.x*cos(oz) - in.y*sin(oz);
		res.y = in.x*sin(oz) + in.y*cos(oz);
		res.z = in.z;
		res.w = in.w;
		return res;
	}
	float4 rotate_matrix_in_rad(float4 in,float3 rot){
		/*         ox
		 * 
		 * 1 0          0         0
		 * 0 cos(ox)    -sin(ox)  0
		 * 0 sin(ox)    cos(ox)   0
		 * 0 0          0         1
		 * 
		 *         oy
		 * 
		 * cos(oy)  0 sin(oy) 0
		 * 0        1 0       0
		 * -sin(oy) 0 cos(oy) 0
		 * 0        0 0       1
		 * 
		 * 		   oz
		 * 
		 * cos(oz) -sin(oz) 0 0
		 * sin(oz) cos(oz)  0 0
		 * 0       0        1 0
		 * 0       0        0 1
		 * */
		 float4 res;
		 res = rotate_x(in,rot.x);
		 res = rotate_y(res,rot.y);
		 res = rotate_z(res,rot.z);
		 return res;
	}
	float4 rotate_matrix_in_grad(float4 in,float3 rot){
		/*         ox
		 * 
		 * 1 0          0         0
		 * 0 cos(ox)    -sin(ox)  0
		 * 0 sin(ox)    cos(ox)   0
		 * 0 0          0         1
		 * 
		 *         oy
		 * 
		 * cos(oy)  0 sin(oy) 0
		 * 0        1 0       0
		 * -sin(oy) 0 cos(oy) 0
		 * 0        0 0       1
		 * 
		 * 		   oz
		 * 
		 * cos(oz) -sin(oz) 0 0
		 * sin(oz) cos(oz)  0 0
		 * 0       0        1 0
		 * 0       0        0 1
		 * */
		 float4 res;
		 res = rotate_x(in,rot.x/180*PI);
		 res = rotate_y(res,rot.y/180*PI);
		 res = rotate_z(res,rot.z/180*PI);
		 return res;
	}
	float4 move_matrix(float4 in,float4 move){
		/*
		* 1 0 0 a
		* 0 1 0 b
		* 0 0 1 c
		* 0 0 0 1
		*/
		float4 res;
		res.x = in.x + (in.w*move.x);
		res.y = in.y + (in.w*move.y);
		res.z = in.z + (in.w*move.z);
		res.w = in.w;
		return res;
	}
};

