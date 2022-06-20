/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLfloat theta[3] = { 0, 0, 0 };
GLfloat dr = 5;

mat4 model;
GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}
GLuint loc_vColor;
void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	 loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(0.0, 0.0, 1.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}


mat4 tg;
// bắt đầu phần đối tượng máy bay  của đông//

float k = 0.0f;
mat4 tgk = 1;

int d = 0;
void sprin(int n) {
	d = d + n;
	tg = RotateY(d);
	if (d > 360)d = -360;
	glutPostRedisplay();
}
void tienLenD(float d2) {
	k = k + d2;
	d = d + 5;
	tgk = Translate(k, 0, 0);
	if (k > 2.0) k = 2.0;
		glutPostRedisplay();
}
int D_co = 0;
int D_qy = 0;
mat4 tgqd = 1;
void D_quaytudong(int) {
	D_qy += 8;
	if (D_co == 0) {
		glutPostRedisplay();
		glutTimerFunc(1000 / 60, D_quaytudong, 0);
	}

}
void D_BayLen(float d2) {
	k = k + d2;
	tgk = Translate(0, k, 0);
	if (k > 2.0) k = 2.0;
	glutPostRedisplay();

}
void thanMay() {
	model = tgk * tg * Translate(-1.8, 0, 0) * Scale(0.8, 0.4, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void canhMay() {
	model = tgk * tg * Translate(-1.7, -0.05, 0) * Scale(0.3, 0.1, 1.0);
	color4 light_diffuse(0.5, 0, 0.5, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glVertexAttribPointer(loc_vColor,1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void duoiMay() {
	model = tgk * tg * Translate(-2.185, 0.30, 0) * Scale(0.1, 0.5, 0.05);
	color4 light_diffuse(0, 0.5, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glVertexAttribPointer(loc_vColor,1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void duoiMayDuoi() {
	model = tgk * tg * Translate(-2.2, 0.15, 0) * Scale(0.2, 0.1, 0.2);
	color4 light_diffuse(0.5, 0, 0.5, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glVertexAttribPointer(loc_vColor,1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void canhQuat() {
	
	model = tgk * tg * Translate(-1.5, 0, 0) * Scale(0.4, 0.1, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void canh1() {
	tgqd = RotateX(D_qy);
	model = tgk* tgqd* tg * RotateX(30) * Translate(-1.3, 0, 0) * Scale(0.05, 0.1, 0.4);
	color4 light_diffuse(0.5, 0, 0.5, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void canh2() {
	tgqd = RotateX(D_qy);
	model = tgk * tgqd*  tg * RotateX(30) * Translate(-1.3, 0, 0) * Scale(0.05, 0.4, 0.05);
	color4 light_diffuse(0.5, 0, 0.5, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void planeOfDong() {
	thanMay();
	canhMay();
	canhQuat();
	canh2();
	canh1();
	duoiMay();
	duoiMayDuoi();
}

// kết thúc phần đối tượng máy bay  của đông//
// 
// 
//Phan cua chính ( máy bay trực thăng)
mat4 c_cmt = 1;
mat4 c_tg = 1;
void c_dauMB() {
	c_cmt = c_tg *Translate(0,0,0)*Scale(0.5,0.5,0.5);
	color4 light_diffuse(0, 0.5, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, c_cmt);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void c_thanMB() {
	c_cmt = c_tg * Translate(0, 0, -0.45) * Scale(0.2, 0.2, 1);
	color4 light_diffuse(0.5, 0.5, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, c_cmt);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
}
void c_trucduoiMB() {
	c_cmt = c_tg * Translate(0, 0.1, -0.9) * Scale(0.1, 0.3, 0.1);
	color4 light_diffuse(0, 0.5, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, c_cmt);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void c_duoiMB() {
	c_trucduoiMB();
	c_cmt = c_tg * Translate(0, 0.25, -0.9) * Scale(0.5, 0.05, 0.1);
	color4 light_diffuse(0, 0.5, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, c_cmt);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void c_trucquatMB() {
	c_cmt = c_tg * Translate(0, 0.2, 0) * Scale(0.1, 0.5, 0.1);
	color4 light_diffuse(0.5, 0, 0.5, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, c_cmt);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
int qy = 0;

void c_canhquatMB(float ca, float cb, float cc,int cd) {
	mat4 c_tgq = RotateY(qy);
	c_cmt = c_tg*c_tgq*RotateY(cd) * Translate(ca, cb, cc) * Scale(0.5, 0.02, 0.1);
	color4 light_diffuse(0.5, 0, 0.5, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, c_cmt);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
int co = 0;
int bay = 0;
float by = 0;
float bz = 0;
void quaytudong(int) {
	qy += 8;
	if (co == 0) {
		glutPostRedisplay();
		glutTimerFunc(1000 / 60, quaytudong, 0);
	}
	
}
void baytudong(int) {
	if (bay >= 3&&by<0.4) {
		by += 0.05;
		c_tg =Translate(0, by * by + by, by)*c_tg ;
		glutPostRedisplay();
		glutTimerFunc(1000/2 , baytudong, 0);
	}
	if (by > 0.39) {
			c_tg =c_tg*RotateY(180) ;
				if (bz < 0.4) {
					bz += 0.02;
					c_tg = Translate(0, 0, -bz) * c_tg;
					glutPostRedisplay();
					glutTimerFunc(1000 / 2, baytudong, 0);
				}
	}
	if (bz > 0.39) {
		c_tg = Translate(0,0,0);
	}
}
void c_trucchanMB(float ca, float cb, float cc) {
	c_cmt = c_tg * Translate(ca, cb, cc) * Scale(0.05, 0.3, 0.05);
	color4 light_diffuse(0.2, 0.2, 0.5, 1);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, c_cmt);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void c_quatMB() {
	c_canhquatMB(0.25, 0.45, 0,10);
	c_canhquatMB(0.25, 0.45, 0, 130);
	c_canhquatMB(0.25, 0.45, 0, 250);
}
void c_chan(float ca, float cb, float cc) {
	c_cmt = c_tg  * Translate(ca, cb, cc) * Scale(0.1, 0.02, 0.5);
	color4 light_diffuse(0.5, 0, 0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, c_cmt);
	glVertexAttribPointer(loc_vColor, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
void c_chanMB() {
	c_trucchanMB(0.15, -0.35, 0);
	c_trucchanMB(-0.15, -0.35, 0);
	c_chan(0.15, -0.5, 0);
	c_chan(-0.15, -0.5, 0);
}

void maybayTT() {
	c_thanMB();
	c_duoiMB();
	c_trucquatMB();
	c_quatMB();
	c_chanMB();
	c_dauMB();
}


//het phan model cua chinh
float ax = 0, ay = 0, az = 2;
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const vec3 viewer_pos(0.0, 0.0, 2.0);  /*Trùng với eye của camera*/
	maybayTT();
	planeOfDong();
	//ca me ra
	vec4 eye(ax, ay, az, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 1, 0, 1);
	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);
	projection = Frustum(-2, 2, -2, 2, 1, 5);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case 'c':
		sprin(5);
		glutPostRedisplay();
		break;
	case 'X':
		tienLenD(0.1);
		D_quaytudong(0);
		glutPostRedisplay();
		break;
	case 'y':
		tienLenD(-0.1);
		D_quaytudong(0);
		glutPostRedisplay();
		break;
	case 'Y':
		D_co = 0;
		D_quaytudong(0);
		glutPostRedisplay();
		break;
	case 'z':
		D_co = 1;
		glutPostRedisplay();
		break;
	case 'Z':
		D_co = 0;
		D_BayLen(0.1);
		D_quaytudong(0);
		glutPostRedisplay();
		break;
	case 'v':
		D_co = 0;
		D_BayLen(-0.1);
		D_quaytudong(0);
		glutPostRedisplay();
		break;
		// chinh them
	case '1':
		ax += 0.2;
		glutPostRedisplay();
		break;
	case '2':
		ax -= 0.2;
		glutPostRedisplay();
		break;
	case '3':
		ay += 0.2;
		glutPostRedisplay();
		break;
	case '4':
		ay -= 0.2;
		glutPostRedisplay();
		break;
	case '5':
		az += 0.2;
		glutPostRedisplay();
		break;
	case '6':
		az -= 0.2;
		glutPostRedisplay();
		break;
	case 'q':
		co = 0;
		bay++;
		quaytudong(0);
		baytudong(0);
		break;
	case 'b':
		baytudong(0);
		break;
	case 'Q':
		co = 1;
		glutPostRedisplay();
		break;
	case 'r':
		ax = 1;
		az = 0;
		glutPostRedisplay();
		break;

	// het cua chinh
	}
	



}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("A Cube is rotated by keyboard and shaded");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
