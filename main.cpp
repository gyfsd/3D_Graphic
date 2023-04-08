#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include "gamelib.hpp"
#include "grlib.cpp"
namespace points_{
	struct point_{
		float4 pos;
		uint32_t *edges;
		uint32_t edgesC;
		uint32_t color = 0xffffffff;
		uint32_t edgecolor = 0xffffffff;
	};
	struct points{
		point_ *points;
		uint32_t pointsC;
	};
}
namespace camera_{
	struct cam{
	float4 pos;
	float3 rotation;
	float2 size;
	float speed;
	};
}
using namespace camera_;
using namespace points_;
using namespace GMatrix;
class render_{
	private:
	class draw_ d;
	public:
	float4 point_rotate(float4 point,cam camd){
		float4 pointc;
		pointc.x = point.x - camd.pos.x;
		pointc.y = point.y - camd.pos.y;
		pointc.z = point.z - camd.pos.z;
		pointc.w = point.w - camd.pos.w;
		/*
		printf("rotate input\n");
		printf("x:%f\n",pointc.x);
		printf("y:%f\n",pointc.y);
		printf("z:%f\n",pointc.z);
		printf("w:%f\n",pointc.w);
		*/
		pointc = rotate_y(pointc,camd.rotation.y/180*PI);
		pointc = rotate_z(pointc,camd.rotation.z/180*PI);
		pointc = rotate_x(pointc,camd.rotation.x/180*PI);
		
		pointc.x = pointc.x + camd.pos.x;
		pointc.y = pointc.y + camd.pos.y;
		pointc.z = pointc.z + camd.pos.z;
		pointc.w = pointc.w + camd.pos.w;
		/*
		printf("after rotate\n");
		printf("x:%f\n",pointc.x);
		printf("y:%f\n",pointc.y);
		printf("z:%f\n",pointc.z);
		printf("w:%f\n",pointc.w);
		*/
		return pointc;
	}
	float4 point_scale(float4 point,cam camd){
		float4 vectp;
		float4 res;
		vectp.x = point.x-camd.pos.x;
		vectp.y = point.y-camd.pos.y;
		vectp.z = point.z-camd.pos.z;
		
		res.x = (vectp.x / vectp.z * camd.size.x);
		res.y = (vectp.y / vectp.z * camd.size.y);
		
		res.z = point.z;
		res.w = point.w;
		return res;
	}
	float3 point_to_2d(float4 point,cam camd){
		float4 pointb = point_rotate(point,camd);
		pointb = point_scale(pointb,camd);
		float3 pointa;
		pointa.x = pointb.x;
		pointa.y = pointb.y;
		pointa.z = pointb.z;
		return pointa;
	}
	float2 td_to_scr(float3 point,float2 cam_size,int scr_size_x,int scr_size_y){
		float2 pixp;
		pixp.x = (point.x  * scr_size_x)+(scr_size_x/2);
		pixp.y = (point.y  * scr_size_y)+(scr_size_y/2);
		return pixp;
	}
	void draw_point(point_ point,cam camd,SDL_Surface *scr){
		float3 pointc = point_to_2d(point.pos,camd);
		float2 pointd = td_to_scr(pointc,camd.size,scr->w,scr->h);
		if(pointd.x > -1 & pointd.x < scr->w & pointc.y > -1 & pointc.y < scr->h  & pointc.z-camd.pos.z >= 0){
		d.circle((int)pointd.x,(int)pointd.y,3,point.color,scr);}
	}
	void draw_edge(point_ point,point_ topoint,cam camd,SDL_Surface *scr){
		float3 pointc = point_to_2d(point.pos,camd);
		float2 pointd = td_to_scr(pointc,camd.size,scr->w,scr->h);
		float3 tpointc = point_to_2d(topoint.pos,camd);
		float2 tpointd = td_to_scr(tpointc,camd.size,scr->w,scr->h);
		
		if(/*pointd.x > -1 & pointd.x < scr->w & pointc.y > -1 & pointc.y < scr->h  & */pointc.z-camd.pos.z >= 0){
			if(/*tpointd.x > -1 & tpointd.x < scr->w & tpointc.y > -1 & tpointc.y < scr->h  &*/ tpointc.z-camd.pos.z >= 0){
				d.line((int)pointd.x,(int)pointd.y,(int)tpointd.x,(int)tpointd.y,point.edgecolor,scr);
		
		}
	}
}
	void render(points PS,cam camd,SDL_Surface *scr){
		camd.rotation.x = fabs(fmod(camd.rotation.x,360));
		camd.rotation.y = fabs(fmod(camd.rotation.y,360));
		camd.rotation.z = fabs(fmod(camd.rotation.z,360));
		uint32_t Psel = 0;
		int Esel;
		while(Psel < PS.pointsC){
			draw_point(PS.points[Psel],camd,scr);
			Esel = 0;
			while(Esel < PS.points[Psel].edgesC){
			draw_edge(PS.points[Psel],PS.points[PS.points[Psel].edges[Esel]],camd,scr);
			Esel++;
			}
			Psel++;
		}
	}
};
class obj_file{
	private:
	
