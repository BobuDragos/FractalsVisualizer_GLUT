// Lab3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cfloat>
#include <cstring>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#define __STDC_WANT_LIB_EXT1__ 1;
#include <stdio.h>

using namespace std;

// dimensiunea ferestrei in pixeli
#define dim 300
#define NRITER_JF 5000
#define NRITER_MDB 5000
// modulul maxim pentru testarea apartenentei la mult.Julia-Fatou
#define MODMAX_JF 10000000
#define MODMAX_MDB 2
// ratii ptr. CJuliaFatou
#define RX_JF 0.01
#define RY_JF 0.01
#define RX_MDB 0.01
#define RY_MDB 0.01

unsigned char prevKey;

class Color {
public:
	Color(double r, double g, double b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}
	double getR() {
		return r;
	}
	double getG() {
		return g;
	}
	double getB() {
		return b;
	}
private:
	double r;
	double g;
	double b;
};
vector<Color*> colors;

void createColors(int numberOfColors) {

	double stepsPerColor = numberOfColors / 3.0;
	int stepsPerColorInt = stepsPerColor + 0.5;
	int step = 1;
	double r = 0.0;
	double g = 0.0;
	double b = 0.0;

	while (step < stepsPerColorInt)
	{
		colors.push_back(new Color(r, g, b));
		r += 1.0 / stepsPerColorInt;
		step++;
	}
	r = 0.5;
	step = 0;
	while (step < stepsPerColorInt)
	{
		colors.push_back(new Color(r, g, b));
		g += 1.0 / stepsPerColorInt;
		step++;
	}
	r = 0.1;
	g = 0.1;
	step = 0;
	while (step < stepsPerColorInt)
	{
		colors.push_back(new Color(r, g, b));
		b += 1.0 / stepsPerColorInt;
		step++;
	}
}

class CComplex {
public:
	CComplex() : re(0.0), im(0.0) {}
	CComplex(double re1, double im1) : re(re1 * 1.0), im(im1 * 1.0) {}
	CComplex(const CComplex& c) : re(c.re), im(c.im) {}
	~CComplex() {}

	CComplex& operator=(CComplex& c)
	{
		re = c.re;
		im = c.im;
		return *this;
	}

	double getRe() { return re; }
	void setRe(double re1) { re = re1; }

	double getIm() { return im; }
	void setIm(double im1) { im = im1; }

	double getModul() { return sqrt(re * re + im * im); }

	int operator==(CComplex& c1)
	{
		return ((re == c1.re) && (im == c1.im));
	}

	CComplex pow2()
	{
		CComplex rez;
		rez.re = powl(re * 1.0, 2) - powl(im * 1.0, 2);
		rez.im = 2.0 * re * im;
		return rez;
	}

	friend CComplex& operator+(CComplex& c1, CComplex& c2);
	friend CComplex& operator*(CComplex& c1, CComplex& c2);

	void print(FILE* f)
	{
		fprintf(f, "%.20f%+.20f i", re, im);
	}

private:
	double re, im;
};

CComplex& operator +(CComplex& c1, CComplex& c2)
{
	CComplex* rez = new CComplex(c1.re + c2.re, c1.im + c2.im);
	return *rez;
}

CComplex& operator*(CComplex& c1, CComplex& c2)
{
	CComplex* rez = new CComplex(c1.re * c2.re - c1.im * c2.im,
		c1.re * c2.im + c1.im * c2.re);
	return *rez;
}

int nivel = 0;

class CJuliaFatou {
public:
	CJuliaFatou()
	{
		// m.c se initializeaza implicit cu 0+0i

		m.nriter = NRITER_JF;
		m.modmax = MODMAX_JF;
	}

	CJuliaFatou(CComplex& c)
	{
		m.c = c;
		m.nriter = NRITER_JF;
		m.modmax = MODMAX_JF;
	}

	~CJuliaFatou() {}

	void setmodmax(double v) { assert(v <= MODMAX_JF); m.modmax = v; }
	double getmodmax() { return m.modmax; }

	void setnriter(int v) { assert(v <= NRITER_JF); m.nriter = v; }
	int getnriter() { return m.nriter; }

