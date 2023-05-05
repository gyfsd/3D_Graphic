#include <math.h>
#define PI 3.14159265359
class sdl_{
	private:
	public:
	//SDL Init() flags
	uint32_t initflags = SDL_INIT_VIDEO; //Init this
	//SDL SetVideoMode() settings
	int sx = 500; //Window size x
	int sy = 500; //Window size y
	int bp = 32; //color type
	uint32_t modeflags = SDL_HWSURFACE | SDL_RESIZABLE;
	//scr
	SDL_Surface *scr;
	//event
	//SDL_Event event;
	//event type
	//uint32_t event_type = SDL_USEREVENT;
	//initiliaze event function
	/*void init_event(){
		
	}*/
	//initiliaze sdl function
	void init_screen(){
		SDL_Init(initflags);
		scr = SDL_SetVideoMode(sx,sy,bp,modeflags);
	}
	//flip
	void flip(){
		SDL_Flip(scr);
	}
	
};
class event_{
	private:
	
	public:
	//For comfort
	struct motiondata{
		int type;
		int x;
		int y;
		uint32_t winid;
	};
	struct buttondata{
		int type;
		int clickx;
		int clicky;
		int button;
	};
	struct keyboarddata{
		int type;
		int scancode;
		int code;
	};
	SDL_Event event;
	void poll(){
		SDL_PollEvent(&event); //polls event
	}
	struct motiondata getmot(){
		struct motiondata a;
		//a.winid = event.motion.windowID;
		a.type = event.motion.type;
		a.x = event.motion.x;
		a.y = event.motion.y;
		return a;
	}
	struct buttondata getclick(){
		struct buttondata a;
		a.type = event.button.type;
		a.clickx = event.button.x;
		a.clicky = event.button.y;
		a.button = event.button.button;
		return a;
	}
	struct keyboarddata getkey(){
		struct keyboarddata a;
		a.type = event.key.type;
		a.scancode = event.key.keysym.scancode;
		a.code = event.key.keysym.sym;
		return a;
	}
};
class alpha_{
	private:
	
	public:
	uint8_t mixchannel(uint8_t lowlevel,uint8_t highlevel,uint8_t alpha){
		return lowlevel+(highlevel*alpha/255);
	}
	uint32_t mixcolor(uint32_t lowlevel,uint32_t highlevel,uint8_t alpha){
		uint32_t output = 0;
		uint8_t *out = (uint8_t * )&output;
		uint8_t *hl = (uint8_t * )&highlevel;
		uint8_t *ll = (uint8_t * )&lowlevel;
		out[1] = mixchannel(ll[1],hl[1],hl[0]);
		out[2] = mixchannel(ll[2],hl[2],hl[0]);
		out[3] = mixchannel(ll[3],hl[3],hl[0]);
		return output;
	}
};
class draw_{
	private:
	class alpha_ alpha;
	public:
	 void fill_polygon(int *pointsx,int *pointsy,uint32_t pointsC,uint32_t col,SDL_Surface *scr){
                uint32_t point = 0;
                float sumx = 0;
                float sumy = 0;
                while(point < pointsC){
                        sumx+=pointsx[point];
                        sumy+=pointsy[point];
                        point++;
                }
                int centerx = sumx/pointsC;
                int centery = sumy/pointsC;
				float maxr = 0;
				float r = 0;
				point = 0;
				while(point < pointsC){
					r = sqrt(pow(pointsx[point]-centerx,2)+pow(pointsy[point]-centery,2));
					if(r > maxr){maxr = r;}
					point++;
				}
                float scale = 1;
                float scalesub = 1/maxr;

                while(scale > 0){
                        point = 0;
                        while(point < pointsC){
                                int pcenx = pointsx[point]-centerx;
                                int pceny = pointsy[point]-centery;
                                pcenx = pcenx*scale;
                                pceny = pceny*scale;
                                pceny = pceny+centery;
                                pcenx = pcenx+centerx;
                                int tpcenx = pointsx[(point+1)%pointsC]-centerx;
                                int tpceny = pointsy[(point+1)%pointsC]-centery;
                                tpcenx = tpcenx*scale;
                                tpceny = tpceny*scale;
                                tpceny = tpceny+centery;
                                tpcenx = tpcenx+centerx;
                                line(pcenx,pceny,tpcenx,tpceny,col,scr);
                                point++;
                        }
                        scale -= scalesub;
                }
        }

