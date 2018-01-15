#ifndef __GWU_MODEL__
#define __GWU_MODEL__
#define N 999
#define window_width 600
#define window_height 600

//================================
// Illumination
//================================
class Illumination {
public:
	vec3 Ia; //ambient intensity
	float ka; //ambient reflectivity
	float fatt;  //further mode
	vec3 Ip;  //position intensity
	float kd;  //diffuse reflectivity
	float ks;  //specular reflectivity
	vec3 Os;  //specular color
	int n;  
	vec3 Li1;  //light dirction 1
	vec3 Li2;  //light dirction 2
	vec3 Vi;   //camera dirction
	vec3 Hi1;  //half angle between light direction and camera dirction 1
	vec3 Hi2;  //half angle between light direction and camera dirction 2

public:
	Illumination() {
		Ia = vec3(1.0f, 1.0f, 1.0f);
		ka = 0;
		fatt = 1;
		Ip = vec3(1.0f, 1.0f, 1.0f);
		kd = 0.5;
		ks = 1;
		Os = vec3(1.0f, 1.0f, 1.0f);
		n = 8;
		Li1 = vec3(-2.0f, -1.0f, -1.0f);
		Li2 = vec3(-1.0f, -0.5f, -1.0f);
		Vi = vec3(0.0f, 1.0f, 0.0f);

		Li1 = Li1.normalize();
		Li2 = Li2.normalize();
		Hi1 = (Li1 + Vi).normalize();
		Hi2 = (Li2 + Vi).normalize();
	}

	~Illumination() {

	}

	//culculate illumination by color and normal
	vec3 getIllumination(vec3 Od, vec3 Ni) {
		vec3 ambient = Od.multi(Ia) * ka;
		vec3 reflect1 = Ip.multi((Od * (Ni.dot(Li1) * kd) + Os * pow((Ni.dot(Hi1)), n) * ks));
		vec3 reflect2 = Ip.multi((Od * (Ni.dot(Li2) * kd) + Os * pow((Ni.dot(Hi2)), n) * ks)); 
		
		vec3 I = ambient + reflect1 * fatt + reflect2 * fatt;

		return I;
	}
};

//================================
// Point
//================================
class Point {
public:
	int ymax;
	float xmin;
	float xy;

	float z;
	float zy;

	vec3 pointNor;  //point normal
	vec3 pointColor;  //point color 
	vec3 Nexcolor;  //upper point color 
	vec3 Nexnormal;  //upper point normal
	vec3 Lastcolor;  //lower point color 
	vec3 Lastnormal;  //lower point normal

	float Nexy;
	float Lasty;

public:
	Point() {
	}

	~Point() {

	}
};

//================================
// ZBuffer
//================================
class ZBuffer {
public:
	std::vector< vec3 > zb;

public:
	ZBuffer() {
		zb.resize(window_width * window_height);
		for(int i = 0; i < window_height; i++){
			for (int j = 0; j < window_width; j++) {
				int num = i * window_width + j;

				//set 10000 as background
				zb[num].z = 10000;
			}

		}
	}

	~ZBuffer() {

	}

};

//================================
// ModelFace
//================================
class ModelFace {
public:
	std::vector< int > indices;
	std::vector<std::list<Point> > Edges;
	vec3 faceNormal;

public:
	ModelFace() {
	}

	~ModelFace() {
	}
};

//================================
// Model
//================================
class Model {
public:
	std::vector< vec3 > verts;
	std::vector< ModelFace > faces;
	std::vector< vec3 > pointNormals;
	std::vector< vec3 > pointColors;

	//output debug
	std::ofstream outFile;

public:
	Model() {
	}

	~Model() {
	}

	//=============================================
	// Load Model
	//=============================================
	void Free(void) {
		verts.clear();
		faces.clear();
	}