	// testeaza daca x apartine multimii Julia-Fatou Jc
	// returneaza 0 daca apartine, -1 daca converge finit, +1 daca converge infinit
  	int isIn(CComplex &x, int &i)
	{
		int rez = 0;
		// tablou in care vor fi memorate valorile procesului iterativ z_n+1 = z_n * z_n + c
		CComplex z0, z1;

		z0 = x;
		for (i = 1; i < m.nriter; i++)
		{
			z1 = z0 * z0 + m.c;
			if (z1 == z0)
			{
				// x nu apartine m.J-F deoarece procesul iterativ converge finit
				rez = -1;
				break;
			}
			else if (z1.getModul() > m.modmax)
			{
				// x nu apartine m.J-F deoarece procesul iterativ converge la infinit
				rez = 1;
				break;
			}
			z0 = z1;
		}

		return rez;
	}

	// afisarea multimii J-F care intersecteaza multimea argument
	void display(double xmin, double ymin, double xmax, double ymax)
	{
		glPushMatrix();
		glLoadIdentity();

		//    glTranslated((xmin + xmax) * 1.0 / (xmin - xmax), (ymin + ymax)  * 1.0 / (ymin - ymax), 0);
		//    glScaled(1.0 / (xmax - xmin), 1.0 / (ymax - ymin), 1);
		// afisarea propriu-zisa
		glBegin(GL_POINTS);
		for (double x = xmin; x <= xmax; x += RX_JF)
			for (double y = ymin; y <= ymax; y += RY_JF)
			{
				CComplex z(x, y);
				int i;
				int r = isIn(z, i);
				//        z.print(stdout);
				if (r == 0) 
				{
				glColor3f(1.0, 0.1, 0.1);
				glVertex3d(x,y,0);
				}
				else if (r == -1)
				{
				glColor3f(0.1, 1.0 * i / m.nriter, 0.5 - 1.0 * i / m.nriter);
				glVertex3d(x,y,0);
				}
				else if (r == 1)
				{
				glColor3f(0.1, 0.5 - 1.0 * i / m.nriter, 1.0 * i / m.nriter);
				glVertex3d(x,y,0);
				}
			}
		fprintf(stdout, "STOP\n");
		glEnd();

		glPopMatrix();
	}

private:
	struct SDate {
		CComplex c;
		// nr. de iteratii
		int nriter;
		// modulul maxim
		double modmax;
	} m;
};

class Mandelbrot {
public:
	Mandelbrot()
	{
		m.nriter = NRITER_MDB;
		m.modmax = MODMAX_MDB;
	}


	~Mandelbrot() {}

	void setmodmax(double v) { assert(v <= MODMAX_MDB); m.modmax = v; }
	double getmodmax() { return m.modmax; }

	void setnriter(int v) { assert(v <= NRITER_MDB); m.nriter = v; }
	int getnriter() { return m.nriter; }

	// testeaza daca x apartine multimii Mandelbrot
	// returneaza 0 daca apartine, nr pasi dupa care converge la infinit altfel
	int isIn(CComplex& x) //x = c
	{
		int rez = 0;
		int steps = 0;
		// tablou in care vor fi memorate valorile procesului iterativ z_n+1 = z_n * z_n
		CComplex z1, z2;

		z1 = x; //z1 = c
		for (int i = 2; i < m.nriter; i++)
		{
			steps++;
			z2 = z1 * z1 + x;
			if (z1.getModul() > m.modmax)
			{
				// x nu apartine Mandelbrot deoarece procesul iterativ converge la infinit
				rez = steps;
				break;
			}
			z1 = z2;
		}

		return rez;
	}

