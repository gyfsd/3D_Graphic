#include <Windows.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include "gamelib2.hpp"
#include "grlib.cpp"
namespace memory{
	uint32_t freeoffset = 0;
	uint32_t busyC = 0;
	void *mem;
	//========================================
	uint32_t maxaddrs = 100;
	uint32_t memoryC = 0;
	//========================================
	struct busy_address{
		void *pointer;
		uint32_t offset;
		uint32_t bytesC;
	};
	busy_address *busy;
	//========================================
	void init_mem(uint32_t C){
		busy = (struct busy_address *)malloc(sizeof(struct busy_address)*maxaddrs);
		mem = malloc(C);
		memoryC = C;
	}
	void create_part(uint32_t bytesC){
		if(freeoffset+bytesC < memoryC){
			busy[busyC].offset = freeoffset;
			busy[busyC].bytesC = bytesC;
			busy[busyC].pointer = &mem+busy[busyC].offset;
			freeoffset += bytesC;
		}
	}
	
};
namespace mesh_{
	struct point_{
		float4 pos;
		uint32_t color = 0xff0000;
	};
	struct edge_{
		uint32_t *points;
		uint32_t pointsC;
		uint32_t color;
	};
	struct mesh{
		edge_ *edges;
		point_ *points;
		uint32_t pointsC = 0;
		uint32_t edgesC = 0;
	};
}
namespace camera_{
	struct cam{
	float4 pos;
	float3 rotation;
	float2 size;
	float speed;
	float rotspeed;
	};
}

