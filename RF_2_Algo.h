#ifndef ML_ALGO
#define ML_ALGO

#define __USE_C99_MATH

#include <stdbool.h>
#include <stdint.h>

bool RF_predict(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z);
#define totaltrees 10

/*********** Tree 0 *************/
static bool tree0(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z){
  if(dx <= 141) {
    if(dz <= 167) {
      if(yavg <= -157) {
        return 1; } 
         
      else {  // if yavg > -157
        if(dz <= -200) {
          if(xavg <= 12) {
            return 1; } 
             
          else {  // if xavg > 12
            return 1; } 
		}
             
        else {  // if dz > -200
          if(dx <= -339) {
            if(dy <= -82) {
              return 1; } 
               
            else {  // if dy > -82
              return 1; } 
		  }
               
          else {  // if dx > -339
            if(dy <= -186) {
              return 0; } 
               
            else {  // if dy > -186
              return 0; } 
		  }
		}
	  }
	}
               
    else {  // if dz > 167
      if(dz <= 204) {
        return 1; } 
         
      else {  // if dz > 204
        if(dx <= -66) {
          return 1; } 
           
        else {  // if dx > -66
          return 1; } 
	  }
	}
  }
           
  else {  // if dx > 141
    if(x <= 369) {
      if(dx <= 294) {
        if(xavg <= 29) {
          return 1; } 
           
        else {  // if xavg > 29
          if(xavg <= 186) {
            return 0; } 
             
          else {  // if xavg > 186
            return 0; } 
		}
	  }
             
      else {  // if dx > 294
        if(dy <= -42) {
          return 0; } 
           
        else {  // if dy > -42
          return 1; } 
	  }
	}
           
    else {  // if x > 369
      if(x <= 451) {
        return 1; } 
         
      else {  // if x > 451
        return 1; } 
	}
  }
}
         
/*********** Tree 1 *************/
static bool tree1(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z){
  if(dx <= -187) {
    if(z <= 2) {
      return 1; } 
       
    else {  // if z > 2
      if(dx <= -336) {
        if(z <= 154) {
          return 1; } 
           
        else {  // if z > 154
          return 1; } 
	  }
           
      else {  // if dx > -336
        if(y <= 94) {
          return 1; } 
           
        else {  // if y > 94
          return 0; } 
	  }
	}
  }
           
  else {  // if dx > -187
    if(x <= 336) {
      if(dz <= -244) {
        return 1; } 
         
      else {  // if dz > -244
        if(dy <= -186) {
          if(x <= 136) {
            return 0; } 
             
          else {  // if x > 136
            if(dz <= -57) {
              return 0; } 
               
            else {  // if dz > -57
              return 1; }
		  }
		}		  
               
        else {  // if dy > -186
          if(dz <= 163) {
            if(yavg <= -35) {
              return 0; } 
               
            else {  // if yavg > -35
              return 0; }
		  }			  
               
          else {  // if dz > 163
            if(zavg <= 48) {
              return 1; } 
               
            else {  // if zavg > 48
              return 0; }
		  }
		}
	  }
	}	  
               
    else {  // if x > 336
      if(x <= 451) {
        if(y <= 120) {
          return 1; } 
           
        else {  // if y > 120
          if(dz <= 16) {
            return 0; } 
             
          else {  // if dz > 16
            return 0; }
		}
	  }		
             
      else {  // if x > 451
        return 1; } 
	}
  }
}
         
