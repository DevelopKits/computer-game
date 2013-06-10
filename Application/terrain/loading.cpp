#include "module.h"

#include <string>
#include <mutex>
#include <GLM/glm.hpp>
#include <GLM/gtc/noise.hpp>
#include <SFML/OpenGL.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;
using namespace sf;

#include "terrain.h"
#include "form.h"
#include "model.h"


void ModuleTerrain::Loading()
{
	while(running)
	{
		if(loading && access.try_lock())
		{
			// just remesh updated chunk
			if(current.Changed)
			{
				Mesh(&current);
			}
			
			// load or generate new chunk
			else
			{
				Generate(&current);
				Mesh(&current);
			}

			access.unlock();
			loading = false;
		}
	}
}

#define GRID vec2(1.f / TILES_U, 1.f / TILES_V)
#define GAP float(0.000/*1*/)

void ModuleTerrain::Mesh(Terrain *Terrain)
{
	int n = 0;
	for(int X = 0; X < CHUNK_SIZE.x;  ++X)
	for(int Y = 0; Y < CHUNK_SIZE.y; ++Y)
	for(int Z = 0; Z < CHUNK_SIZE.z;  ++Z)
	{
		if(Terrain->Blocks[X][Y][Z])
		{
			uint8_t tile = clamp((int)Terrain->Blocks[X][Y][Z], 0, TILES_U * TILES_V - 1);
			for(int dim = 0; dim < 3; ++dim) { int dir = -1; do {
				ivec3 neigh = Shift(dim, ivec3(dir, 0, 0)) + ivec3(X, Y, Z);

				if(!(Inside(neigh, ivec3(0), CHUNK_SIZE - 1) && Terrain->Blocks[neigh.x][neigh.y][neigh.z]))
				{
					for(float i = 0; i <= 1; ++i)
					for(float j = 0; j <= 1; ++j)
					{
						vec3 vertex = vec3(X, Y, Z) + vec3(Shift(dim, ivec3((dir+1)/2, i, j)));
						positions.push_back(vertex.x); positions.push_back(vertex.y); positions.push_back(vertex.z);
					}

					vec3 normal = normalize(vec3(Shift(dim, ivec3(dir, 0, 0))));
					for(int i = 0; i < 4; ++i)
					{
						normals.push_back(normal.x); normals.push_back(normal.y); normals.push_back(normal.z);
					}

					vec2 coords(tile % TILES_U, tile / TILES_U);
					vec2 position = coords * GRID;
					texcoords.push_back(position.x          + GAP); texcoords.push_back(position.y          + GAP);
					texcoords.push_back(position.x + GRID.x - GAP); texcoords.push_back(position.y          + GAP);
					texcoords.push_back(position.x          + GAP); texcoords.push_back(position.y + GRID.y - GAP);
					texcoords.push_back(position.x + GRID.x - GAP); texcoords.push_back(position.y + GRID.y - GAP);

					if(dir == -1) {
						elements.push_back(n+0); elements.push_back(n+1); elements.push_back(n+2);
						elements.push_back(n+1); elements.push_back(n+3); elements.push_back(n+2);
					} else {
						elements.push_back(n+0); elements.push_back(n+2); elements.push_back(n+1);
						elements.push_back(n+1); elements.push_back(n+2); elements.push_back(n+3);
					}
					n += 4;
				}

			dir *= -1; } while(dir > 0); }
		}
	}

	GLfloat coords[9];
	for(unsigned int i = 0; i < elements.size(); i += 3)
	{
		for(int n = 0, j = 0; j < 3; ++j)
			for(int k = 0; k < 3; ++k, ++n)
				coords[n] = positions[3 * elements[i + j] + k];

		triangles.push_back(
			tuple<btVector3, btVector3, btVector3>(
				btVector3(coords[0], coords[1], coords[2]),
				btVector3(coords[3], coords[4], coords[5]),
				btVector3(coords[6], coords[7], coords[8])
			)
		);
	}
}

bool ModuleTerrain::Inside(ivec3 Position, ivec3 Min, ivec3 Max)
{
	if(Position.x < Min.x || Position.y < Min.y || Position.z < Min.z) return false;
	if(Position.x > Max.x || Position.y > Max.y || Position.z > Max.z) return false;
	return true;
}

ivec3 ModuleTerrain::Shift(int Dimension, ivec3 Vector)
{
	if      (Dimension % 3 == 1) return ivec3(Vector.z, Vector.x, Vector.y);
	else if (Dimension % 3 == 2) return ivec3(Vector.y, Vector.z, Vector.x);
	else                         return Vector;
}

void ModuleTerrain::Buffer(unsigned int Id)
{
	auto mdl = Entity->Get<Model>(Id);
	auto frm = Entity->Get<Form>(Id);

	if(!mdl->Positions) glGenBuffers(1, &mdl->Positions);
	glBindBuffer(GL_ARRAY_BUFFER, mdl->Positions);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof GLfloat, &positions[0], GL_STATIC_DRAW);
	if(!mdl->Normals  ) glGenBuffers(1, &mdl->Normals  );
	glBindBuffer(GL_ARRAY_BUFFER, mdl->Normals);
	glBufferData(GL_ARRAY_BUFFER, normals.size()   * sizeof GLfloat, &normals[0],   GL_STATIC_DRAW);
	if(!mdl->Texcoords) glGenBuffers(1, &mdl->Texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, mdl->Texcoords);
	glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof GLfloat, &texcoords[0], GL_STATIC_DRAW);
	if(!mdl->Elements ) glGenBuffers(1, &mdl->Elements );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->Elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof GLuint, &elements[0], GL_STATIC_DRAW);

	positions.clear(); normals.clear(); texcoords.clear(); elements.clear();
	
	btTriangleMesh *mesh = new btTriangleMesh();
	for(auto i = triangles.begin(); i != triangles.end(); ++i)
		mesh->addTriangle(get<0>(*i), get<1>(*i), get<2>(*i));
	triangles.clear();

	btCollisionShape *oldshape = frm->Body->getCollisionShape();
	frm->Body->setCollisionShape(new btBvhTriangleMeshShape(mesh, true, true));
	delete oldshape;

	null = true;
}
