model('PrimitiveQube', 'magic.mtl', 20, 5, -6, 27, 31, 57, 1, false);
light(20, 6, -4.5, 25, 1.0, 0.6, 0.2, 1.0);

var a = 3;
for(x = -3; x <= 3; ++x)
for(z = -3; z <= 3; ++z)
	model("PrimitiveQube", "dirt.mtl", a*x, 1, a*z, 0, 0, 0, 1, true);

for(x = -3; x <= 3; x+=3)
for(z = -3; z <= 3; z+=3)
	if(x == 0 && z == 0) continue;
	else model("PrimitiveQube", "grass.mtl", a*x, a+1, a*z, 0, 0, 0, 1, true);

model("PrimitivePlane", "chess.mtl", 0, 0, 0, 0, 0, 0, 1, true);

model("barrel.3ds", "barrel.mtl",    16, 0,  8,   -90,  0,  0,    4.0,    true);
model("shrine.3ds", "shrine.mtl",    40, 0,-10,   -90,  0,-30,    2.5,    true);
model("rock.obj",   "rock.mtl",      50, 2, 45,   -90,  0,  0,    5.0,    true);

light(-10, 15,-15,    100,    1.0, 1.0, 1.0,    1.0); // white
light(-10, 15, 15,     80,    0.2, 1.0, 0.2,    2.0); // green
light( 40, 10,-10,    100,    1.0, 1.0, 1.0,    2.5); // white
light( 50, 20, 50,    120,    0.0, 0.4, 1.0,    2.0); // blue
light( 40,  8, 58,     40,    1.0, 0.0, 0.0,    5.0); // red
//light(0, 50, 50, 500, 1, 1, 1, 1); // faked daylight