	bool LoadModel(const char *path) {
		if (!path) {
			return false;
		}

		Free();

		// open file
		FILE *fp = fopen(path, "r");
		if (!fp) {
			return false;
		}

		unsigned int numVerts = 0;
		unsigned int numFaces = 0;
		// num of vertices and indices
		fscanf(fp, "data%d%d", &numVerts, &numFaces);

		// alloc vertex and index buffer
		verts.resize(numVerts);
		faces.resize(numFaces);
		pointNormals.resize(numVerts);
		pointColors.resize(numVerts);

		// read vertices
		for (unsigned int i = 0; i < numVerts; i++) {
			fscanf(fp, "%f%f%f", &verts[i].x, &verts[i].y, &verts[i].z);
		}

		// read indices
		for (unsigned int i = 0; i < numFaces; i++) {
			int numSides = 0;
			fscanf(fp, "%i", &numSides);
			faces[i].indices.resize(numSides);

			for (unsigned int k = 0; k < faces[i].indices.size(); k++) {
				fscanf(fp, "%i", &faces[i].indices[k]);
			}
		}

		// close file
		fclose(fp);

		//Translation();

		//ResizeModel();

		outFile.open("debug.txt", std::ios::out);

		return true;
	}

	//get vertics' normals and colors
	void SetNormals(Illumination ill) {
		for (unsigned int i = 0; i < verts.size(); i++) {
			vec3 EmptyNor;

			for (unsigned int j = 0; j < faces.size(); j++) {
				for (unsigned int k = 0; k < faces[j].indices.size(); k++) {
					if (faces[j].indices[k] - 1 == i) {

						vec3 ver3New0 = verts[(faces[j].indices[0]) - 1];
						vec3 ver3New1 = verts[(faces[j].indices[1]) - 1];
						vec3 ver3New2 = verts[(faces[j].indices[2]) - 1];
						vec3 vertCross1 = ver3New1 - ver3New0;
						vec3 vertCross2 = ver3New2 - ver3New1;
						vec3 cross = vertCross1.cross(vertCross2);

						EmptyNor = EmptyNor + cross.normalize();
					}
				}
			}

			pointNormals[i] = EmptyNor.normalize();
			pointColors[i] = ill.getIllumination(vec3(0.05f, 0.5f, 0.05f), pointNormals[i]);

		}
	}

	//=============================================
	// Render Model
	//=============================================
	void DrawEdges() {

		for (unsigned int i = 0; i < faces.size(); i++) {
			glBegin(GL_LINES);
			//judge the polygon back or face to the camera
			bool judge = false;

			//seems some faces just have 2 points or less?
			if (faces[i].indices.size() < 3) {
				judge = true;
			}
			else{
				vec3 ver3New0 = verts[(faces[i].indices[0]) - 1];
				vec3 ver3New1 = verts[(faces[i].indices[1]) - 1];
				vec3 ver3New2 = verts[(faces[i].indices[2]) - 1];

				vec3 vertCross1 = ver3New1 - ver3New0;
				vec3 vertCross2 = ver3New2 - ver3New1;
				vec3 normal = vertCross1.cross(vertCross2);
				float result = normal.x * ver3New0.x + normal.y * ver3New0.y + normal.z * ver3New0.z;
				if (result <= 0.0f) judge = true;
			}

			//only draw the front faces
			if (judge == true) {
				
				for (unsigned int k = 0; k < faces[i].indices.size(); k++) {

					int p0 = faces[i].indices[k];
					int p1 = faces[i].indices[(k + 1) % faces[i].indices.size()];

					printf("x=");
					fflush(stdout);
					printf("%6.2f\n", verts[p0 - 1].x);
					fflush(stdout);
					printf("y=");
					fflush(stdout);
					printf("%6.2f\n", verts[p0 - 1].y);
					fflush(stdout);
					printf("z=");
					fflush(stdout);
					printf("%6.2f\n", verts[p0 - 1].z);

					glVertex2f(verts[p0 - 1].x, verts[p0 - 1].y);
					glVertex2f(verts[p1 - 1].x, verts[p1 - 1].y);
					
				}
				
			}
			glEnd();
			
		}
		
	}