	public:
	uint32_t find(char *str,uint32_t len,char *c,uint32_t cC,uint32_t num){
		uint32_t cur = 0;
		uint32_t n = 0;
		uint32_t sc;
		while(cur < cC){
			if(str[cur] == c[0]){
				sc = 1;
				while(c[sc] == str[sc+cur] & sc < cC){
					sc++;
				}
				if(sc == cC){if(n == num){goto skip;}n++;}
			}
			cur++;
		}
		return -1;
		skip:
		return cur;
	}
	
};
class main_{
	private:
	class render_ render;
	class sdl_ sdl;
	class draw_ d;
	public:
	points create_random(){
		points cube;
		point_ *p = (points_::point_*)malloc(sizeof(struct point_)*9999);
		uint32_t *edges = (uint32_t *)malloc(9999*4);
		srand(754);
		int pC = 0;
		while(pC < 9999){
			p[pC].pos.x = fmod(rand(),99);
			p[pC].pos.y = fmod(rand(),99);
			p[pC].pos.z = fmod(rand(),99);
			p[pC].edgesC = 1;
			p[pC].edges = &*edges+pC;
			p[pC].edges[0] = rand() % 9999;
			
			pC++;
		}
		cube.points = p;
		cube.pointsC = 9999;
		return cube;
	}
	points create_sphere(float x,float y,float z,float r,point_ *p,uint32_t *edges){
		points sphere;
		/*point_ *p = (points_::point_*)malloc(sizeof(struct point_)*36*360);
		uint32_t *edges = (uint32_t *)malloc(36*360*4*4);*/
		float4 rot;
		rot.x = 0;
		rot.y = 0;
		rot.z = 1;
		rot.w = 0;
		float3 rt;
		float4 arot;
		float fx = 0;
		float fy = 0;
		long pC = 0;
		fy = 0;
		while(fy < 360){
			fx = 0;
			while(fx < 360){
				rt.x = fy;
				rt.y = fx;
				rt.z = 0;
				arot = rotate_matrix_in_grad(rot,rt);
				p[pC].color = 0xffffffff;
				p[pC].edgecolor = 0xff;
				p[pC].pos.x = (arot.x*r)+x;
				p[pC].pos.y = (arot.y*r)+y;
				p[pC].pos.z = (arot.z*r)+z;
				p[pC].edgesC = 4;
				p[pC].edges = &*edges+(pC*4);
				p[pC].edges[0] = pC-36;
				p[pC].edges[1] = pC+1;
				p[pC].edges[2] = pC-1;
				p[pC].edges[3] = pC+36;
				pC++;
				fx+=10;
			}
			fy+=10;
		}
		sphere.points = p;
		sphere.pointsC = 36*36;
		return sphere;
	}
	int main(){
		class event_ event;
		sdl.init_screen();
		cam c;
		c.pos.x = 0;
		c.pos.y = 0;
		c.pos.z = 0;
		c.rotation.x = 0;
		c.rotation.y = 0;
		c.rotation.z = 0;
		c.speed = 10;
		c.size.x = 1;
		c.size.y = 1;
		point_ *p = (points_::point_*)malloc(sizeof(struct point_)*36*36);
		uint32_t *edges = (uint32_t *)malloc(36*360*4*4);
		struct event_::keyboarddata keyd;
		float spheres = 0;
		while(1){
		points cube = create_sphere(0,0,0,100,p,edges);
		spheres++;
		if(spheres > 20){
			spheres = 10;
		}
		render.render(cube,c,sdl.scr);
		sdl.flip();
		d.fill(0,sdl.scr);
		keyd = event.getkey();
		event.poll();
			
			if(keyd.type == SDL_KEYDOWN){
				if(keyd.code == SDLK_w){
					float4 r;
					float3 g = c.rotation;
					g.y  = g.y*-1;
					g.x = g.x*-1;
					r.x = 0;
					r.y = 0;
					r.z = 1*c.speed;
					r.w = 0;
					r = rotate_matrix_in_grad(r,g);
					c.pos.x += r.x;
					c.pos.y += r.y;
					c.pos.z += r.z;
					c.pos.w += r.w;
				}
				if(keyd.code == SDLK_s){
					float4 r;
					float3 g = c.rotation;
					g.y  = g.y*-1;
					g.x = g.x*-1;
					r.x = 0;
					r.y = 0;
					r.z = 1*c.speed;
					r.w = 0;
					r = rotate_matrix_in_grad(r,g);
					c.pos.x -= r.x;
					c.pos.y -= r.y;
					c.pos.z -= r.z;
					c.pos.w -= r.w;
				}
				if(keyd.code == SDLK_t){
					
				}
				if(keyd.code == SDLK_r){
					
				}
				if(keyd.code == SDLK_RIGHT){
					c.rotation.y -= 2;
					if(c.rotation.y < 0){c.rotation.y = 360;}

				}
				if(keyd.code == SDLK_LEFT){
					c.rotation.y += 2;
					if(c.rotation.y > 360){c.rotation.y = 0;}
				}
				if(keyd.code == SDLK_UP){
					c.rotation.x -= 2;
					if(c.rotation.x < 0){c.rotation.x = 360;}
				}
				if(keyd.code == SDLK_DOWN){
					c.rotation.x += 2;
					if(c.rotation.x > 360){c.rotation.x = 0;}
				}
			
				
			}
			//if(keyd.type == SDL_KEYDOWN){}
		}
		while(1){}
		return 0;
	}
};
int main(){
	class main_ main;
	return main.main();
}