	void display(double xmin, double ymin, double xmax, double ymax)
	{
		int numberOfColors = 10;
		glPushMatrix();
		glLoadIdentity();
		glTranslated((xmin + xmax) * 2.0 / (xmin - xmax),	(ymin + ymax) * 2.0 / (ymin - ymax), 0);
		glScaled(2.0 / (xmax - xmin), 2.0 / (ymax - ymin), 1);

		// afisarea propriu-zisa
		glBegin(GL_POINTS);
		createColors(m.nriter);
		createColors(m.nriter);
		createColors(m.nriter);
		// fprintf(stdout, "%d\n", colors.size());

		int rmax = 0;
		for (double x = xmin; x <= xmax; x += RX_MDB)
			for (double y = ymin; y <= ymax; y += RY_MDB)
			{
				CComplex z(x, y);
				int r = isIn(z);

				glColor3f(1.0, 0.1, 0.1);
				if (r == 0)
				{
					glColor3f(colors[0]->getR(), colors[0]->getG(), colors[0]->getB());
					glVertex3d(x, y, 0);
				}
				else
				{
					//fprintf(stdout, "    %d\n", r);
					double value = double(r * colors.size()) / double(m.nriter);
					int intValue = value;
					//fprintf(stdout, "    %d\n", intValue);
					//fprintf(stdout, "    %lf\n", value);
					glColor3f(colors[intValue]->getR(), colors[intValue]->getG(), colors[intValue]->getB());
					//glColor3f(1.0, (1-value)*(1-value), 1.0);
					glVertex3d(x, y, 0);
				}
			}
		fprintf(stdout, "STOP\n");
		fprintf(stdout, "%d\n", rmax);
		glEnd();

		glPopMatrix();
	}

private:
	struct SDate {
		// nr. de iteratii
		int nriter;
		// modulul maxim
		double modmax;
	} m;
};

class C2coord
{
public:
	C2coord()
	{
		m.x = m.y = 0;
	}

	C2coord(double x, double y)
	{
		m.x = x;
		m.y = y;
	}

	C2coord(C2coord& p)
	{
		m.x = p.m.x;
		m.y = p.m.y;
	}

	C2coord& operator=(C2coord& p)
	{
		m.x = p.m.x;
		m.y = p.m.y;
		return *this;
	}

	int operator==(C2coord& p)
	{
		return ((m.x == p.m.x) && (m.y == p.m.y));
	}

protected:
	struct SDate
	{
		double x, y;
	} m;
};

class CPunct : public C2coord
{
public:
	CPunct() : C2coord(0.0, 0.0)
	{}

	CPunct(double x, double y) : C2coord(x, y)
	{}

	CPunct& operator=(CPunct& p)
	{
		m.x = p.m.x;
		m.y = p.m.y;
		return *this;
	}

	void getxy(double& x, double& y)
	{
		x = m.x;
		y = m.y;
	}

	int operator==(CPunct& p)
	{
		return ((m.x == p.m.x) && (m.y == p.m.y));
	}

	void marcheaza()
	{
		glBegin(GL_POINTS);
		glVertex2d(m.x, m.y);
		glEnd();
	}

	void print(FILE* fis)
	{
		fprintf(fis, "(%+f,%+f)", m.x, m.y);
	}

};

class CVector : public C2coord
{
public:
	CVector() : C2coord(0.0, 0.0)
	{
		normalizare();
	}

	CVector(double x, double y) : C2coord(x, y)
	{
		normalizare();
	}

	CVector& operator=(CVector& p)
	{
		m.x = p.m.x;
		m.y = p.m.y;
		return *this;
	}

	int operator==(CVector& p)
	{
		return ((m.x == p.m.x) && (m.y == p.m.y));
	}

	CPunct& getDest(CPunct& orig, double lungime)
	{
		double x, y;
		orig.getxy(x, y);
		CPunct* p = new CPunct(x + m.x * lungime, y + m.y * lungime);
		return *p;
	}

	void rotatie(double grade)
	{
		double x = m.x;
		double y = m.y;
		double t = 2 * (4.0 * atan(1)) * grade / 360.0;
		m.x = x * cos(t) - y * sin(t);
		m.y = x * sin(t) + y * cos(t);
		normalizare();
	}

	void deseneaza(CPunct p, double lungime)
	{
		double x, y;
		p.getxy(x, y);
		glColor3f(1.0, 0.1, 0.1);
		glBegin(GL_LINE_STRIP);
		glVertex2d(x, y);
		glVertex2d(x + m.x * lungime, y + m.y * lungime);
		glEnd();
	}

	void print(FILE* fis)
	{
		fprintf(fis, "%+fi %+fj", C2coord::m.x, C2coord::m.y);
	}

private:
	void normalizare()
	{
		double d = sqrt(C2coord::m.x * C2coord::m.x + C2coord::m.y * C2coord::m.y);
		if (d != 0.0)
		{
			C2coord::m.x = C2coord::m.x * 1.0 / d;
			C2coord::m.y = C2coord::m.y * 1.0 / d;
		}
	}
};