using namespace camera_;
using namespace mesh_;
using namespace GMatrix;
namespace render_shader_{
	struct shader{
		void (*function)(mesh m,cam camd,void *mem,uint32_t memC,class zlayer_mask_ zlm,SDL_Surface *scr);
	};
};
class render_shaders_{
	private:
	public:
	render_shader_::shader shaders[1000];
	uint32_t shadersC = 0;
	uint32_t add_shader(render_shader_::shader s){
		shaders[shadersC] = s;
		shadersC++;
	}
	
};
namespace pointop{
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
		class draw_ d;
		float3 pointc = point_to_2d(point.pos,camd);
		float2 pointd = td_to_scr(pointc,camd.size,scr->w,scr->h);
		if(pointd.x > -1 & pointd.x < scr->w & pointc.y > -1 & pointc.y < scr->h  & pointc.z-camd.pos.z >= 0){
		d.circle((int)pointd.x,(int)pointd.y,1,point.color,scr);}
	}
	void draw_line(point_ point,point_ topoint,cam camd,SDL_Surface *scr){
		class draw_ d;
		float3 pointc = point_to_2d(point.pos,camd);
		float2 pointd = td_to_scr(pointc,camd.size,scr->w,scr->h);
		float3 pointa = point_to_2d(topoint.pos,camd);
		float2 pointb = td_to_scr(pointa,camd.size,scr->w,scr->h);
		
		if(pointd.x > -scr->w & pointd.x < 2*scr->w & pointc.y > -scr->h  & pointc.y < scr->h*2 & pointc.z-camd.pos.z >= 0 & pointa.z-camd.pos.z >= 0){
			if(pointb.x > -scr->w  & pointb.x < 2*scr->w & pointb.y > -scr->h & pointb.y < 2*scr->h){
		d.line((int)pointd.x,(int)pointd.y,(int)pointb.x,(int)pointb.y,0xffffffff,scr);
		}
	}
	}
	void draw_line_layer(point_ point,point_ topoint,cam camd,float layer,SDL_Surface *scr,class zlayer_mask_ zlm){
		float3 pointc = point_to_2d(point.pos,camd);
		float2 pointd = td_to_scr(pointc,camd.size,scr->w,scr->h);
		float3 pointa = point_to_2d(topoint.pos,camd);
		float2 pointb = td_to_scr(pointa,camd.size,scr->w,scr->h);
		
		if(pointd.x > -scr->w & pointd.x < 2*scr->w & pointc.y > -scr->h  & pointc.y < scr->h*2 & pointc.z-camd.pos.z >= 0 & pointa.z-camd.pos.z >= 0){
			if(pointb.x > -scr->w  & pointb.x < 2*scr->w & pointb.y > -scr->h & pointb.y < 2*scr->h){
		zlm.line((int)pointd.x,(int)pointd.y,(int)pointb.x,(int)pointb.y,0xff0000,layer,scr);
		}
	}
	}
	float edgezsum(mesh mesh,edge_ edge,cam camd){
		float sum = 0;
		uint32_t p = 0;
		while(p < edge.pointsC){
			sum += mesh.points[edge.points[p]].pos.z;
			p++;
		}
		return sum/p;
	}
};
#include "shaders.cpp"
class render_{
	private:
	class draw_ d;
	public:
	void render(mesh PS,cam camd,void *mem,uint32_t memC,class zlayer_mask_ zlm,class render_shaders_ shaders,uint32_t selshader,SDL_Surface *scr){
		if(selshader < shaders.shadersC){
		shaders.shaders[selshader].function(PS,camd,mem,memC,zlm,scr);
		}
	}
	/*void render(mesh PS,cam camd,void *mem,uint32_t memC,class zlayer_mask_ zlm,SDL_Surface *scr){
		camd.rotation.x = fabs(fmod(camd.rotation.x,360));
		camd.rotation.y = fabs(fmod(camd.rotation.y,360));
		camd.rotation.z = fabs(fmod(camd.rotation.z,360));
		/*uint32_t Psel = 0;
		while(Psel < PS.pointsC){
			draw_point(PS.points[Psel],camd,scr);
			Psel++;
		}
		uint32_t Esel = 0;
		uint32_t Asel = 0;
		int *px = (int*)(mem);
		int *py = (int*)(mem+(memC/2));
		float sum = 0;
		zlm.fill(-923841095);
		zlm.setbit = 0;
		while(Esel < PS.edgesC){
			Asel = 0;
			sum = 0;
			while(Asel < PS.edges[Esel].pointsC){
				if(Asel < PS.edges[Esel].pointsC-1){
				//draw_line(PS.points[PS.edges[Esel].points[Asel]],PS.points[PS.edges[Esel].points[Asel+1]],camd,scr);
				}
				float3 pointc = point_to_2d(PS.points[PS.edges[Esel].points[Asel]].pos,camd);
				float2 pointd = td_to_scr(pointc,camd.size,scr->w,scr->h);
				if(!(pointd.x > -scr->w & pointd.x < 2*scr->w & pointc.y > -scr->h  & pointc.y < scr->h*2 & pointc.z-camd.pos.z >= 0)){goto skip;}
				px[Asel] = (int)pointd.x;
				py[Asel] = (int)pointd.y;
				sum+=pointc.z;
				
				Asel++;
			}
			zlm.fill_polygon(px,py,PS.edges[Esel].pointsC,PS.edges[Esel].color,-(sum/PS.edges[Esel].pointsC),scr);
			skip:
			Esel++;
		}
	}*/
};