	//fill color in polygons
	void fillColor(std::vector< vec3 >& vecZB, Illumination& ill, int shading) {

		for (unsigned int i = 0; i < faces.size(); i++) {
			bool judge = false;

			//seems some faces just have 2 points or less?
			if (faces[i].indices.size() < 3) {
				judge = true;
			}
			else {
				vec3 ver3New0 = verts[(faces[i].indices[0]) - 1];
				vec3 ver3New1 = verts[(faces[i].indices[1]) - 1];
				vec3 ver3New2 = verts[(faces[i].indices[2]) - 1];

				vec3 vertCross1 = ver3New1 - ver3New0;
				vec3 vertCross2 = ver3New2 - ver3New1;
				vec3 normal = vertCross1.cross(vertCross2);
				faces[i].faceNormal = normal.normalize();
				float result = normal.x * ver3New0.x + normal.y * ver3New0.y + normal.z * ver3New0.z;
				if (result <= 0.0f)
					judge = true;
			}

			//only draw the front faces
			if (judge == true) {
				Point emptyPoint;
				std::list<Point> newList;
				std::list<Point> ATEList;

				//build list length = windows height  
				newList.push_front(emptyPoint);
				for (int j = 0; j <= window_height; j++) {
					faces[i].Edges.push_back(newList);
				}

				//build ET
				buildTable(faces[i],ill);
				
				//scan and draw lines
				for (int scanLine = 0; scanLine <= window_height; scanLine++) {
					buildAET(ATEList, faces[i].Edges[scanLine]);
					if (!ATEList.empty()) {

						//select shading mode
						if(shading == 0)
							Constant(scanLine, ATEList, vecZB,ill, i);
						else if (shading == 1)
							Gouraud(scanLine, ATEList, vecZB);
						else if (shading == 2)
							Phong(scanLine, ATEList, vecZB, ill);

						updateAET(scanLine, ATEList);
						resortAET(ATEList);
					}
					
				}

				//clear lists
				faces[i].Edges.clear();

			}
			
		}

	}

	//======================================
	//Translations
	//======================================
	//model parameters
	void ModelTranslation(vec3 position) {
		for (unsigned int i = 0; i < verts.size(); i++) {
			vec3 vp = verts[i].ModelMatrix(position);

			//verts[i].set(screenX, screenY, screenZ);
			verts[i].set(vp.x, vp.y, vp.z);
		}
	}

	//view parameters
	void ViewTranslation(vec3 Pref, vec3 C, vec3 V) {
		for (unsigned int i = 0; i < verts.size(); i++) {
			//n u v for ViewMatrix
			vec3 n = (Pref - C).normalize();
			vec3 u1 = n.cross(V);
			vec3 u = u1.normalize();
			vec3 v = u.cross(n);

			vec3 vp = verts[i].ViewMatrix(u, v, n, C);

			verts[i].set(vp.x, vp.y , vp.z);
		}
	}

	//project parameters
	void PerspectiveTranslation(float d, float f, float h) {
		for (unsigned int i = 0; i < verts.size(); i++) {
			vec3 vp = verts[i].PerspectiveMatrix(d, f, h);

			verts[i].set(vp.x * window_height/2 + window_height/2,
				vp.y * window_height/2 + window_height/2,
				vp.z * window_height/2 + window_height/2);
		}
	}

	//======================================
	//build ET 
	//======================================
	//insert a point in ET
	void insertEdge(std::list<Point>& edgeslist, const Point& p) {
		std::list<Point>::iterator itera = edgeslist.begin();
		std::list<Point>::iterator stop = edgeslist.end();

		while ((itera != stop) && ((*itera).xmin < p.xmin))
			itera++;

		edgeslist.insert(itera, p);
	}

	//judge next y
	int nextY(unsigned int current, std::vector<int> v) {
		unsigned int nextPoint;

		if ((current + 1) > (v.size() - 1))
			nextPoint = 0;
		else
			nextPoint = current + 1;

		while (verts[v[current]-1].y == verts[v[nextPoint] - 1].y) {
			if ((nextPoint + 1) > (v.size() - 1))
				nextPoint = 0;
			else
				nextPoint++;
		}

		return (verts[v[nextPoint] - 1].y);
	}