class CCurbaKoch
{
public:
	void segmentKoch(double lungime, int nivel, CPunct& p, CVector v)
	{
		CPunct p1;
		if (nivel == 0)
		{
			v.deseneaza(p, lungime);
		}
		else
		{
			//    v.print(stderr);
			//    fprintf(stderr, "\n");
			segmentKoch(lungime / 3.0, nivel - 1, p, v);
			p1 = v.getDest(p, lungime / 3.0);
			v.rotatie(60);
			//    v.print(stderr);
			//    fprintf(stderr, "\n");
			segmentKoch(lungime / 3.0, nivel - 1, p1, v);
			p1 = v.getDest(p1, lungime / 3.0);
			v.rotatie(-120);
			//    v.print(stderr);
			//    fprintf(stderr, "\n");
			segmentKoch(lungime / 3.0, nivel - 1, p1, v);
			p1 = v.getDest(p1, lungime / 3.0);
			v.rotatie(60);
			//    v.print(stderr);
			//    fprintf(stderr, "\n");
			segmentKoch(lungime / 3.0, nivel - 1, p1, v);
		}
	}

	void afisare(double lungime, int nivel)
	{
		CVector v1(sqrt(3.0) / 2.0, 0.5);
		CPunct p1(-1.0, 0.0);

		CVector v2(0.0, -1.0);
		CPunct p2(0.5, sqrt(3.0) / 2.0);

		CVector v3(-sqrt(3.0) / 2.0, 0.5);
		CPunct p3(0.5, -sqrt(3.0) / 2.0);

		segmentKoch(lungime, nivel, p1, v1);
		segmentKoch(lungime, nivel, p2, v2);
		segmentKoch(lungime, nivel, p3, v3);
	}
};

class CArboreBinar
{
public:
	void arboreBinar(double lungime, int nivel, CPunct& p, CVector v)
	{
		CPunct p1;
		if (nivel == 0)
		{
			v.deseneaza(p, lungime);
		}
		else
		{
			arboreBinar(lungime, nivel - 1, p, v);
			p1 = v.getDest(p, lungime);

			v.rotatie(-45);
			arboreBinar(lungime / 2.0, nivel - 1, p1, v);

			v.rotatie(90);
			arboreBinar(lungime / 2.0, nivel - 1, p1, v);
		}
	}

	void afisare(double lungime, int nivel)
	{
		CVector v(0.0, -1.0);
		CPunct p(0.0, 1.0);

		arboreBinar(lungime, nivel, p, v);
	}
};