namespace model_operation{
	void rotate_mesh(mesh &m,float3 rot,float4 pos){
		uint32_t p = 0;
		while(p < m.pointsC){
			m.points[p].pos.x -= pos.x;
			m.points[p].pos.y -= pos.y;
			m.points[p].pos.z -= pos.z;
			m.points[p].pos = rotate_matrix_in_grad(m.points[p].pos,rot);
			m.points[p].pos.x += pos.x;
			m.points[p].pos.y += pos.y;
			m.points[p].pos.z += pos.z;
			p++;
		}
		
	}
	void scale_mesh(mesh &m,float4 scale,float4 pos){
		uint32_t p = 0;
		while(p < m.pointsC){
			m.points[p].pos.x -= pos.x;
			m.points[p].pos.y -= pos.y;
			m.points[p].pos.z -= pos.z;
			m.points[p].pos = scale_matrix(m.points[p].pos,scale);
			m.points[p].pos.x += pos.x;
			m.points[p].pos.y += pos.y;
			m.points[p].pos.z += pos.z;
			p++;
		}
		
	}
}
#include "objreader.cpp"
namespace model{
	mesh create_sphere(float x,float y,float z,float r,point_ *p,uint32_t *edges,edge_ *edgesS){
		mesh sphere;
		sphere.edges = edgesS;
		/*point_ *p = (points_::point_*)malloc(sizeof(struct point_)*36*36);
		uint32_t *edges = (uint32_t *)malloc(36*36*4*4);*/
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
				//p[pC].color = 0xffffffff;
				//p[pC].edgecolor = 0xff;
				p[pC].pos.x = (arot.x*r)+x;
				p[pC].pos.y = (arot.y*r)+y;
				p[pC].pos.z = (arot.z*r)+z;
sphere.edges[sphere.edgesC].pointsC = 0;
				//if(pC + 37 < 36*36){
				//if(fmod(fy / 10,2) == 1){
					
				sphere.edges[sphere.edgesC].pointsC = 4;
				sphere.edges[sphere.edgesC].points = &*edges+(pC * 4);
				if(pC % 2 == 0){
				sphere.edges[sphere.edgesC].color = 0xffff;
			}	
				if(pC % 2 == 1){
				sphere.edges[sphere.edgesC].color = 0xff0000;
			}
				sphere.edges[sphere.edgesC].points[0] = pC;
				sphere.edges[sphere.edgesC].points[1] = pC+1;
				sphere.edges[sphere.edgesC].points[2] = pC+37;
				sphere.edges[sphere.edgesC].points[3] = pC+36;
				//}
				sphere.edgesC++;
				pC++;
				fx+=10;
			}
			fy+=10;
		}
		sphere.points = p;
		sphere.pointsC = 36*36;
		return sphere;
	}
}
using namespace model_operation;
class main_{
	private:
	class render_ render;
	class sdl_ sdl;
	class draw_ d;
	class zlayer_mask_ zlm;
	public:
	int main(HINSTANCE hInstance, HINSTANCE hpInstance, LPSTR nCmdLine, int iCmdShow){
		class obj2mesh o2m;
		o2m.openfile((char *)nCmdLine);
		//=============================================================
		class event_ event;
		class render_shaders_ rss;
		sdl.init_screen();
		//================================================================
		cam c;
		c.speed = 2;
		c.rotspeed = 2;
		
		c.pos.x = 0;
		c.pos.y = 0;
		c.pos.z = 0;
		c.rotation.x = 0;
		c.rotation.y = 0;
		c.rotation.z = 0;
		c.size.x = 2;
		c.size.y = 2;
		//====================================================================
		/*point_ *p = (mesh_::point_*)malloc(sizeof(struct point_)*36*36);
		uint32_t *edges = (uint32_t *)malloc(36*36*4*4);
		edge_ *edges_structs = (struct edge_ *)malloc(36*36*sizeof(edge_));*/
		uint32_t edgesbufC = 10000000;
		void *edgesbuf = malloc(edgesbufC);
		void *blurmem = malloc(sdl.scr->h*sdl.scr->w*4);
		zlm.init_layers_for_scr(sdl.scr);
		//========================================================
		mesh cube = o2m.convert();
		uint32_t printpoint = 0;
		while(printpoint < cube.pointsC){
			printf("point %i: x %f y %f z %f\n",printpoint,cube.points[printpoint].pos.x,cube.points[printpoint].pos.y,cube.points[printpoint].pos.z);
			printpoint++;
		}
		uint32_t printedge = 0;
		uint32_t printedgep = 0;
		while(printedge < cube.edgesC){
			printedgep = 0;
			printf("edges %i points:",printedge);
			while(printedgep < cube.edges[printedge].pointsC){
				printf("%i ",cube.edges[printedge].points[printedgep]);
				printedgep++;
			}
			printf("\n");
			printedge++;
		}
		//==============================================================================
		struct event_::keyboarddata keyd;
		//================================================================================
		float3 crot;
		crot.x = 0;
		crot.y = 5;
		crot.z = 0;
		float4 csca;
		csca.x = 2;
		csca.y = 2;
		csca.z = 2;
		float4 cpos;
		cpos.x = 0;
		cpos.y = 0;
		cpos.z = 0;
		scale_mesh(cube,csca,cpos);
		//=========================
		render_shader_::shader s0;
		s0.function = shaders::render::lineshader;
		render_shader_::shader s1;
		s1.function = shaders::render::edgeshader;
		render_shader_::shader s4;
		s4.function = shaders::render::randomcshader;
		render_shader_::shader s5;
		s5.function = shaders::render::rangeshader;
		rss.add_shader(s5);
		rss.add_shader(s0);
		rss.add_shader(s1);
		rss.add_shader(s4);

		int shader = 0;
		while(1){
		//rotate_mesh(cube,crot,cpos);
		//===============================================================================
		render.render(cube,c,edgesbuf,edgesbufC,zlm,rss,shader,sdl.scr);
		//d.rand_scr(1,1,sdl.scr,blurmem);
		//d.blur_scr(50,sdl.scr,blurmem);
		
		sdl.flip();
		d.fill(0,sdl.scr);
		//=================================================================================
		keyd = event.getkey();
		event.poll();
		if(keyd.type == SDL_KEYDOWN){
			if(keyd.code == SDLK_h){
				shader++;
				if(shader > rss.shadersC-1){shader = 0;}
				while(keyd.type == SDL_KEYDOWN){keyd = event.getkey();event.poll();}
			}
			if(keyd.type == SDL_KEYDOWN){
				if(keyd.code == SDLK_a){
					float4 r;
					float3 g = c.rotation;
					g.y  = g.y*-1;
					g.x = g.x*-1;
					r.x = -1;
					r.y = 0;
					r.z = 0;
					r.w = 0;
					r = rotate_matrix_in_grad(r,g);
					c.pos.x += r.x*c.speed;
					c.pos.y += r.y*c.speed;
					c.pos.z += r.z*c.speed;
					c.pos.w += r.w*c.speed;
				}
				if(keyd.code == SDLK_d){
					float4 r;
					float3 g = c.rotation;
					g.y  = g.y*-1;
					g.x = g.x*-1;
					r.x = 1;
					r.y = 0;
					r.z = 0;
					r.w = 0;
					r = rotate_matrix_in_grad(r,g);
					c.pos.x += r.x*c.speed;
					c.pos.y += r.y*c.speed;
					c.pos.z += r.z*c.speed;
					c.pos.w += r.w*c.speed;
				}
				if(keyd.code == SDLK_w){
					float4 r;
					float3 g = c.rotation;
					g.y  = g.y*-1;
					g.x = g.x*-1;
					r.x = 0;
					r.y = 0;
					r.z = 1;
					r.w = 0;
					r = rotate_matrix_in_grad(r,g);
					c.pos.x += r.x*c.speed;
					c.pos.y += r.y*c.speed;
					c.pos.z += r.z*c.speed;
					c.pos.w += r.w*c.speed;
				}
				if(keyd.code == SDLK_s){
					float4 r;
					float3 g = c.rotation;
					g.y  = g.y*-1;
					g.x = g.x*-1;
					r.x = 0;
					r.y = 0;
					r.z = -1;
					r.w = 0;
					r = rotate_matrix_in_grad(r,g);
					c.pos.x += r.x*c.speed;
					c.pos.y += r.y*c.speed;
					c.pos.z += r.z*c.speed;
					c.pos.w += r.w*c.speed;
				}
				if(keyd.code == SDLK_t){
					
				}
				if(keyd.code == SDLK_r){
					
				}
				if(keyd.code == SDLK_RIGHT){
					c.rotation.y -= c.rotspeed;
					if(c.rotation.y < 0){c.rotation.y = 360;}

				}
				if(keyd.code == SDLK_LEFT){
					c.rotation.y += c.rotspeed;
					if(c.rotation.y > 360){c.rotation.y = 0;}
				}
				if(keyd.code == SDLK_UP){
					c.rotation.x -= c.rotspeed;
					if(c.rotation.x < 0){c.rotation.x = 360;}
				}
				if(keyd.code == SDLK_DOWN){
					c.rotation.x += c.rotspeed;
					if(c.rotation.x > 360){c.rotation.x = 0;}
				}
			
				
			}
			//while(keyd.type == SDL_KEYDOWN){keyd = event.getkey();event.poll();}
		}
		}
		//==================================================================================================
		while(1){}
		return 0;
	}
};
int WinMain(HINSTANCE hInstance, HINSTANCE hpInstance, LPSTR nCmdLine, int iCmdShow){
	class main_ main;
	return main.main(hInstance,hpInstance,nCmdLine,iCmdShow);
}
