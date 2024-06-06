#include "Matrix.h"
#include <cmath>
Vector operator-(const Vector& V, const Vector& W)
  {
      int d = (int)V.size();
      Vector U(d);
      for (int j = 0; j < d; j++) U[j] = V[j] - W[j];
      return U;
  }

Vector operator+(const Vector& V, const Vector& W)
  {
      int d = (int)V.size();
      Vector U(d);
      for (int j = 0; j < d; j++) U[j] = V[j] + W[j];
      return U;
  }

Vector operator*(const Vector& V, const Vector& W)
{
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++) U[j] = V[j] * W[j];
    return U;
}


Vector operator*(const double& V, const Vector& W)
  {
      int d = (int)W.size();
      Vector U(d);
      for (int j = 0; j < d; j++) U[j] = V * W[j];
      return U;
  }

Vector sqrt(const Vector& V)
{
    int d = (int)V.size();
    Vector U(d);
    for(int j = 0; j < d; j++) U[j] = sqrt(V[j]);
    return U;
}

void operator+=(Vector& V, const Vector& W) {
    if (V.size() != W.size()) {
        throw std::invalid_argument("Vectors must be of the same size to perform addition.");
    }
    for (size_t j = 0; j < V.size(); j++) {
        V[j] += W[j];
    }
}