class CArborePerron
{
public:
	void arborePerron(double lungime,
		int nivel,
		double factordiviziune,
		CPunct p,
		CVector v)
	{
		assert(factordiviziune != 0);
		CPunct p1, p2;
		if (nivel == 0)
		{
		}
		else
		{
			v.rotatie(30);
			v.deseneaza(p, lungime);
			p1 = v.getDest(p, lungime);
			arborePerron(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

			v.rotatie(-90);
			v.deseneaza(p, lungime);
			p1 = v.getDest(p, lungime);
			p2 = p1;

			v.rotatie(-30);
			v.deseneaza(p1, lungime);
			p1 = v.getDest(p1, lungime);
			arborePerron(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

			p1 = p2;
			v.rotatie(90);
			v.deseneaza(p1, lungime);
			p1 = v.getDest(p1, lungime);
			p2 = p1;

			v.rotatie(30);
			v.deseneaza(p1, lungime);
			p1 = v.getDest(p1, lungime);
			arborePerron(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

			p1 = p2;
			v.rotatie(-90);
			v.deseneaza(p1, lungime);
			p1 = v.getDest(p1, lungime);
			arborePerron(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);
		}
	}

	void afisare(double lungime, int nivel)
	{
		CVector v(0.0, 1.0);
		CPunct p(0.0, -1.0);

		v.deseneaza(p, 0.25);
		p = v.getDest(p, 0.25);
		arborePerron(lungime, nivel, 0.4, p, v);
	}
};

class CCurbaHilbert
{
public:
	void curbaHilbert(double lungime, int nivel, CPunct& p, CVector& v, int d)
	{
		if (nivel == 0)
		{
		}
		else
		{
			v.rotatie(d * 90);
			curbaHilbert(lungime, nivel - 1, p, v, -d);

			v.deseneaza(p, lungime);
			p = v.getDest(p, lungime);

			v.rotatie(-d * 90);
			curbaHilbert(lungime, nivel - 1, p, v, d);

			v.deseneaza(p, lungime);
			p = v.getDest(p, lungime);

			curbaHilbert(lungime, nivel - 1, p, v, d);

			v.rotatie(-d * 90);
			v.deseneaza(p, lungime);
			p = v.getDest(p, lungime);

			curbaHilbert(lungime, nivel - 1, p, v, -d);

			v.rotatie(d * 90);
		}
	}

	void afisare(double lungime, int nivel)
	{
		CVector v(0.0, 1.0);
		CPunct p(0.0, 0.0);

		curbaHilbert(lungime, nivel, p, v, 1);
	}
};

class Imagine1
{
public:
	void imagine1(double lungime,
		int nivel,
		CPunct p,
		CVector v)
	{
		CPunct p1, p2;
		if (nivel == 0)
		{
		}
		else
		{
			// gasire punct stanga sus, patrat central
			v.rotatie(-90);
			p2 = v.getDest(p, lungime / 3);
			v.rotatie(-90);
			p2 = v.getDest(p2, lungime / 3);

			v.rotatie(180);

			//desen normal

			v.rotatie(-90);
			v.deseneaza(p2, lungime / 3);
			p2 = v.getDest(p2, lungime / 3);
			v.rotatie(-90);
			v.deseneaza(p2, lungime / 3);

			p2 = v.getDest(p2, lungime / 3);
			v.rotatie(-90);
			v.deseneaza(p2, lungime / 3);

			p2 = v.getDest(p2, lungime / 3);
			v.rotatie(-90);
			v.deseneaza(p2, lungime / 3);

			// recursie 8 patratele
			CVector vPar(0.0, 1.0);
			imagine1(lungime / 3, nivel - 1, p, vPar);
			v.rotatie(-90);
			p2 = v.getDest(p, lungime / 3);
			imagine1(lungime / 3, nivel - 1, p2, vPar);
			p2 = v.getDest(p2, lungime / 3);
			imagine1(lungime / 3, nivel - 1, p2, vPar);

			v.rotatie(-90);
			p2 = v.getDest(p2, lungime / 3);
			imagine1(lungime / 3, nivel - 1, p2, vPar);
			p2 = v.getDest(p2, lungime / 3);
			imagine1(lungime / 3, nivel - 1, p2, vPar);

			v.rotatie(-90);
			p2 = v.getDest(p2, lungime / 3);
			imagine1(lungime / 3, nivel - 1, p2, vPar);
			p2 = v.getDest(p2, lungime / 3);
			imagine1(lungime / 3, nivel - 1, p2, vPar);

			v.rotatie(-90);
			p2 = v.getDest(p2, lungime / 3);
			imagine1(lungime / 3, nivel - 1, p2, vPar);

		}
	}

	void afisare(double lungime, int nivel)
	{
		CVector v(0.0, 1.0);
		CPunct p1(-lungime / 2, lungime / 2);
		CPunct p2;

		v.rotatie(-90);
		v.deseneaza(p1, lungime);
		p1 = v.getDest(p1, lungime);
		p2 = p1;
		v.rotatie(-90);
		v.deseneaza(p1, lungime);

		p1 = v.getDest(p1, lungime);
		v.rotatie(-90);
		v.deseneaza(p1, lungime);

		p1 = v.getDest(p1, lungime);
		v.rotatie(-90);
		v.deseneaza(p1, lungime);

		CPunct p3(-lungime / 2, lungime / 2);
		CVector v2(0.0, 1.0);
		imagine1(lungime, nivel + 1, p3, v2);
	}
};
class Imagine2
{
public:
	void imagine2(double lungime,
		int nivel,
		double factordiviziune,
		CPunct p,
		CVector v)
	{
		assert(factordiviziune != 0);
		CPunct p1, p2;
		if (nivel == 0)
		{
		}
		else
		{
			v.rotatie(-45);
			v.deseneaza(p, lungime);
			p1 = v.getDest(p, lungime);
			imagine2(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

			v.rotatie(90);
			v.deseneaza(p, lungime);
			p1 = v.getDest(p, lungime);
			p2 = p1;


			v.rotatie(15);
			v.deseneaza(p1, lungime);
			p1 = v.getDest(p1, lungime);
			imagine2(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

			p1 = p2;
			v.rotatie(-60);
			v.deseneaza(p1, lungime);
			p1 = v.getDest(p1, lungime);
			p2 = p1;

			v.rotatie(30);
			v.deseneaza(p1, lungime / 2);
			p1 = v.getDest(p1, lungime / 2);
			imagine2(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

			p1 = p2;
			v.rotatie(-120);
			v.deseneaza(p1, lungime / 2);
			p1 = v.getDest(p1, lungime / 2);
			imagine2(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);
		}
	}

	void afisare(double lungime, int nivel)
	{
		CVector v(0.0, -1.0);
		CPunct p(-0.3, 0.9);

		v.deseneaza(p, 0.1);
		p = v.getDest(p, 0.1);
		imagine2(lungime, nivel, 0.4, p, v);
	}
};
class Image3
{
public:
	void image3(double lungime, int nivel, CPunct& p, CVector v)
	{
		CPunct p1;
		if (nivel == -1) {

		}
		else
			if (nivel == 0)
			{
				v.deseneaza(p, lungime);
			}
			else
			{
				v.rotatie(60);
				CPunct p1 = v.getDest(p, lungime / 2.0);
				v.rotatie(-180);
				image3(lungime / 2.0, nivel - 1, p1, v);

				v.rotatie(180);
				v.rotatie(-60);
				image3(lungime / 2.0, nivel - 1, p1, v);
				p1 = v.getDest(p1, lungime / 2.0);

				//v.rotatie(-180);
				v.rotatie(-60);
				p1 = v.getDest(p1, lungime / 2.0);
				v.rotatie(180);
				image3(lungime / 2.0, nivel - 1, p1, v);

			}
	}


	void afisare(double lungime, int nivel)
	{
		CVector v(0.0, -1.0);
		CPunct p(-0.80, 0.90);
		image3(lungime, nivel, p, v);
		/*
				v.rotatie(60);
				CPunct p1 = v.getDest(p, lungime / 2.0);
				v.rotatie(-180);
				image3(lungime/2.0, nivel - 1, p1, v);

				v.rotatie(180);
				v.rotatie(-60);
				image3(lungime/ 2.0, nivel - 1, p1, v);
				p1 = v.getDest(p1, lungime / 2.0);

				//v.rotatie(-180);
				v.rotatie(-60);
				p1 = v.getDest(p1, lungime / 2.0);
				v.rotatie(180);
				image3(lungime / 2.0, nivel - 1, p1, v);
			*/
	}
};

// afisare curba lui Koch "fulg de zapada"
void Display1() {
	CCurbaKoch cck;
	cck.afisare(sqrt(3.0), nivel);

	char c[3];
	sprintf(c, "%2d", nivel);
	glRasterPos2d(-0.98, -0.98);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

	glRasterPos2d(-1.0, 0.9);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'c');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'u');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'u');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'K');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'c');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'h');