	//push a point to ET
	void pushInTable(vec3 lower, vec3  upper, int prey, ModelFace& poly, int i, int j, Illumination& ill) {
		Point n;

		n.xy = (float)(upper.x - lower.x) / (upper.y - lower.y);
		n.xmin = lower.x;
		n.zy = (float)(upper.z - lower.z) / (upper.y - lower.y);
		n.z = lower.z;

		n.pointNor = pointNormals[i];
		n.Nexnormal = pointNormals[j];
		n.Lastnormal = pointNormals[i];

		n.pointColor = pointColors[i];
		n.Nexcolor = pointColors[j];
		n.Lastcolor = pointColors[i];

		n.Nexy = upper.y;
		n.Lasty = lower.y;


		if (upper.y < prey)
			n.ymax = upper.y - 1;
		else
			n.ymax = upper.y;

		insertEdge(poly.Edges[lower.y], n);
	}

	//build ET
	void buildTable(ModelFace& poly, Illumination& ill) {
		vec3 v1, v2;
		int pre_y;

		int num1 = poly.indices.size() - 1;
		int num2 = poly.indices.size() - 2;
		pre_y = verts[poly.indices[num2] - 1].y;
		v1 = verts[poly.indices[num1] - 1];
		int q = poly.indices[num1];

		for (unsigned int i = 0; i < poly.indices.size(); i++) {
			int p = poly.indices[i];
			
			v2 = verts[p - 1];

			if (v1.y != v2.y){
				if (v1.y < v2.y) {
					pushInTable(v1, v2, nextY(i, poly.indices), poly, q - 1, p - 1,ill);
				}
				else
					pushInTable(v2, v1, pre_y, poly, p - 1, q - 1,ill);
			}
			pre_y = v1.y;
			v1 = v2;
			q = p;
		}
	}

	//======================================
	//build AET 
	//======================================
	//put ET to AET
	void buildAET(std::list<Point> &AET, std::list<Point> ET){
		std::list<Point>::iterator itera;
		itera = ET.begin();
		itera++;

		while (itera != ET.end()){
			insertEdge(AET, *itera);
			itera++;
		}
	}

	//build next two AET points according to ET
	void updateAET(int y, std::list<Point>& AET) {
		std::list<Point>::iterator itera = AET.begin();

		while (itera != AET.end()) {
			if (y >= (*itera).ymax)
				AET.erase(itera++);
			else {
				(*itera).xmin += (*itera).xy;
				(*itera).z += (*itera).zy;
				(*itera).pointNor = (*itera).Nexnormal * (y - (*itera).Lasty) / ((*itera).Nexy - (*itera).Lasty) +
					(*itera).Lastnormal * ((*itera).Nexy - y) / ((*itera).Nexy - (*itera).Lasty);
				(*itera).pointColor = (*itera).Nexcolor * (y - (*itera).Lasty) / ((*itera).Nexy - (*itera).Lasty) +
					(*itera).Lastcolor * ((*itera).Nexy - y) / ((*itera).Nexy - (*itera).Lasty);

				itera++;
			}
		}
	}

	//not a problem with single polygons
	void resortAET(std::list<Point>& AET){
		std::list<Point> tmp;
		std::list<Point>::iterator itera = AET.begin();
		
		while (itera != AET.end()){
			insertEdge(tmp, *itera);
			AET.erase(itera++);
		}
		AET = tmp;
	}

	//Constant Shading
	void Constant(int y, std::list<Point> AET, std::vector< vec3 >& vecZB, Illumination& ill, int i){
		std::list<Point>::iterator itera1 = AET.begin();
		int x1, x2, z1 ,r1,g1,b1;
		int count = 0;
		vec3 pointNor1;

		//draw a line 2 by 2 in AET
		while (itera1 != AET.end()) {
			count = count + 1;
			if (count % 2 == 0) {
				x2 = (int)(*itera1).xmin;
                
				glBegin(GL_POINTS);

				vec3 color = vec3(0.05f, 0.5f, 0.05f);

				//constant shading
				vec3 truecolor = ill.getIllumination(color, faces[i].faceNormal);
				glColor3f(truecolor.x, truecolor.y, truecolor.z);

				/*outFile << " I: " << faces[i].faceNormal.z << std::endl;
				outFile << " TR: " << truecolor.x << std::endl;
				outFile << " TG: " << truecolor.y << std::endl;
				outFile << " TB: " << truecolor.z << std::endl;*/

				for (int i = x1; i < x2; i++) {

					int zNum = (int)(*itera1).z + ((int)(*itera1).z - z1) / ((int)(*itera1).xmin - x1) * (i - x1);
					//outFile << " Z Buffer: " << zNum << std::endl;

					//judge z buffer
					int zbZ = vecZB[y * window_width + i - 1].z;

					if(zbZ >= zNum){
						vecZB[y * window_width + i - 1].z = zNum;

						glVertex2i(i, y);
					}
				}
				glEnd();

			}
			else {
				x1 = (int)(*itera1).xmin;
				z1 = (int)(*itera1).z;
			}
				
			itera1++;
		}

	}