	void copypixel(int x,int y,int px,int py,SDL_Surface *scr){
		uint32_t *p = (uint32_t * )scr->pixels;
		if(x < scr->w & px < scr->w & y < scr->h & py < scr->h){
			if(x >= 0 & y >= 0 & px >= 0 & py >= 0){
				p[px+(py*scr->w)] = p[x+(y*scr->w)];
			}
		}
	}
	void circle(int x,int y,float r,uint32_t col,SDL_Surface *scr){
		uint32_t *p = (uint32_t * )scr->pixels;
		float g = 0;
		float rc = 0;
			rc = 0;
			while(rc < r){
				g = 0;
				float ar = 360/(3.14*2*rc);
				while(g < 360){
					setpixel(x+(sin(g/180*PI)*rc),y+(cos(g/180*PI)*rc),col,scr);
					g+=ar;
				}
				rc+=1;
			}
	}
	
	void rand_scr(uint32_t rad,int seed,SDL_Surface *scr,void *mem){
			uint32_t *blurbuf = (uint32_t *)mem;
			uint32_t *p = (uint32_t *)scr->pixels;
			uint8_t *p8 = (uint8_t *)p;
			uint8_t *b8 = (uint8_t *)blurbuf;
			int x = 0;
			int y = 0;
			float r,g;
			//srand(seed);
			while(x < scr->w){
			y = 0;
			while(y < scr->h){
				r = fmod(rand(),rad);
				g = rand();
				float ax = ((float)(sin(g/180*3.14159265359)*r)+(float)x);
				float ay = ((float)(cos(g/180*3.14159265359)*r)+(float)y);
				setpixelbuf(x,y,getpixelbuf((uint32_t)ax,(uint32_t)ay,p,scr->w,scr->h),blurbuf,scr->w,scr->h);
				if((int)ax < 1 | (int)ay < 1 | (int)ax >= scr->w | (int)ay >= scr->h){
					setpixelbuf(x,y,getpixelbuf(x,y,p,scr->w,scr->h),blurbuf,scr->w,scr->h);
				}
				y++;
			}
			x++;
		}
		uint32_t d = 0;
		while(d < scr->h*scr->w){
			p[d] = blurbuf[d];
			d++;
		}
	}
	void blur_scr(uint32_t rad,SDL_Surface *scr,void *mem){
		uint32_t *blurbuf = (uint32_t *)mem;
		uint32_t *p = (uint32_t *)scr->pixels;
		uint8_t *p8 = (uint8_t *)p;
		uint8_t *b8 = (uint8_t *)blurbuf;
		int x = 0;
		int y = 0;
		float r;
		float g;
		float c = 0;
		float ar;
		int ax,ay;
		float suma,sumb,sumc,sumd;
		while(x < scr->w){
			y = 0;
			while(y < scr->h){
				suma = 0;
				sumb = 0;
				sumc = 0;
				sumd = 0;
				c = 0;
				r = 0;
					while(r < rad){
						g = 0;
						ar = 360/(3.14*2*r)*2;
						while(g < 360){
							ax = x+(r*sin(g/180*3.14));
							ay = y+(r*cos(g/180*3.14));
							if(ax > -1 & ay > -1 & ax < scr->w & ay < scr->h){
								suma += p8[((ax+(ay*scr->w))*4)];
								sumb += p8[((ax+(ay*scr->w))*4)+1];
								sumc += p8[((ax+(ay*scr->w))*4)+2];
								sumd += p8[((ax+(ay*scr->w))*4)+3];
							}
							c++;
							g+=ar;
						}
						r+=2;
					}
				
				b8[((x+(y*scr->w))*4)] = suma/c;
				b8[((x+(y*scr->w))*4)+1] = sumb/c;
				b8[((x+(y*scr->w))*4)+2] = sumc/c;
				b8[((x+(y*scr->w))*4)+3] = sumd/c;
				y++;
			}
			x++;
		}
		uint32_t d = 0;
		while(d < scr->h*scr->w){
			p[d] = blurbuf[d];
			d++;
		}
		
	}
	void copyrect(int sx,int sy,int ex,int ey,int px,int py,SDL_Surface *scr){
	uint32_t *p = (uint32_t * )malloc(scr->h*scr->w*4);
	uint32_t *pixels = (uint32_t * )scr->pixels;
	int cx = 0;
	int cy = 0;
	while(cx < scr->w){
		cy  = 0;
		while(cy < scr->h){
			p[(cy*scr->w)+cx] = pixels[(cy*scr->w)+cx];
			cy++;
		}
		cx++;
	}
	if(sx > -1 & sy > -1){
	int x = sx;
	int y = 0;
	while(y <= ey & x < scr->h){
	x = sx;
	while(x <= ex & x < scr->w){
	pixels[((py+y)*scr->w)+(px+x)] = p[((y)*scr->w)+(x)];
	x++;
	}
	y++;
	}
	}
	}
	void fill(uint32_t color,SDL_Surface *scr){
	int x = 0;
	int y = 0;
	uint32_t *p = (uint32_t * )scr->pixels;
	while(y < scr->h){
		x = 0;
		while(x < scr->w){
			p[((y*scr->w)+x)] = color;
			x++;
		}
		y++;
	}
	}
	void drawimage(int sx,int sy,int imgx,int imgy,SDL_Surface *scr,uint32_t *img){
		int x = 0;
		int y = 0;
		uint32_t *p = (uint32_t * )scr->pixels;
		while(y < imgy & y+sy < scr->h){
			x = 0;
			while(x < imgx & x+sx < scr->w){
				uint32_t color = img[(y*imgx)+(x)];
				uint8_t colorbuf[4];
				uint8_t *prevcol = (uint8_t *)(&*p+((y*imgx)+(x)));
				uint8_t *cb = (uint8_t *)(&*img+((y*imgx)+(x)));
				colorbuf[0] = alpha.mixchannel(prevcol[2],cb[2],cb[3]);
				colorbuf[1] = alpha.mixchannel(prevcol[1],cb[1],cb[3]);
				colorbuf[2] = alpha.mixchannel(prevcol[0],cb[0],cb[3]);
				p[(x+sx)+((y+sy) * scr->w)] = (uint32_t)&colorbuf;
				x++;
				}

			y++;
			}
	}
	void drawpicture(int sx,int sy,int imgx,int imgy,SDL_Surface *scr,uint32_t *img){
		int x = 0;
		int y = 0;
		uint32_t *p = (uint32_t * )scr->pixels;
		uint8_t *pb = (uint8_t * )scr->pixels;
		while(y < imgy & y+sy < scr->h){
			x = 0;
			while(x < imgx & x+sx < scr->w){
				uint32_t color = img[(y*imgx)+(x)];
				uint8_t *color_a = (uint8_t * )&color;
				uint32_t color_buf = color;
				uint8_t *cb = (uint8_t *)(&color_buf);
				uint8_t *prevcol = (uint8_t *)(&*p+(y*imgx)+(x));
				color_a[0] = alpha.mixchannel(prevcol[2],cb[2],cb[3]);
				color_a[1] = alpha.mixchannel(prevcol[1],cb[1],cb[3]);
				color_a[2] = alpha.mixchannel(prevcol[0],cb[0],cb[3]);
				p[(x+sx)+((y+sy) * scr->w)] = color;
				
				x++;
				}

			y++;
			}
	}
	void rect(int sx,int sy,int ex,int ey,uint32_t val,SDL_Surface *scr){
	uint32_t *p = (uint32_t * ) scr->pixels;
	if(sx > -1 & sy > -1){
	int x = sx;
	int y;
	while(x <= ex & x < scr->w){
	y = sy;
	while(y <= ey & y < scr->h){
	p[x+(y*scr->w)] = val;
	y++;
	}
	x++;
	}
	}
	}
	void line(int sx,int sy,int ex,int ey,uint32_t val,SDL_Surface *scr){
		int cbuf;
		float range = sqrt(fabs(pow(ex-sx,2)+pow(ey-sy,2)));
		float ax = float(ex-sx)/range;
		float ay = float(ey-sy)/range;
		float cx = sx;
		float cy = sy;
		uint32_t *p = (uint32_t * )scr->pixels;
		float r = 0;
		while(r < range & cx < scr->w & cy < scr->h & cx > -1 & cy > -1){
			setpixel((int)cx,(int)cy,val,scr);
			cx+=ax;
			cy+=ay;
			r++;
		}
		ax = float(sx-ex)/range;
		ay = float(sy-ey)/range;
		cx = ex;
		cy = ey;
		while(r < range & cx < scr->w & cy < scr->h & cx > -1 & cy > -1){
			setpixel((int)cx,(int)cy,val,scr);
			cx+=ax;
			cy+=ay;
			r++;
		}
		
		
	}
	void drawimage_on_buffer(int sx,int sy,int imgx,int imgy,int bufx,int bufy,uint32_t *p,uint32_t *img){
		int x = 0;
		int y = 0;
		while(y < imgy & y+sy < bufy){
			x = 0;
			while(x < imgx & x+sx < bufx){
				uint32_t color = img[(y*imgx)+(x)];
				p[(x+sx)+((y+sy) * bufx)] = color;
				
				x++;
				}

			y++;
			}
	}
	void setpixel(int x,int y,uint32_t val,SDL_Surface *scr){
	if(x > -1 & y > -1 & x < scr->w & y < scr->h){
	((uint32_t *)scr->pixels)[x+(y*scr->w)] = val;
	}
	}
	uint32_t getpixel(int x,int y,SDL_Surface *scr){
	uint32_t *p = (uint32_t * )scr->pixels;
	if((scr->w*scr->h) > x+(y*scr->w) & x+(y*scr->w) > -1){
	return p[(x+(y * scr->w ))];
	}
	}
	void setpixelbuf(int x,int y,uint32_t val,uint32_t *p,int sx,int sy){
	if(x > -1 & y > -1 & x < sx & y < sy){
	p[x+(y*sx)] = val;}
	}
	uint32_t getpixelbuf(int x,int y,uint32_t *p,int sx,int sy){
	if(x > -1 & y > -1 & x < sx & y < sy){
	return p[(x+(y * sx ))];
	}
	}
	
};
class zlayer_mask_{
	private:
	class draw_ draw;
	public:
	float	*scr_layers;
	bool setbit = 0;
	int sx,sy;
	void init_layers_for_scr(SDL_Surface *scr){
		scr_layers = (float *)malloc(scr->w*scr->h*4);
		sx = scr->w;
		sy = scr->h;
		int x = 0;
		int y = 0;
		while(x < sx){
			y = 0;
			while(y < sx){
				setlayer(x,y,0);
				y++;
			}
			x++;
		}
	}
	float getlayer(int x,int y){
		if(x > -1 & x < sx & y > -1 & y < sy){
			return scr_layers[x+(y*sx)];
		}
	}
	void setlayer(int x,int y,float layer){
		if(x > -1 & x < sx & y > -1 & y < sy){
			scr_layers[x+(y*sx)] = layer;
		}
		
	}
	void setpixel(int x,int y,uint32_t col,float layer,SDL_Surface *scr){
		float Prev_layer = getlayer(x,y);
		if(layer > Prev_layer){
			draw.setpixel(x,y,col,scr);
			setlayer(x,y,layer);
			goto skip;
		}
		if(setbit == 0){
			draw.setpixel(x,y,col,scr);
			setbit = 1;
		}
		skip:
		return;
	}
	void fill(float layer){
		int x = 0;
		int y = 0;
		while(x < sx){
			y = 0;
			while(y < sx){
				setlayer(x,y,layer);
				y++;
			}
			x++;
		}
	}
	void line(int sx,int sy,int ex,int ey,uint32_t val,float layer,SDL_Surface *scr){
		int cbuf;
		float range = sqrt(fabs(pow(ex-sx,2)+pow(ey-sy,2)));
		float ax = float(ex-sx)/range;
		float ay = float(ey-sy)/range;
		float cx = sx;
		float cy = sy;
		uint32_t *p = (uint32_t * )scr->pixels;
		float r = 0;
		while(r < range & cx < scr->w & cy < scr->h & cx > -1 & cy > -1){
			setpixel((int)cx,(int)cy,val,layer,scr);
			cx+=ax;
			cy+=ay;
			r++;
		}
		ax = float(sx-ex)/range;
		ay = float(sy-ey)/range;
		cx = ex;
		cy = ey;
		while(r < range & cx < scr->w & cy < scr->h & cx > -1 & cy > -1){
			setpixel((int)cx,(int)cy,val,layer,scr);
			cx+=ax;
			cy+=ay;
			r++;
		}
		
		
	}
	void fill_polygon(int *pointsx,int *pointsy,uint32_t pointsC,uint32_t col,float layer,float quality,SDL_Surface *scr){
                uint32_t point = 0;
                float sumx = 0;
                float sumy = 0;
                while(point < pointsC){
                        sumx+=pointsx[point];
                        sumy+=pointsy[point];
                        point++;
                }
                int centerx = sumx/pointsC;
                int centery = sumy/pointsC;
				float rsum = 0;
				float r = 0;
				point = 0;
				while(point < pointsC){
					r = sqrt(pow(pointsx[point]-centerx,2)+pow(pointsy[point]-centery,2));
					rsum+=r;
					point++;
				}
				bool b = 0;
				point = 0;
				while(point < pointsC){
					if(pointsx[point] > -1 & pointsx[point] < scr->w & pointsy[point] > -1 & pointsy[point] < scr->h){b = 1;}
					point++;
				}
				if(b == 0){return;}
                float scale = 0;
                float scaleadd = (quality/(rsum/point));
				if(scaleadd < 0.01){
					scaleadd = 0.01;
				}
				int pcenx,pceny,tpcenx,tpceny;
                while(scale <= 1){
                        point = 0;
                        while(point < pointsC){
                                pcenx = ((pointsx[point]-centerx)*scale)+centerx;
                                pceny = ((pointsy[point]-centery)*scale)+centery;
                                tpcenx = ((pointsx[(point+1)%pointsC]-centerx)*scale)+centerx;
                                tpceny = ((pointsy[(point+1)%pointsC]-centery)*scale)+centery;
                                line(pcenx,pceny,tpcenx,tpceny,col,layer,scr);
                                point++;
                        }
                        scale += scaleadd;
                }
        }
};