	nivel++;
}

// afisare arbore binar
void Display2() {
	CArboreBinar cab;
	cab.afisare(1, nivel);

	char c[3];
	sprintf(c, "%2d", nivel);
	glRasterPos2d(-0.98, -0.98);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

	glRasterPos2d(-1.0, 0.9);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'n');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');

	nivel++;
}

// afisare arborele lui Perron
void Display3() {
	CArborePerron cap;

	char c[3];
	sprintf(c, "%2d", nivel);
	glRasterPos2d(-0.98, -0.98);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

	glRasterPos2d(-1.0, -0.9);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'P');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'n');

	glPushMatrix();
	glLoadIdentity();
	glScaled(0.4, 0.4, 1);
	glTranslated(-0.5, -0.5, 0.0);
	cap.afisare(1, nivel);
	glPopMatrix();
	nivel++;
}

// afisare curba lui Hilbert
void Display4() {
	CCurbaHilbert cch;
	cch.afisare(0.05, nivel);

	char c[3];
	sprintf(c, "%2d", nivel);
	glRasterPos2d(-0.98, -0.98);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

	glRasterPos2d(-1.0, -0.9);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'c');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'u');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'H');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');

	nivel++;
}
//Imagine 1
void Display5() {
	Imagine1 im;

	im.afisare(1.50, nivel);

	char c[3];
	sprintf(c, "%2d", nivel);
	glRasterPos2d(-0.98, -0.98);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

	glRasterPos2d(-1.0, -0.9);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'I');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'm');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'g');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'n');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'u');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'm');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '1');

	nivel++;
}

