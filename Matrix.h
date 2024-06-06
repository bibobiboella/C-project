#pragma once
#include <vector>
#include <iostream>
using namespace std;
typedef vector<double> Vector;
typedef vector<Vector> Matrix;

Vector operator-(const Vector& V, const Vector& W);
Vector operator+(const Vector& V, const Vector& W);
Vector operator*(const Vector& V, const Vector& W);
Vector operator*(const double& V, const Vector& W);
Vector sqrt(const Vector& V);
void operator+=(Vector& V, const Vector& W);
