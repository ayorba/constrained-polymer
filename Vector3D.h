#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include <iostream>


class Vector3D
{

	public:

	double x,y,z;

	Vector3D() : x{0.0}, y{0.0}, z{0.0}{}
	Vector3D(double X, double Y, double Z) : x{X}, y{Y}, z{Z}{}


	// Zero Vector
	void zeroVector()
	{
		this->x = 0.0;
		this->y = 0.0;
		this->z = 0.0;
	}	


	// Addition
	Vector3D operator+(const Vector3D& v) const 
	{
		return Vector3D(x + v.x, y + v.y, z + v.z);
	}

	// Addition and Assigment
	Vector3D& operator+=(const Vector3D& v) 
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}


	// Subtraction
	Vector3D operator-(const Vector3D& v) const 
	{
		return Vector3D(x - v.x, y - v.y, z - v.z);
	}

	// Subtraction and Assigment
	Vector3D& operator-=(const Vector3D& v) 
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}


	// Scalar multiplication
	Vector3D operator*(double scalar) const
	{
		return Vector3D(x * scalar, y * scalar, z * scalar);
	}
	friend inline Vector3D operator*(double scalar, const Vector3D& vec);


	// Scalar Multiplication and Assigment
	Vector3D& operator*=(const double scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}


	// Scalar division
	Vector3D operator/(double scalar) const
	{
		return Vector3D(x / scalar, y / scalar, z / scalar);
	}

	// Scalar Division and Assigment
	Vector3D& operator/=(const double scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}


	// Negating the vector
	Vector3D operator-() const
	{
		return Vector3D(-x, -y, -z);
	}


	// Checking equality
	bool operator==(const Vector3D& v) const
	{
		if(x==v.x && y==v.y && z==v.z)
		{
			return 1;
		}

		return 0;
	}


	// Dot product
	inline double dot(const Vector3D& v) const 
	{
		return x * v.x + y * v.y + z * v.z;
	}


	// Cross product
	Vector3D cross(const Vector3D& v) const
	{
		return Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}


	// Norm
	inline double norm() const 
	{
		return sqrt(x*x + y*y + z*z);
	}


	// Normalize input vector in place
	Vector3D& normalize()
	{
		double mag = norm();
		if(mag > 0)
		{
			x /= mag;
			y /= mag;
			z /= mag;

			return *this;
		}
		else
		{
			std::cerr<<"mag = 0 in Vector3D::normalize().\n";
			exit(1);
		}
	}


	// Normalize input vector and return as new vector
	// This is good for assigning the normalized vector to a new Vector3D
	// without changing the Vector3D that is normalized.
	Vector3D normalized() const
	{
		double mag = norm();
		if(mag > 0)
		{
			return Vector3D(x/mag, y/mag, z/mag);
		}
		else
		{
			std::cerr<<"Vector3D Error: magnitude = 0 in Vector3D::normalized().\n";
			exit(1);
		}
	}


	// Print vector
	void print() const
	{
		std::cout<<"x,y,z: "<<x<<", "<<y<<", "<<z<<"\n";
	}


};


inline Vector3D operator*(double scalar, const Vector3D& vec)
{
	return vec * scalar;
}


#endif