/*********** Tree 2 *************/
static bool tree2(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z){
  if(xavg <= 313) {
    if(dz <= 152) {
      if(dy <= -186) {
        if(zavg <= 49) {
          return 1; } 
           
        else {  // if zavg > 49
          if(y <= -40) {
            return 1; } 
             
          else {  // if y > -40
            return 0; } 
		}
	  }
             
      else {  // if dy > -186
        if(zavg <= -256) {
          return 1; } 
           
        else {  // if zavg > -256
          if(dy <= 246) {
            if(dz <= -236) {
              return 1; } 
               
            else {  // if dz > -236
              return 0; } 
		  }
               
          else {  // if dy > 246
            if(dz <= 38) {
              return 1; } 
               
            else {  // if dz > 38
              return 0; } 
		  }
		}
	  }
	}
               
    else {  // if dz > 152
      if(xavg <= 232) {
        if(z <= 284) {
          return 1; } 
           
        else {  // if z > 284
          if(z <= 372) {
            return 1; } 
             
          else {  // if z > 372
            return 1; } 
		}
	  }
             
      else {  // if xavg > 232
        return 1; } 
	}
  }
         
  else {  // if xavg > 313
    if(dx <= -274) {
      return 1; } 
       
    else {  // if dx > -274
      if(xavg <= 344) {
        return 0; } 
         
      else {  // if xavg > 344
        if(zavg <= 42) {
          return 1; } 
           
        else {  // if zavg > 42
          return 1; } 
	  }
	}
  }
}
           
/*********** Tree 3 *************/
static bool tree3(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z){
  if(z <= -34) {
    if(xavg <= -1) {
      if(zavg <= -257) {
        return 1; } 
         
      else {  // if zavg > -257
        if(dy <= -136) {
          return 1; } 
           
        else {  // if dy > -136
          if(zavg <= -3) {
            if(xavg <= -26) {
              return 0; } 
               
            else {  // if xavg > -26
              return 0; } 
		  }
               
          else {  // if zavg > -3
            return 1; }
		}
	  }
	}	  
             
    else {  // if xavg > -1
      if(zavg <= -47) {
        return 1; } 
         
      else {  // if zavg > -47
        if(z <= -53) {
          if(dy <= 84) {
            return 1; } 
             
          else {  // if dy > 84
            return 1; }
		}			
             
        else {  // if z > -53
          if(zavg <= -22) {
            return 0; } 
             
          else {  // if zavg > -22
            return 0; } 
		}
	  }
	}
  }
             
  else {  // if z > -34
    if(xavg <= 313) {
      if(dy <= -173) {
        if(dz <= -35) {
          if(dx <= -52) {
            return 1; } 
             
          else {  // if dx > -52
            return 0; } 
		}
             
        else {  // if dz > -35
          if(dz <= 66) {
            return 1; } 
             
          else {  // if dz > 66
            return 1; } 
		}
	  }
             
      else {  // if dy > -173
        if(dz <= 110) {
          if(dz <= -74) {
            if(yavg <= 32) {
              return 1; } 
               
            else {  // if yavg > 32
              return 0; }
		  }			  
               
          else {  // if dz > -74
            if(dy <= 165) {
              return 0; } 
               
            else {  // if dy > 165
              return 0; } 
		  }
		}
               
        else {  // if dz > 110
          if(yavg <= 81) {
            if(dz <= 204) {
              return 1; } 
               
            else {  // if dz > 204
              return 1; } 
		  }
               
          else {  // if yavg > 81
            if(xavg <= 84) {
              return 0; } 
               
            else {  // if xavg > 84
              return 1; } 
		  }
		}
	  }
	}
               
    else {  // if xavg > 313
      if(yavg <= 71) {
        return 1; } 
         
      else {  // if yavg > 71
        if(zavg <= 160) {
          return 0; } 
           
        else {  // if zavg > 160
          return 1; } 
	  }
	}
  }
}
           
