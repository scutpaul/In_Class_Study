#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
typedef struct { double x, y; int group; } point_t, *point;

#	define for_n for (c = cent, i = 0; i < n_cluster; i++, c++) //macro for traverse through centroids
#	define for_len for (j = 0, p = pts; j < len; j++, p++) //macro for traverse through points

double randf(double m)
{
	return m * rand() / (RAND_MAX - 1.);
}
 
point gen_xy(int count, double radius) //generate data
{
	double ang, r;
	point p, pt = (point_t*)malloc(sizeof(point_t) * count);
 
	// note: this is not a uniform 2-d distribution 
	for (p = pt + count; p-- > pt;) {
		ang = randf(2 * M_PI);
		r = randf(radius);
		p->x = r * cos(ang);
		p->y = r * sin(ang);
	}
 
	return pt;
}
 
inline double dist2(point a, point b) //calculate l2 distance
{
	double x = a->x - b->x, y = a->y - b->y;
	return x*x + y*y;
}
 
inline int nearest(point pt, point cent, int n_cluster) //find nearest cluster center of point pt
{
	int i, min_i;
	point c;
	double d, min_d;
 
	//TODO: find the nearest cluster center of point pt, return its index: min_i
	return min_i;
}
 
point KMeans(point pts, int len, int n_cluster)
{
	int i, j, min_i;
	int changed;
 
	point cent = (point_t*)malloc(sizeof(point_t) * n_cluster), p, c; //cent is an array of centroids, c is an iterator on cent
 
	// assign init grouping randomly 
	for_len p->group = j % n_cluster;
 
	do {
		//TODO: calculate centroids of each group
  		changed = 0;
		// TODO: find closest centroid of each point & update its group 
		// you should update 'changed' when you assign a new centroid to a point

	} while (changed > (len >> 10)); // stop when groups of points have changed enough time 
 
	for_n { c->group = i; }
 
	return cent;
}
 
void print_eps(point pts, int len, point cent, int n_cluster)
{
#	define W 400
#	define H 400
	int i, j;
	point p, c;
	double min_x, max_x, min_y, max_y, scale, cx, cy;
 
	max_x = max_y = -(min_x = min_y = HUGE_VAL);
	for_len {
		if (max_x < p->x) max_x = p->x;
		if (min_x > p->x) min_x = p->x;
		if (max_y < p->y) max_y = p->y;
		if (min_y > p->y) min_y = p->y;
	}
	scale = W / (max_x - min_x);
	if (scale > H / (max_y - min_y)) scale = H / (max_y - min_y);
	cx = (max_x + min_x) / 2;
	cy = (max_y + min_y) / 2;
 
	for_n {
		for_len {
			if (p->group != i) continue;
			printf("points: %.3f %.3f \n",
				(p->x - cx) * scale + W / 2,
				(p->y - cy) * scale + H / 2);
		}
		printf("center: %g %g \n\n",
			(c->x - cx) * scale + W / 2,
			(c->y - cy) * scale + H / 2);
	}
#	undef for_n
#	undef for_len
}
 
#define PTS 100
#define K 10
int main()
{
	int i;
	point v = gen_xy(PTS, 10);
	point c = KMeans(v, PTS, K);
	print_eps(v, PTS, c, K);
	// free(v); free(c);
	return 0;
}