	//Gouraud Shading
	void Gouraud(int y, std::list<Point> AET, std::vector< vec3 >& vecZB) {
		std::list<Point>::iterator itera1 = AET.begin();
		int x1, x2, z1;
		vec3 pColor1;
		int count = 0;

		//draw a line 2 by 2 in AET
		while (itera1 != AET.end()) {
			count = count + 1;
			if (count % 2 == 0) {
				x2 = (int)(*itera1).xmin;

				glBegin(GL_POINTS);

				for (int i = x1; i < x2; i++) {

					int zNum = z1 * ((*itera1).xmin - i) / ((*itera1).xmin - x1) + (int)(*itera1).z * (i - x1) / ((*itera1).xmin - x1); 
					//outFile << " Z Buffer: " << zNum << std::endl;

					//judge z buffer
					int zbZ = vecZB[y * window_width + i - 1].z;

					if (zbZ >= zNum) {
						vecZB[y * window_width + i - 1].z = zNum;

						//Gouraud shading
						vec3 truecolor = pColor1 * ((*itera1).xmin - i) /  ((*itera1).xmin - x1) + (*itera1).pointColor * (i - x1) / ((*itera1).xmin - x1);

						glColor3f(truecolor.x, truecolor.y, truecolor.z);

						glVertex2i(i, y);
					}
				}
				glEnd();

			}
			else {
				x1 = (int)(*itera1).xmin;
				z1 = (int)(*itera1).z;
				pColor1 = (*itera1).pointColor;

			}

			itera1++;
		}

	}

	//Phong Shading
	void Phong(int y, std::list<Point> AET, std::vector< vec3 >& vecZB, Illumination& ill) {
		std::list<Point>::iterator itera1 = AET.begin();
		int x1, x2, z1;
		int count = 0;
		vec3 pointNor1;

		//draw a line 2 by 2 in AET
		while (itera1 != AET.end()) {
			count = count + 1;
			if (count % 2 == 0) {
				x2 = (int)(*itera1).xmin;

				glBegin(GL_POINTS);
				//vec3 color = vec3(0.1f / faces.size() * (i + 1), 1.0f / faces.size() * (i + 1), 0.1f / faces.size() * (i + 1));
				vec3 color = vec3(0.05f, 0.5f, 0.05f);

				for (int i = x1; i < x2; i++) {

					int zNum = (int)(*itera1).z + ((int)(*itera1).z - z1) / ((int)(*itera1).xmin - x1) * (i - x1);
					//outFile << " Z Buffer: " << zNum << std::endl;

					//judge z buffer
					int zbZ = vecZB[y * window_width + i - 1].z;

					if (zbZ >= zNum) {
						vecZB[y * window_width + i - 1].z = zNum;

						//phong shading
						vec3 truenormal;
						truenormal = pointNor1 + ((*itera1).pointNor - pointNor1) / ((int)(*itera1).xmin - x1) * (i - x1);
						vec3 truecolor = ill.getIllumination(color, truenormal.normalize());
						glColor3f(truecolor.x, truecolor.y, truecolor.z);

						glVertex2i(i, y);
					}
				}
				glEnd();

			}
			else {
				x1 = (int)(*itera1).xmin;
				z1 = (int)(*itera1).z;
				pointNor1 = (*itera1).pointNor;

			}

			itera1++;
		}

	}

};

#endif // __GWU_MODEL__