/*********** Tree 4 *************/
static bool tree4(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z){
  if(dz <= -149) {
    if(dz <= -240) {
      return 1; } 
       
    else {  // if dz > -240
      if(y <= 99) {
        return 1; } 
         
      else {  // if y > 99
        return 0; } 
	}
  }
         
  else {  // if dz > -149
    if(dz <= 110) {
      if(yavg <= 360) {
        if(dx <= 266) {
          if(xavg <= 368) {
            if(zavg <= -254) {
              return 1; } 
               
            else {  // if zavg > -254
              return 0; } 
		  }
               
          else {  // if xavg > 368
            return 1; } 
		}
             
        else {  // if dx > 266
          if(xavg <= 92) {
            return 1; } 
             
          else {  // if xavg > 92
            return 1; } 
		}
	  }
             
      else {  // if yavg > 360
        return 1; } 
	}
         
    else {  // if dz > 110
      if(dz <= 225) {
        if(zavg <= 27) {
          if(y <= 104) {
            return 1; } 
             
          else {  // if y > 104
            return 1; }
		}			
             
        else {  // if zavg > 27
          if(y <= 32) {
            return 1; } 
             
          else {  // if y > 32
            if(xavg <= 184) {
              return 0; } 
               
            else {  // if xavg > 184
              return 1; } 
		  }
		}
	  }
               
      else {  // if dz > 225
        if(xavg <= 266) {
          return 1; } 
           
        else {  // if xavg > 266
          return 1; } 
	  }
	}
  }
}
           
/*********** Tree 5 *************/
static bool tree5(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z){
  if(x <= 358) {
    if(dx <= -186) {
      if(z <= 2) {
        return 1; } 
         
      else {  // if z > 2
        if(zavg <= 86) {
          if(dz <= 170) {
            return 0; } 
             
          else {  // if dz > 170
            return 1; } 
		}
             
        else {  // if zavg > 86
          if(dx <= -403) {
            return 1; } 
             
          else {  // if dx > -403
            return 1; } 
		}
	  }
	}
             
    else {  // if dx > -186
      if(y <= -180) {
        return 1; } 
         
      else {  // if y > -180
        if(dx <= 160) {
          if(xavg <= 368) {
            if(z <= 358) {
              return 0; } 
               
            else {  // if z > 358
              return 1; } 
		  }
               
          else {  // if xavg > 368
            return 1; } 
		}
             
        else {  // if dx > 160
          if(dz <= 10) {
            if(xavg <= 22) {
              return 1; } 
               
            else {  // if xavg > 22
              return 1; } 
		  }
               
          else {  // if dz > 10
            if(yavg <= 125) {
              return 0; } 
               
            else {  // if yavg > 125
              return 0; } 
		  }
		}
	  }
	}
  }
               
  else {  // if x > 358
    if(dy <= 14) {
      return 1; } 
       
    else {  // if dy > 14
      if(dx <= 227) {
        if(dx <= 20) {
          return 1; } 
           
        else {  // if dx > 20
          return 0; }
	  }		  
           
      else {  // if dx > 227
        return 1; } 
	}
  }
}
         
/*********** Tree 6 *************/
static bool tree6(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z){
  if(dz <= 150) {
    if(dz <= -112) {
      if(z <= 48) {
        if(zavg <= -2) {
          return 1; } 
           
        else {  // if zavg > -2
          if(dz <= -152) {
            if(dy <= -252) {
              return 1; } 
               
            else {  // if dy > -252
              return 1; } 
		  }
               
          else {  // if dz > -152
            return 0; } 
		}
	  }
             
      else {  // if z > 48
        if(xavg <= 197) {
          if(dy <= -15) {
            return 0; } 
             
          else {  // if dy > -15
            return 0; }
		}			
             
        else {  // if xavg > 197
          return 1; } 
	  }
	}
           
    else {  // if dz > -112
      if(x <= 327) {
        if(dy <= -173) {
          if(y <= 30) {
            return 1; } 
             
          else {  // if y > 30
            if(yavg <= 223) {
              return 1; } 
               
            else {  // if yavg > 223
              return 0; } 
		  }
		}
               
        else {  // if dy > -173
          if(dx <= -236) {
            return 1; } 
             
          else {  // if dx > -236
            if(xavg <= 317) {
              return 0; } 
               
            else {  // if xavg > 317
              return 1; } 
		  }
		}
	  }
               
      else {  // if x > 327
        if(y <= 68) {
          return 1; } 
           
        else {  // if y > 68
          if(dy <= 6) {
            return 1; } 
             
          else {  // if dy > 6
            if(zavg <= 36) {
              return 1; } 
               
            else {  // if zavg > 36
              return 0; } 
		  }
		}
	  }
	}
  }
               
  else {  // if dz > 150
    if(yavg <= 91) {
      if(x <= 14) {
        return 1; } 
         
      else {  // if x > 14
        return 1; } 
	}
         
    else {  // if yavg > 91
      if(yavg <= 186) {
        return 0; } 
         
      else {  // if yavg > 186
        return 1; } 
	}
  }
}
         