class interface_{
	private:
	class draw_ d;
	public:
	struct button{
		int startx;
		int starty;
		int sizex;
		int sizey;
		int clicktype;
		uint32_t *image;
	};
	struct button buttons[100];
	int set_image(int id,uint32_t *image){
		buttons[id].image = image;
	}
	int draw_button(int id,SDL_Surface *scr){
		d.drawpicture(buttons[id].startx,buttons[id].starty,buttons[id].sizex,buttons[id].sizey,scr,buttons[id].image);
	}
	int createbutton(int id,int startx,int starty,int sizex,int sizey,int clicktype){
		//buttons[id] = (interface_::button)malloc(sizeof(struct button));
		buttons[id].startx = startx;
		buttons[id].starty = starty;
		buttons[id].sizex = sizex;
		buttons[id].sizey = sizey;
		buttons[id].clicktype = clicktype;//SDL_BUTTON_LEFT and more
	}
	int checkbutton(int id,class event_ event){
		event.poll();
		if(event.getmot().x > buttons[id].startx & event.getmot().x < (buttons[id].sizex)+buttons[id].startx & event.getmot().y > buttons[id].starty & event.getmot().y < buttons[id].sizey+buttons[id].starty & event.getclick().button == buttons[id].clicktype & event.getclick().type == SDL_MOUSEBUTTONDOWN){
			return 1;
		}
		return 0;
	};
};
class font_{
	private:
	class draw_ d;
	public:
	int csizex = 31;//symbol size x
	int csizey = 30;//symbol size y
	uint32_t *smap;//symbols picture
	int smapx = 1000;//symbols picture size x
	int smapy = 30;//symbols picture size y
	char alphabet[255] = "abcdefghijklmnopqrstuvwxyz";//symbols filter
	int charcount = 27;//symbols count
	int font_file(char* file){//opens and saves to smap font file
		FILE* font_f = fopen(file,"rb");
		uint32_t *font_i = (uint32_t * )malloc(smapx*smapy*4);
		fread(font_i,smapx,smapy*4,font_f);
		smap = font_i;
	}
	int draw_char(int sx,int sy,int id,SDL_Surface *scr){//draw one symbol
		uint32_t *cbuf = (uint32_t * )malloc(4*csizex*csizey);
		int x = 0;
		int y = 0;
		while(y < csizey){
			x = 0;
			while(x < csizex){
				cbuf[x+(y*csizex)] = smap[(x+(id*csizex))+(y*smapx)];
				x++;
			}
			y++;
		}
		d.drawpicture(sx,sy,csizex,csizey,scr,cbuf);
	}
	int text(int sx,int sy,char* text,int textsize,SDL_Surface *scr){//print text
		int symbol = 0;
		while(symbol < textsize){
			int findcur = 0;
			int b = 0;
			while(findcur < charcount & b == 0){
				if(alphabet[findcur] == text[symbol]){
					b = 1;
					findcur--;
				}
				findcur++;
			}
			if(b != 0){
				draw_char(sx+(symbol*csizex),sy,findcur,scr);	
			}
			symbol++;
		}
	}
};
