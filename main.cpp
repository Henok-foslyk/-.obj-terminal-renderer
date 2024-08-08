#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#define STR(foo) #foo
#define XSTR(foo) STR(foo)
#define ASSERT(b) do { if (!(b)) { \
    printf("ASSERT("); \
    printf(STR(b)); \
    printf("); <- "); \
    printf("Line %d in %s\n", __LINE__, __FILE__); \
    *((volatile int *) 0) = 0; \
} } while (0)

void segvHandler(int) {
	printf("Segmentation Fault\n");
	exit(EXIT_FAILURE);
}

struct Vec3 { double x, y, z; };

// Global 
const int ROWS = 80;
const int COLS = 80;
const Vec3 lightsource = {1.0, 0.0, -0.0};
char arr[ROWS][COLS];
double z_buffer[ROWS][COLS];

double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

void drawline(Vec3 v0, Vec3 v1, char c) {
	#undef max
	double temp = std::max(abs(v1.x - v0.x), abs(v1.y - v0.y));
    double number_of_points = temp ==  0 ? 1 : temp;
    for (int i = 0; i <= number_of_points; ++i) {
        double t = i/number_of_points;
        int x = lerp(v0.x, v1.x, t);
        int y = lerp(v0.y, v1.y, t);
        int z = lerp(v0.z, v1.z, t);
        if  ( !( x > ROWS || y > COLS || x < 0 || y < 0) 
        	  && z_buffer[y][x] <= z) {
        	  arr[y][x] = c;
              z_buffer[y][x] = z;
        }
    }
}

void drawtriangle(Vec3 v0, Vec3 v1, Vec3 v2) {
	Vec3 a = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z}; //v1- v0
	Vec3 b = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z}; //v1- v0
	Vec3 normal = { a.y * b.z - a.z * b.y,
                    a.z * b.x - a.z * b.z,
                    a.z * b.y - a.y * b.x};
    double normalizer = sqrt(pow((normal.x), 2) + 
    	            	     pow((normal.y), 2) + 
    	                     pow((normal.z), 2));
    Vec3 N = { normal.x/normalizer, normal.y/normalizer, normal.z/normalizer};
    
    double dot_product =  N.x * lightsource.x + N.y * lightsource.y + N.z * lightsource.z;
    char lights[] = ".,oasxH@";
    //char lights[] = ".,|/ODX@";

    char char_light = ' ';
    if (dot_product < 0.2) { char_light = lights[0];}
    else if (dot_product < 0.4) { char_light = lights[1];}
    else if (dot_product < 0.5) { char_light = lights[2];}
    else if (dot_product < 0.6) { char_light = lights[3];}
    else if (dot_product < 0.7) { char_light = lights[4];}
    else if (dot_product < 0.8) { char_light = lights[5];}
    else if (dot_product < 0.9) { char_light = lights[6];}
    else if (dot_product < 2) { char_light = lights[7];}

	drawline(v0, v1, char_light);
	drawline(v0, v2, char_light);
	drawline(v1, v2, char_light);
}

int main() {
	setvbuf(stdout, NULL, _IOFBF, 1000000);
    signal(SIGSEGV, segvHandler);
	memset(arr, ' ', ROWS * COLS);
	for(int row = 0; row < ROWS; row++) {
		for(int col = 0; col < COLS; col++) {
			z_buffer[row][col] = -99999.99;
		}
	}

    double *vertex_positions = (double *) calloc(99999, sizeof(double)); 
    int *triangle_indices = (int *) calloc(99999, sizeof(int)); 

	FILE *file = fopen("teapot.txt", "r");
	ASSERT(file);
	int num_vertices = 0;
	int num_triangles = 0;
	char point_type;
	double a, b, c;
	char line[1024];

	//read from file
	while (fgets(line, 1024, file) != NULL) { 
		sscanf(line,"%c %lf %lf %lf", &point_type, &a, &b, &c);
		if (point_type == 'v') {
			vertex_positions[3 * num_vertices + 0] = a;
			vertex_positions[3 * num_vertices + 1] = b;
			vertex_positions[3 * num_vertices + 2] = c;
			++num_vertices;
		}
		if (point_type == 'f') {
			triangle_indices[3 * num_triangles + 0] = a - 1;
			triangle_indices[3 * num_triangles + 1] = b - 1;
			triangle_indices[3 * num_triangles + 2] = c - 1;
			++num_triangles;
		}
	}
	fclose(file);
    
    const int y_axis = 40;
	double t = 0.0;
	char *output = (char *) calloc(99999, sizeof(char));
    while (true) {
    	//rotation
        t += 0.033;
    	int traingle_index = 0; 
        while (traingle_index < (3 * num_triangles)) {
			int i = triangle_indices[traingle_index];
			int j = triangle_indices[traingle_index + 1];
			int k = triangle_indices[traingle_index + 2];

			// NOTE: assuming teapot x and y in range [-4, 4]
			// TODO: convert to [0, 80]
		    double x_i = (10.0f * vertex_positions[3 * i + 0]);
		    double y_i = (8.0f * vertex_positions[3 * i + 1])+ 15.0f;
		    double z_i = (10.0f * vertex_positions[3 * i + 2]);
		    double x_j = (10.0f * vertex_positions[3 * j + 0]);
		    double y_j = (8.0f * vertex_positions[3 * j + 1]) + 15.0f;
		    double z_j = (10.0f * vertex_positions[3 * j + 2]);
			double x_k = (10.0f * vertex_positions[3 * k + 0]);
			double y_k = (8.0f * vertex_positions[3 * k + 1]) + 15.0f;
			double z_k = (10.0f * vertex_positions[3 * k + 2]);

			// CLAIM: this is a rotation about y = y_axis
			double xi_new = x_i  * cos(t) + z_i * sin(t);
			double yi_new = y_i; 
			double zi_new = -(x_i - y_axis) * sin(t) + z_i * cos(t);

			double xj_new = x_j * cos(t) + z_j * sin(t);
			double yj_new = y_j;
			double zj_new = -(x_j - y_axis) * sin(t) + z_j * cos(t);

			double xk_new = x_k * cos(t) + z_k * sin(t);
			double yk_new = y_k; 
			double zk_new = -(x_k - y_axis) * sin(t) + z_k * cos(t);

			Vec3 p_i_new = { xi_new + 40, yi_new, zi_new };
			Vec3 p_j_new = { xj_new + 40, yj_new, zj_new };
			Vec3 p_k_new = { xk_new + 40, yk_new, zk_new };

			drawtriangle(p_i_new, p_j_new, p_k_new);
			traingle_index += 3;
		}

        output[0] = '\0';
        for (int i = ROWS - 1; i > -1; i--) {
			for (int j = 0; j < COLS; j++) {
				char numStr[100];
				sprintf(numStr, "%c", arr[i][j]);
				strcat(output, numStr);
			}
			strcat(output, "\n");
		}
		
		WriteConsole(
			GetStdHandle(STD_OUTPUT_HANDLE),
			output,
			strlen(output),
			NULL,
			NULL);
		Sleep(5);
		memset(arr, ' ', ROWS * COLS);
		for(int row = 0; row < ROWS; row++) {
			for(int col = 0; col < COLS; col++) {
				z_buffer[row][col] = -99999.99;
			}
		}
        fflush(stdout);

    }
    

	return 0;
}