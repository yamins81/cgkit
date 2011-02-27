/*
   This shader is used to test the slparams module.
 */

#define arg float Kd = 0.5

#pragma annotation "MyData" "MyValue"

/*
 <meta id="Ks_label">Specular Response</meta>
 <meta id="Ks_min">0</meta>
*/

surface testshader(float Ka = 1;
		   varying vector norm = 0;
		   float uv[2] = {1,2};  /* comment */
		   output point out = point "world" (0,0,0); // comment
		   point pnts[3] = {point "object" (1,2,3), point "world" (4,5,6), point "camera" (7,8,9)};
		   uniform color col = color "rgb" (1,1,1);
		   matrix M = 1, T = matrix(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
		   arg;
		   float singleArr[1] = {0.5};
                  )
{
#define TEST_LINE_CONTINUATION { \
}

  Ci = color "rgb" (1,1,1);
  Oi = 1;
}