/*********** Tree 7 *************/
static bool tree7(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z){
  if(dx <= 160) {
    if(dy <= -156) {
      if(y <= -34) {
        if(y <= -161) {
          return 1; } 
           
        else {  // if y > -161
          return 1; } 
	  }
           
      else {  // if y > -34
        if(dz <= -194) {
          return 1; } 
           
        else {  // if dz > -194
          if(yavg <= 190) {
            return 1; } 
             
          else {  // if yavg > 190
            if(zavg <= 42) {
              return 0; } 
               
            else {  // if zavg > 42
              return 0; } 
		  }
		}
	  }
	}
               
    else {  // if dy > -156
      if(yavg <= -111) {
        if(xavg <= 122) {
          if(dx <= 4) {
            return 0; } 
             
          else {  // if dx > 4
            return 1; } 
		}
             
        else {  // if xavg > 122
          return 1; } 
	  }
           
      else {  // if yavg > -111
        if(dx <= -236) {
          if(xavg <= -1) {
            return 1; } 
             
          else {  // if xavg > -1
            return 1; } 
		}
             
        else {  // if dx > -236
          if(xavg <= 452) {
            if(z <= -254) {
              return 1; } 
               
            else {  // if z > -254
              return 0; }
		  }			  
               
          else {  // if xavg > 452
            return 1; } 
		}
	  }
	}
  }
             
  else {  // if dx > 160
    if(yavg <= 86) {
      if(dx <= 212) {
        return 1; } 
         
      else {  // if dx > 212
        return 1; } 
	}
         
    else {  // if yavg > 86
      if(dy <= -149) {
        return 1; } 
         
      else {  // if dy > -149
        if(yavg <= 260) {
          if(xavg <= 22) {
            return 0; } 
             
          else {  // if xavg > 22
            if(zavg <= 111) {
              return 0; } 
               
            else {  // if zavg > 111
              return 0; }
		  }
		}		  
               
        else {  // if yavg > 260
          return 1; } 
	  }
	}
  }
}
           
/*********** Tree 8 *************/
static bool tree8(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z){
  if(dx <= 248) {
    if(xavg <= 317) {
      if(dy <= -156) {
        if(dx <= -164) {
          if(xavg <= 186) {
            return 1; } 
             
          else {  // if xavg > 186
            return 1; } 
		}
             
        else {  // if dx > -164
          if(yavg <= 43) {
            return 1; } 
             
          else {  // if yavg > 43
            if(xavg <= 28) {
              return 1; } 
               
            else {  // if xavg > 28
              return 0; }
		  }
		}
	  }		
               
      else {  // if dy > -156
        if(dx <= -184) {
          if(y <= 195) {
            if(yavg <= 170) {
              return 1; } 
               
            else {  // if yavg > 170
              return 0; } 
		  }
               
          else {  // if y > 195
            if(z <= 48) {
              return 1; } 
               
            else {  // if z > 48
              return 1; } 
		  }
		}
               
        else {  // if dx > -184
          if(z <= -266) {
            return 1; } 
             
          else {  // if z > -266
            if(dz <= 199) {
              return 0; } 
               
            else {  // if dz > 199
              return 1; } 
		  }
		}
	  }
	}
               
    else {  // if xavg > 317
      if(zavg <= 86) {
        return 1; } 
         
      else {  // if zavg > 86
        if(dz <= -52) {
          return 1; } 
           
        else {  // if dz > -52
          return 1; } 
	  }
	}
  }
           
  else {  // if dx > 248
    if(xavg <= 156) {
      if(y <= 8) {
        return 1; } 
         
      else {  // if y > 8
        if(yavg <= 140) {
          return 0; } 
           
        else {  // if yavg > 140
          return 1; }
	  }
	}	  
           
    else {  // if xavg > 156
      return 1; } 
  }
}
       
