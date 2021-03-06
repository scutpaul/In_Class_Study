// PRlab2.cpp: 定义控制台应用程序的入口点。
// KMeans的实现

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <algorithm>
#define sigema 0.1
using namespace std;
typedef struct { double x, y; int group; } point_t, *point;

#	define for_n for (c = cent, i = 0; i < n_cluster; i++, c++) //macro for traverse through centroids
#	define for_len for (j = 0, p = pts; j < len; j++, p++) //macro for traverse through points
void Cal(point, int, point, int);
const int maxn = 1e5 + 5;
const double eps = 1e-4;
const double inf = (double)(1 << 29)*(1 << 29);
double dev[maxn];

double randf(double m)
{
	return m * rand() / (RAND_MAX - 1.);
}

point gen_xy(int count, double radius) //generate data
{
	double ang, r;
	point p, pt = (point_t*)malloc(sizeof(point_t) * count);
	int i = 0;
	// note: this is not a uniform 2-d distribution 
	for (p = pt + count; p-- > pt;) {
		i++;
		ang = randf(2 * M_PI);
		r = randf(radius);
		p->x = r * cos(ang)+(i%5)*40;
		p->y = r * sin(ang) + ((i % 5)%2) * 40;
	}

	return pt;
}

inline double dist2(point a, point b) //calculate l2 distance
{
	double x = a->x - b->x, y = a->y - b->y;
	return x * x + y * y;
}
inline double dist(point a, point b, double _std)
{
	
	//double x = a->x - b->x, y = a->y - b->y;
	//double list =  x * x + y * y;
	//return exp(list / (2 * sigema));
	//return abs(x) + abs(y);
	double tmp = dist2(a, b) / (_std * 2);
	if (tmp > 1000) return (inf*inf);
	if (tmp < eps) return 1;
	return exp(tmp);
}
void Cal(point pts, int len, point cent, int n_cluster) {
	int i, j;
	point c, p;
	for_n{
		double sum = 0;
	int cnt = 0;
	for_len{
		if (i != p->group) continue;
	cnt++;
	sum += dist2(p, c);
	}
		if (cnt == 0) {
			dev[i] = eps;
		}
		else {
			sum /= cnt;
			dev[i] = max(eps,sqrt(sum));
		}
	}
}

inline int nearest(point pt, point cent, int n_cluster) //find nearest cluster center of point pt
{
	int i, min_i;
	point c;
	double d, min_d;

	//TODO: find the nearest cluster center of point pt, return its index: min_i
	min_d = HUGE_VAL;
	min_i = pt->group;
	for_n{
		d = dist2(c,pt);
		//d = dist(c,pt,dev[i]);
		if (d < min_d) {
			min_d = d;
			min_i = i;
		}
	}
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
		for_n{
			double sum_x,sum_y,sum_have;
			sum_x = sum_y = sum_have = 0;
			for_len{
				if (p->group == i) {
					sum_x += p->x;
					sum_y += p->y;
					sum_have += 1;
				}
			}
			c->x = sum_x / sum_have;
			c->y = sum_y / sum_have;
		}
		//Cal(pts, len, cent, n_cluster);
		changed = 0;
		// TODO: find closest centroid of each point & update its group 
		for_len{
			min_i = nearest(p,cent,n_cluster);
			if (min_i != p->group) {
				changed ++;
				p->group = min_i;
			}
		}
		// you should update 'changed' when you assign a new centroid to a point
		//printf("%d\n", changed);
	} while (changed > (len >> 10)); // stop when groups of points have changed enough time 

	for_n{ c->group = i; }

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
	for_len{
		if (max_x < p->x) max_x = p->x;
	if (min_x > p->x) min_x = p->x;
	if (max_y < p->y) max_y = p->y;
	if (min_y > p->y) min_y = p->y;
	}
	scale = W / (max_x - min_x);
	if (scale > H / (max_y - min_y)) scale = H / (max_y - min_y);
	cx = (max_x + min_x) / 2;
	cy = (max_y + min_y) / 2;
	//fstream fp("d:\\P.txt", ios::out);
	//fstream fc("d:\\C.txt", ios::out);
	double sum_dist = 0.0;
	for_len{
		//fp << (p->x - cx) * scale + W / 2 << "," << (p->y - cy) * scale + H / 2 << "," << p->group<<endl;
		sum_dist += dist2(p, cent + p->group);
	}
	printf("%d,%f\n", n_cluster, sum_dist);
	//cout << n_cluster << "," << sum_dist << endl;
	//for_n{
	//	fc << (c->x - cx) * scale + W / 2 << "," << (c->y - cy) * scale + H / 2 << "," << c->group<<endl;
	//}
	
	//fp.close();
	//fc.close();
	/*
	for_n{
		for_len{
		if (p->group != i) continue;
		printf("points: %.3f %.3f \n",
		(p->x - cx) * scale + W / 2,
		(p->y - cy) * scale + H / 2);
	}
		printf("center: %g %g \n\n",
		(c->x - cx) * scale + W / 2,
			(c->y - cy) * scale + H / 2);
	}*/
#	undef for_n
#	undef for_len
}

#define PTS 10000
//#define K 11
int main()
{
	freopen("o.csv", "w", stdout);
	point v = gen_xy(PTS, 10);
	for(int K = 1; K < 12; K++) {
		point c = KMeans(v, PTS, K);
		print_eps(v, PTS, c, K);
		free(c);
	}
	// free(v); free(c);
	return 0;
}