void Display6() {
	Imagine2 im2;

	im2.afisare(0.4, nivel);

	char c[3];
	sprintf(c, "%2d", nivel);
	glRasterPos2d(-0.98, -0.98);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

	glRasterPos2d(-1.0, -0.9);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'I');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'm');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'g');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'n');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'u');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'm');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '2');

	nivel++;
}

void Display7() {
	Image3 im3;

	im3.afisare(1.6, nivel);

	char c[3];
	sprintf(c, "%2d", nivel);
	glRasterPos2d(-0.98, -0.98);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

	glRasterPos2d(-1.0, -0.9);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'I');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'm');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'g');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'n');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'u');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'm');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '3');

	nivel++;
}

void Display8() {
	CComplex c(-0.12375, 0.056805);
	CJuliaFatou cjf(c);

	glColor3f(1.0, 0.1, 0.1);
	cjf.setnriter(30);
	cjf.display(-0.8, -0.4, 0.8, 0.4);
}

// multimea Julia-Fatou pentru z0 = 0 si c = -0.012+0.74i
void Display9() {
	CComplex c(-0.012, 0.74);
	CJuliaFatou cjf(c);

	glColor3f(1.0, 0.1, 0.1);
	cjf.setnriter(30);
	cjf.display(-1, -1, 1, 1);
}

void Display10() {
	Mandelbrot* mdb = new Mandelbrot();
	//glColor3f(1.0, 0.1, 0.1);
	mdb->setnriter(50);
	mdb->display(-2, -2, 2, 2);
}


void Init(void) {

	glClearColor(1.0, 1.0, 1.0, 1.0);

	glLineWidth(1);

	glPointSize(3);

	glPolygonMode(GL_FRONT, GL_LINE);
}

void Display(void)
{
	switch (prevKey)
	{
	case '0':
		glClear(GL_COLOR_BUFFER_BIT);
		nivel = 0;
		fprintf(stderr, "nivel = %d\n", nivel);
		break;
	case '1':
		glClear(GL_COLOR_BUFFER_BIT);
		Display1();
		break;
	case '2':
		glClear(GL_COLOR_BUFFER_BIT);
		Display2();
		break;
	case '3':
		glClear(GL_COLOR_BUFFER_BIT);
		Display3();
		break;
	case '4':
		glClear(GL_COLOR_BUFFER_BIT);
		Display4();
		break;
	case '5':
		glClear(GL_COLOR_BUFFER_BIT);
		Display5();
		break;
	case '6':
		glClear(GL_COLOR_BUFFER_BIT);
		Display6();
		break;
	case '7':
		glClear(GL_COLOR_BUFFER_BIT);
		Display7();
		break;
	case '8':
		glClear(GL_COLOR_BUFFER_BIT);
		Display8();
		break;
	case '9':
		glClear(GL_COLOR_BUFFER_BIT);
		Display9();
		break;
	case '/':
		glClear(GL_COLOR_BUFFER_BIT);
		Display10();
		break;
	default:
		break;
	}

	glFlush();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void KeyboardFunc(unsigned char key, int x, int y)
{
	prevKey = key;
	if (key == 27) // escape
		exit(0);
	glutPostRedisplay();
}

void MouseFunc(int button, int state, int x, int y)
{
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitWindowSize(dim, dim);

	glutInitWindowPosition(100, 100);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutCreateWindow(argv[0]);

	Init();

	glutReshapeFunc(Reshape);

	glutKeyboardFunc(KeyboardFunc);

	glutMouseFunc(MouseFunc);

	glutDisplayFunc(Display);

	glutMainLoop();

	return 0;
}