/*********** Tree 9 *************/
static bool tree9(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z){
  if(dz <= 108) {
    if(dz <= -236) {
      if(y <= 61) {
        return 1; } 
         
      else {  // if y > 61
        if(zavg <= 177) {
          return 1; } 
           
        else {  // if zavg > 177
          return 1; }
	  }
	}	  
           
    else {  // if dz > -236
      if(x <= 326) {
        if(dy <= -116) {
          if(yavg <= 190) {
            if(yavg <= -5) {
              return 1; } 
               
            else {  // if yavg > -5
              return 1; } 
		  }
               
          else {  // if yavg > 190
            if(yavg <= 293) {
              return 0; } 
               
            else {  // if yavg > 293
              return 0; } 
		  }
		}
               
        else {  // if dy > -116
          if(dy <= 96) {
            if(dz <= -74) {
              return 0; } 
               
            else {  // if dz > -74
              return 0; } 
		  }
               
          else {  // if dy > 96
            if(zavg <= 26) {
              return 1; } 
               
            else {  // if zavg > 26
              return 0; } 
		  }
		}
	  }
               
      else {  // if x > 326
        if(z <= 34) {
          if(y <= 92) {
            return 1; } 
             
          else {  // if y > 92
            return 1; } 
		}
             
        else {  // if z > 34
          if(dx <= 140) {
            return 0; } 
             
          else {  // if dx > 140
            return 0; }
		}
	  }
	}
  }	
             
  else {  // if dz > 108
    if(zavg <= 27) {
      return 1; } 
       
    else {  // if zavg > 27
      if(z <= 354) {
        if(xavg <= 293) {
          if(yavg <= 75) {
            if(x <= 124) {
              return 1; } 
               
            else {  // if x > 124
              return 0; }
		  }			  
               
          else {  // if yavg > 75
            if(z <= 193) {
              return 0; } 
               
            else {  // if z > 193
              return 0; } 
		  }
		}
               
        else {  // if xavg > 293
          return 1; } 
	  }
           
      else {  // if z > 354
        return 1; } 
	}
  }
}

//single data point prediction of a fall   
bool RF_predict(int16_t xavg, int16_t yavg, int16_t zavg, int16_t dx, int16_t dy, int16_t dz, int16_t x, int16_t y, int16_t z){
	bool trees[totaltrees] = {0};
	trees[0] = tree0(xavg, yavg, zavg, dx, dy, dz, x, y, z);
	trees[1] = tree1(xavg, yavg, zavg, dx, dy, dz, x, y, z);
	trees[2] = tree2(xavg, yavg, zavg, dx, dy, dz, x, y, z);
	trees[3] = tree3(xavg, yavg, zavg, dx, dy, dz, x, y, z);
	trees[4] = tree4(xavg, yavg, zavg, dx, dy, dz, x, y, z);
	trees[5] = tree5(xavg, yavg, zavg, dx, dy, dz, x, y, z);
	trees[6] = tree6(xavg, yavg, zavg, dx, dy, dz, x, y, z);
	trees[7] = tree7(xavg, yavg, zavg, dx, dy, dz, x, y, z);
	trees[8] = tree8(xavg, yavg, zavg, dx, dy, dz, x, y, z);
	trees[9] = tree9(xavg, yavg, zavg, dx, dy, dz, x, y, z);
	uint8_t average = 0;
	for(int i = 0; i<totaltrees; i++){
		average += trees[i];
	}
	if(average>6)
		return 1;
	else
		return 0;	
}

#endif
  