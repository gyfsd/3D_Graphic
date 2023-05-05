namespace shaders{
	namespace render{
		namespace edge{
			float zlayer(mesh m,cam camd,uint32_t edge){
				uint32_t Asel = 0;
				float sum = 0;
				float zlayer;
				while(Asel < m.edges[edge].pointsC){
					float3 pointc = pointop::point_to_2d(m.points[m.edges[edge].points[Asel]].pos,camd);
					sum+=pointc.z;
					Asel++;
				}
				zlayer = sum/m.edges[edge].pointsC;
				return zlayer;
			}
		}
		namespace optimization{
			bool draw_edge_or_no(mesh m,cam camd,uint32_t edge){
				float zlayer = shaders::render::edge::zlayer(m,camd,edge);
				float anotherzlayer;
				float3 checkpointd;
				float3 checkedgepd;
				uint32_t checkpoint;
				uint32_t checkedge = 0;
				uint32_t checkedgep;
				float2 checkv;
				float2 checkv2;
				float dot;
				while(checkedge < m.edgesC){
					anotherzlayer = shaders::render::edge::zlayer(m,camd,checkedge);
					if(anotherzlayer > zlayer){
						checkpoint = 0;
						while(checkpoint < m.edges[edge].pointsC){
							checkpointd = pointop::point_to_2d(m.points[m.edges[edge].points[checkpoint]].pos,camd);
							checkedgep = 0;
							while(checkedgep < m.edges[checkedge].pointsC-1){
								checkedgepd = pointop::point_to_2d(m.points[m.edges[checkedge].points[0]].pos,camd);
								checkv.x = checkedgepd.x-checkpointd.x;
								checkv.y = checkedgepd.y-checkpointd.y;
								checkedgepd = pointop::point_to_2d(m.points[m.edges[checkedge].points[(checkedgep)%m.edges[checkedge].pointsC]].pos,camd);
								checkv2.x = checkedgepd.x-checkpointd.x;
								checkv2.y = checkedgepd.y-checkpointd.y;
								dot = vector_op::scalar_product_float2(checkv,checkv2);
								if(dot > 0 | dot < 0){
									return 1;
								}
								
								checkedgep++;
							}
							checkpoint++;
						}
					}
					checkedge++;
				}
				return 0;
			}
		};
		void *effectsmem0 = NULL;
		float edge_quality = 1;
		void lineshader(mesh m,cam camd,void *mem,uint32_t memC,class zlayer_mask_ zlm,SDL_Surface *scr){
				class draw_ d;
				camd.rotation.x = fabs(fmod(camd.rotation.x,360));
				camd.rotation.y = fabs(fmod(camd.rotation.y,360));
				camd.rotation.z = fabs(fmod(camd.rotation.z,360));
				uint32_t Psel = 0;
				while(Psel < m.pointsC){
					pointop::draw_point(m.points[Psel],camd,scr);
					Psel++;
				}
				uint32_t Esel = 0;
				uint32_t Asel = 0;
				while(Esel < m.edgesC){
					//if(shaders::render::optimization::draw_edge_or_no(m,camd,Esel) == 1){
					Asel = 0;
					while(Asel < m.edges[Esel].pointsC){
							if(Asel < m.edges[Esel].pointsC){
							pointop::draw_line(m.points[m.edges[Esel].points[Asel]],m.points[m.edges[Esel].points[Asel+1]],camd,scr);
							}
						Asel++;
					}
					//}
					Esel++;
			}
		}
		void edgeshader(mesh m,cam camd,void *mem,uint32_t memC,class zlayer_mask_ zlm,SDL_Surface *scr){
			camd.rotation.x = fabs(fmod(camd.rotation.x,360));
			camd.rotation.y = fabs(fmod(camd.rotation.y,360));
			camd.rotation.z = fabs(fmod(camd.rotation.z,360));
			uint32_t Esel = 0;
			uint32_t Asel = 0;
			int *px = (int*)(mem);
			int *py = (int*)(mem+(memC/2));
			float sum = 0;
			zlm.fill(-923841095);
			zlm.setbit = 0;
			while(Esel < m.edgesC){
				Asel = 0;
				sum = 0;
				while(Asel < m.edges[Esel].pointsC){
					float3 pointc = pointop::point_to_2d(m.points[m.edges[Esel].points[Asel]].pos,camd);
					float2 pointd = pointop::td_to_scr(pointc,camd.size,scr->w,scr->h);
					if(!(pointd.x > -scr->w & pointd.x < 2*scr->w & pointc.y > -scr->h  & pointc.y < scr->h*2 & pointc.z-camd.pos.z >= 0)){goto skip;}
					px[Asel] = (int)pointd.x;
					py[Asel] = (int)pointd.y;
					sum+=pointc.z;
					Asel++;
				}
				zlm.fill_polygon(px,py,m.edges[Esel].pointsC,m.edges[Esel].color,-(sum/m.edges[Esel].pointsC),edge_quality,scr);
				skip:
				Esel++;
			}
		}
		void randomshader(mesh m,cam camd,void *mem,uint32_t memC,class zlayer_mask_ zlm,SDL_Surface *scr){
			class draw_ d;
			edgeshader(m,camd,mem,memC,zlm,scr);
			if(effectsmem0 == NULL){
				effectsmem0 = malloc(scr->h*scr->w*4);
			}
			d.rand_scr(10,rand(),scr,effectsmem0);
		}
		void blurshader(mesh m,cam camd,void *mem,uint32_t memC,class zlayer_mask_ zlm,SDL_Surface *scr){
			class draw_ d;
			edgeshader(m,camd,mem,memC,zlm,scr);
			if(effectsmem0 == NULL){
				effectsmem0 = malloc(scr->h*scr->w*4);
			}
			d.blur_scr(5,scr,effectsmem0);
		}
		void randomcshader(mesh m,cam camd,void *mem,uint32_t memC,class zlayer_mask_ zlm,SDL_Surface *scr){
			camd.rotation.x = fabs(fmod(camd.rotation.x,360));
			camd.rotation.y = fabs(fmod(camd.rotation.y,360));
			camd.rotation.z = fabs(fmod(camd.rotation.z,360));
			uint32_t Esel = 0;
			uint32_t Asel = 0;
			int *px = (int*)(mem);
			int *py = (int*)(mem+(memC/2));
			float sum = 0;
			zlm.fill(-923841095);
			zlm.setbit = 0;
			while(Esel < m.edgesC){
				Asel = 0;
				sum = 0;
				while(Asel < m.edges[Esel].pointsC){
					float3 pointc = pointop::point_to_2d(m.points[m.edges[Esel].points[Asel]].pos,camd);
					float2 pointd = pointop::td_to_scr(pointc,camd.size,scr->w,scr->h);
					if(!(pointd.x > -scr->w & pointd.x < 2*scr->w & pointc.y > -scr->h  & pointc.y < scr->h*2 & pointc.z-camd.pos.z >= 0)){goto skip;}
					px[Asel] = (int)pointd.x;
					py[Asel] = (int)pointd.y;
					sum+=pointc.z;
					Asel++;
				}
				srand(rand());
				zlm.fill_polygon(px,py,m.edges[Esel].pointsC,rand()+(rand()*65536),-(sum/m.edges[Esel].pointsC),edge_quality,scr);
				skip:
				Esel++;
			}
		}
		void rangeshader(mesh m,cam camd,void *mem,uint32_t memC,class zlayer_mask_ zlm,SDL_Surface *scr){
			camd.rotation.x = fabs(fmod(camd.rotation.x,360));
			camd.rotation.y = fabs(fmod(camd.rotation.y,360));
			camd.rotation.z = fabs(fmod(camd.rotation.z,360));
			uint32_t Esel = 0;
			uint32_t Asel = 0;
			uint32_t l;
			int *px = (int*)(mem);
			int *py = (int*)(mem+(memC/2));
			float sum = 0;
			float edgelayer;
			zlm.fill(-923841095);
			zlm.setbit = 0;
			float3 pointc;
			float3 bpointc;
			float2 pointd;
			bool b = 0;
			while(Esel < m.edgesC){
				Asel = 0;
				sum = 0;
				pointc = pointop::point_to_2d(m.points[m.edges[Esel].points[m.edges[Esel].pointsC-1]].pos,camd);
				b = 0;
				while(Asel < m.edges[Esel].pointsC){
					bpointc = pointc;
					pointc = pointop::point_to_2d(m.points[m.edges[Esel].points[Asel]].pos,camd);
					pointd = pointop::td_to_scr(pointc,camd.size,scr->w,scr->h);
					if(pointd.x > -1 & pointd.y > -1 & pointd.x < scr->w & pointd.y < scr->h){
						b = 1;
					}
					if(pointc.z-camd.pos.z <= 0){
						/*if(!(bpointc.z-camd.pos.z > 0)){
							goto skip;
						}*/
						goto skip;
					}
					px[Asel] = (int)pointd.x;
					py[Asel] = (int)pointd.y;
					sum+=pointc.z;
					Asel++;
				}
				if(b == 1){
				l = (uint32_t)((((sum/m.edges[Esel].pointsC)-camd.pos.z)*1));
				edgelayer = -(sum/m.edges[Esel].pointsC);
				if(l < 200){
					zlm.fill_polygon(px,py,m.edges[Esel].pointsC,((200-l)+54)*0x01010101,edgelayer,edge_quality,scr);
				}	
				if(l >= 200){
					zlm.fill_polygon(px,py,m.edges[Esel].pointsC,(54)*0x01010101,edgelayer,edge_quality,scr);
				}
				}
				skip:
				Esel++;
			}		
		}
	}
}
