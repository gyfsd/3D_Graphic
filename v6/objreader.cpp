namespace str_{
	struct str_pointer{
		uint32_t pointer = 0;
		uint32_t size = 0;
	};
	uint32_t find(char *str,uint32_t size,char* f,uint32_t fsize,uint32_t repeat,uint32_t offset){
		uint32_t findp = 0;
		uint32_t checkp = 0;
		uint32_t r = 0;
		uint32_t maxp = size-fsize;
		while(findp + offset < size){
			if(str[findp + offset] == f[0]){
				checkp = 1;
				while(checkp < fsize){
					if(str[findp + offset + checkp] != f[checkp]){
						goto nope;
					}
					checkp++;
				}
				if(r == repeat){
					goto skip;
				}
				r++;
				
			}
			nope:
			findp++;
		}
		skip:
		if(findp+offset >= size){
			return -1;
		}
		return findp + offset;
	}
	uint32_t strcount(char *str,uint32_t size,char* f,uint32_t fsize){
		uint32_t findp = 0;
		uint32_t checkp = 0;
		uint32_t r = 0;
		uint32_t maxp = size-fsize;
		while(findp  < size){
			if(str[findp] == f[0]){
				checkp = 1;
				while(checkp < fsize){
					if(str[findp + checkp] != f[checkp]){
						goto nope;
					}
					checkp++;
				}
				r++;
				
			}
			nope:
			findp++;
		}
		return r;
	}
	float getnum(char *str,uint32_t size,uint32_t offset){
		uint32_t p = 0;
		char buf[100];
		int bx = 0;
		while(bx < 100){
			buf[bx] = 0;
			bx++;
		}
		char nums[] = "-0123456789.";
		uint32_t check = 0;
		while(p+offset < size){
			if(str[p+offset] == '/'){goto skip2;}
			check = 0;
			while(check < 12){
				if(str[p+offset] == nums[check]){
					buf[p] = str[p+offset];
					goto skip;
					
				}
				check++;
			}
			goto skip2;
			skip:
			p++;
		}
		skip2:
		return atof(buf);
	}
};
class obj2mesh{
	private:
	FILE* file;
	uint32_t fsize = 0;
	public:
	void openfile(char* PATH){
		file = fopen(PATH,"rb");
		fseek(file, 0L, SEEK_END);
		fsize = ftell(file);
	}
	mesh convert(){
		mesh m;
		char *f = (char *)malloc(fsize);
		fseek(file, 0, 0);
		fread(f,fsize,1,file);
		printf("converting obj to mesh\n");
		uint32_t vcount = str_::strcount(f,fsize,"v ",2);
		uint32_t vtcount = str_::strcount(f,fsize,"v ",2);
		uint32_t vncount = str_::strcount(f,fsize,"v ",2);
		uint32_t fcount = str_::strcount(f,fsize,"f ",2);
		printf("%i,",vcount);
		printf("%i\n",fcount);
		printf("%i\n",fsize);
		printf("%i\n",str_::find(f,fsize,"v ",2,0,0));
		point_ *p = (mesh_::point_*)malloc(sizeof(struct point_)*vcount);
		edge_ *edges = (mesh_::edge_ *)malloc(sizeof(struct edge_)*fcount);
		m.pointsC = vcount;
		m.points = p;
		uint32_t point = 0;
		uint32_t num = 0;
		uint32_t fnum = 0;
		uint32_t cur = 0;
		uint32_t nump;
		float numbuf = 0;
		char nums[] = "0123456789-";
		char numsp[] = "0123456789-.";
		char nbuf[] = {};
		while(point < vcount){
			cur = str_::find(f,fsize,"v ",2,0,cur+1);
			
			num = 0;
			fnum = 0;
			while(num < 3){
				while(fnum+cur < fsize){
					nump = 0;
					while(nump < 11){
						if(f[fnum+cur] == nums[nump]){
							goto skip0;
						}
						nump++;
					}
					fnum++;
				}
				skip0:
				numbuf = str_::getnum(f,fsize,fnum+cur);
				if(num == 0){
				m.points[point].pos.x = numbuf;
				}
				if(num == 1){
				m.points[point].pos.y = numbuf*-1;
				}
				if(num == 2){
				m.points[point].pos.z = numbuf;
				}
				skip1:
				nump = 0;
				while(nump < 12){
					if(f[fnum+cur] == numsp[nump]){
						fnum++;
						goto skip1;
					}
				nump++;
				}
				
				//nump = str_::find();
				num++;
			}
			printf("loadedpoint:%i cur:%i\n",point,cur);
			point++;
		}
		
		uint32_t edge = 0;
		uint32_t edgespC = 0;
		bool numb = 0;
		char numsx[] = "0123456789-./";
		cur = 0;
		while(edge < fcount){
			cur = str_::find(f,fsize,"f ",2,0,cur+1);
			num = 0;
			fnum = 0;
			while(f[cur+fnum] != 0x0a & cur+fnum < fsize){
				nump = 0;
				while(nump < 13){
					if(f[cur+fnum] == numsx[nump] & numb == 0){
						numb = 1;
						edgespC++;
					}
					if(f[cur+fnum] == 0x20){numb = 0;}
					nump++;
				}
				fnum++;
			}
			edge++;
		}
		printf("epcount:%i\n",edgespC);
		uint32_t *edgesP = (uint32_t *)malloc(edgespC*8);
		edge = 0;
		while(edge < edgespC){
			edgesP[edge] = 0;
			edge++;
		}
		uint32_t edgepbuf = 0;
		uint32_t edgep = 0;
		uint32_t freep = 0;
		cur = 0;
		edge = 0;
		while(edge < fcount){
			cur = str_::find(f,fsize,"f ",2,0,cur+1);
			num = 0;
			fnum = 0;
			edgep = 0;
			numb = 0;
			
			edges[edge].points = edgesP+(freep);
			while(f[cur+fnum] != 0x0a & cur+fnum < fsize){
				nump = 0;
				while(nump < 13){
					if(f[cur+fnum] == numsx[nump] & numb == 0){
						numb = 1;
						edgepbuf  = str_::getnum(f,fsize,fnum+cur)-1;
						edges[edge].points[edgep] = edgepbuf;
						edges[edge].color = rand();
						freep++;
						edgep++;
						goto skip4;
					}
					if(f[cur+fnum] == 0x20){numb = 0;}
					nump++;
				}
				skip4:
				fnum++;
			}
			edges[edge].pointsC = edgep;
			edge++;
		}
		m.edgesC = fcount;
		m.edges = edges;
		printf("model importeed\n");
		return m;
	}
};
