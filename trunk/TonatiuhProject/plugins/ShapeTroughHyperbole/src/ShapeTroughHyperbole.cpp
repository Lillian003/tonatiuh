/***************************************************************************
Copyright (C) 2008 by the Tonatiuh Software Development Team.

This file is part of Tonatiuh.

Tonatiuh program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


Acknowledgments:

The development of Tonatiuh was started on 2004 by Dr. Manuel J. Blanco,
then Chair of the Department of Engineering of the University of Texas at
Brownsville. From May 2004 to July 2008, it was supported by the Department
of Energy (DOE) and the National Renewable Energy Laboratory (NREL) under
the Minority Research Associate (MURA) Program Subcontract ACQ-4-33623-06.
During 2007, NREL also contributed to the validation of Tonatiuh under the
framework of the Memorandum of Understanding signed with the Spanish
National Renewable Energy Centre (CENER) on February, 20, 2007 (MOU#NREL-07-117).
Since June 2006, the development of Tonatiuh is being led by the CENER, under the
direction of Dr. Blanco, now Director of CENER Solar Thermal Energy Department.

Developers: Manuel J. Blanco (mblanco@cener.com), Amaia Mutuberria, Victor Martin.

Contributors: Javier Garcia-Barberena, I�aki Perez, Inigo Pagola,  Gilda Jimenez,
Juana Amieva, Azael Mancillas, Cesar Cantu.
***************************************************************************/

#include <algorithm>
#include <vector>

#include <QIcon>
#include <QMap>
#include <QMessageBox>

#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include "DifferentialGeometry.h"
#include "Ray.h"
#include "ShapeTroughHyperbole.h"
#include "tgf.h"
#include "tgc.h"
#include "Vector3D.h"


SO_NODE_SOURCE(ShapeTroughHyperbole);

void ShapeTroughHyperbole::initClass()
{
	SO_NODE_INIT_CLASS(ShapeTroughHyperbole, TShape, "TShape");
}

ShapeTroughHyperbole::ShapeTroughHyperbole()
:m_lastA0Value( 0.5 ),
 m_lastAsymptoticAngleValue( 45.0 * tgc::Degree ),
 m_lastHeightValue( 1.0 ),
 m_lastTruncationValue( 0.0 ),
 m_lastZLengthXMinValue(1.0),
 m_lastZLengthXMaxValue(1.0)
{
	SO_NODE_CONSTRUCTOR(ShapeTroughHyperbole);
	SO_NODE_ADD_FIELD( a0, (m_lastA0Value) );
	SO_NODE_ADD_FIELD( asymptoticAngle, ( m_lastAsymptoticAngleValue ) );
	SO_NODE_ADD_FIELD( zLengthXMin, (m_lastZLengthXMinValue) );
	SO_NODE_ADD_FIELD( zLengthXMax, (m_lastZLengthXMaxValue) );
	SO_NODE_ADD_FIELD( theoreticalHeight, ( m_lastHeightValue ) );
	SO_NODE_ADD_FIELD( truncation, ( m_lastTruncationValue ) );

	SoFieldSensor* m_apertureSensor = new SoFieldSensor(updateApertureValue, this);
	m_apertureSensor->attach( &a0 );
	SoFieldSensor* m_angleSensor = new SoFieldSensor(updateAngleValue, this);
	m_angleSensor->attach( &asymptoticAngle );
	SoFieldSensor* lengthSensor1 = new SoFieldSensor(updateLengthValues, this);
	lengthSensor1->attach( &zLengthXMin );
	SoFieldSensor* lengthSensor2 = new SoFieldSensor(updateLengthValues, this);
	lengthSensor2->attach( &zLengthXMax );
	SoFieldSensor* m_heightSensor = new SoFieldSensor(updateHeightValue, this);
	m_heightSensor->attach( &theoreticalHeight );
	SoFieldSensor* m_truncationSensor = new SoFieldSensor(updateTruncationValue, this);
	m_truncationSensor->attach( &truncation );

}

ShapeTroughHyperbole::~ShapeTroughHyperbole()
{
}


double ShapeTroughHyperbole::GetArea() const
{
	return -1;
}

QString ShapeTroughHyperbole::GetIcon() const
{
	return ":/icons/ShapeTroughHyperbole.png";
}

bool ShapeTroughHyperbole::Intersect(const Ray& objectRay, double *tHit, DifferentialGeometry *dg) const
{
	double a = a0.getValue();
	double b = a / tan( asymptoticAngle.getValue() );

	double A = ( ( b * b ) * ( objectRay.direction.x * objectRay.direction.x ) )
				- ( ( objectRay.direction.y * objectRay.direction.y ) * ( a * a ) );
	double B = 2.0 * ( ( (  objectRay.origin.x * objectRay.direction.x ) * ( b * b ) )
						- ( ( objectRay.origin.y * objectRay.direction.y ) * ( a * a ) ) );
	double C = ( ( objectRay.origin.x * objectRay.origin.x ) * ( b * b ) )
			 - ( ( objectRay.origin.y * objectRay.origin.y ) * ( a * a )  )
			 - ( ( a * a ) * ( b * b ) );

	// Solve quadratic equation for _t_ values
	double t0, t1;
	if( !tgf::Quadratic( A, B, C, &t0, &t1 ) ) return false;

	// Compute intersection distance along ray
	if( t0 > objectRay.maxt || t1 < objectRay.mint ) return false;
	double thit = ( t0 > objectRay.mint )? t0 : t1 ;
	if( thit > objectRay.maxt ) return false;

	//Evaluate Tolerance
	double tol = 0.000001;

	// Compute ShapeSphere hit position and $\phi$
	Point3D hitPoint = objectRay( thit );

	double xMin = sqrt( a * a *
							( 1 + ( ( truncation.getValue() * truncation.getValue() )
									/ ( b* b ) ) ) );
	double xMax = sqrt( a * a * ( 1 +
				( ( theoreticalHeight.getValue() * theoreticalHeight.getValue() )
						/ ( b * b ) ) ) );

	// Test intersection against clipping parameters
	double m =  ( zLengthXMax.getValue() / 2- zLengthXMin.getValue() / 2 ) / ( xMax - xMin );
	double zmax = ( zLengthXMin.getValue()  / 2 ) + m * ( hitPoint.x - xMin );
	double zmin = - zmax;


	// Test intersection against clipping parameters
	if( (thit - objectRay.mint) < tol
			|| hitPoint.x < xMin || hitPoint.x > xMax
			|| hitPoint.y < 0 || hitPoint.y > theoreticalHeight.getValue()
			|| hitPoint.z < zmin ||  hitPoint.z > zmax )
	{
		if ( thit == t1 ) return false;
		if ( t1 > objectRay.maxt ) return false;
		thit = t1;

		// Compute ShapeSphere hit position and $\phi$
		hitPoint = objectRay( thit );
		zmax = ( zLengthXMin.getValue() * 0.5 ) + m * ( hitPoint.x - xMin );
		zmin = - zmax;

		if( (thit - objectRay.mint) < tol
				|| hitPoint.x < xMin || hitPoint.x > xMax
				|| hitPoint.y < 0 || hitPoint.y > theoreticalHeight.getValue()
				|| hitPoint.z < zmin ||  hitPoint.z > zmax )	return false;
	}
	// Now check if the fucntion is being called from IntersectP,
	// in which case the pointers tHit and dg are 0
	if( ( tHit == 0 ) && ( dg == 0 ) ) return true;
	else if( ( tHit == 0 ) || ( dg == 0 ) ) tgf::SevereError( "Function ShapeSphere::Intersect(...) called with null pointers" );


	// Find parametric representation of CPC concentrator hit
	double u = ( hitPoint.x - xMin  ) / ( xMax - xMin );
	double v = ( ( hitPoint.z / zmax ) + 1 )/ 2;


	std::cout<<"--"<<objectRay<<std::endl;
	std::cout<<"--> "<<hitPoint<<std::endl;
	std::cout<<"--> "<<u<< " -- " <<v<<std::endl;

	// Compute  \dpdu and \dpdv
	Vector3D dpdu = GetDPDU( u, v );
	Vector3D dpdv = GetDPDV( u, v );

	// Compute cylinder \dndu and \dndv
	double tanAngle = tan( asymptoticAngle.getValue() );
	double h = theoreticalHeight.getValue();
	double t = truncation.getValue();
	double cotAngle = 1 / tanAngle;
	double aux1 = sqrt( a * a * (1 + ( ( h * h * tanAngle * tanAngle )/ ( a * a ) ) ) );
	double aux2 = sqrt( a * a * (1 + ( ( t * t * tanAngle * tanAngle )/ ( a * a ) ) ) );
	double aux = aux1 - aux2;

	double d2PduuY = - ( ( cotAngle * cotAngle * cotAngle * cotAngle * aux * aux * ( aux2 + u * aux ) )
						/ pow( (a0.getValue() * a0.getValue() * cotAngle * cotAngle *
								( -1 + ( ( ( a0.getValue() + u * aux ) * ( a0.getValue() + u * aux ) )
										/ ( a0.getValue() * a0.getValue() ) ) ) ), 3.0/ 2 ) )
					+ ( ( cotAngle * cotAngle * aux * aux )
						/ sqrt( a0.getValue() * a0.getValue() * cotAngle * cotAngle *
								( -1 + ( ( ( a0.getValue() + u * aux ) * ( a0.getValue() + u * aux ) )
										/ ( a0.getValue() * a0.getValue() ) ) ) ) );
	Vector3D d2Pduu(0 , d2PduuY, 0);
	Vector3D d2Pduv( 0.0, 0.0, 2 * ( -0.5 * zLengthXMin.getValue() + 0.5 * zLengthXMax.getValue() ) );
	Vector3D d2Pdvv( 0.0, 0.0, 0.0 );

	// Compute coefficients for fundamental forms
	double E = DotProduct( dpdu, dpdu );
	double F = DotProduct( dpdu, dpdv );
	double G = DotProduct( dpdv, dpdv );
	Vector3D N = Normalize( CrossProduct( dpdu, dpdv ) );

	std::cout<<"-->N: "<<N<<std::endl;
	double e = DotProduct( N, d2Pduu );
	double f = DotProduct( N, d2Pduv );
	double g = DotProduct( N, d2Pdvv );

		// Compute \dndu and \dndv from fundamental form coefficients
	double invEGF2 = 1.0 / (E*G - F*F);
	Vector3D dndu = (f*F - e*G) * invEGF2 * dpdu +
			        (e*F - f*E) * invEGF2 * dpdv;
	Vector3D dndv = (g*F - f*G) * invEGF2 * dpdu +
	                (f*F - g*E) * invEGF2 * dpdv;

	// Initialize _DifferentialGeometry_ from parametric information
	*dg = DifferentialGeometry( hitPoint ,
		                        dpdv,
								dpdu,
		                        dndu,
								dndv,
		                        u, v, this );

    // Update _tHit_ for quadric intersection
    *tHit = thit;

	return true;
}

bool ShapeTroughHyperbole::IntersectP( const Ray& objectRay ) const
{
	return Intersect( objectRay, 0, 0 );
}

Point3D ShapeTroughHyperbole::Sample( double u, double v ) const
{
	return GetPoint3D( u, v );
}

void ShapeTroughHyperbole::updateAngleValue( void *data, SoSensor *)
{
	ShapeTroughHyperbole* shape = (ShapeTroughHyperbole *) data;
	if( ( shape->asymptoticAngle.getValue() < 0 ) ||
			( shape->asymptoticAngle.getValue() >=  ( 0.5 * tgc::Pi ) ) )
	{
		QMessageBox::warning( 0, QString( "Tonatiuh" ), QString( "Hyperbole Trough angle value must take values on the (0, Pi/2 ) range. ") );
		shape->asymptoticAngle.setValue( shape->m_lastAsymptoticAngleValue );
	}
	else
		shape->m_lastAsymptoticAngleValue = shape->asymptoticAngle.getValue();
}

void ShapeTroughHyperbole::updateApertureValue( void *data, SoSensor *)
{
	ShapeTroughHyperbole* shape = (ShapeTroughHyperbole *) data;
	if( shape->a0.getValue() < 0 )
	{
		QMessageBox::warning( 0, QString( "Tonatiuh" ), QString( "Hyperbole Trough a0 must take positive value. ") );
		shape->a0.setValue( shape->m_lastA0Value );
	}
	else
		shape->m_lastA0Value = shape->a0.getValue();
}


void ShapeTroughHyperbole::updateLengthValues( void *data, SoSensor *)
{
	ShapeTroughHyperbole* shape = (ShapeTroughHyperbole *) data;
	if( ( shape->zLengthXMin.getValue() < 0 ) || ( shape->zLengthXMax.getValue() < 0 ) )
	{
		QMessageBox::warning( 0, QString( "Tonatiuh" ), QString( "Hyperbole Trough z length variables must take positive values. ") );
		shape->zLengthXMin.setValue( shape->m_lastZLengthXMinValue );
		shape->zLengthXMax.setValue( shape->m_lastZLengthXMaxValue );
	}
	else
	{
		shape->m_lastZLengthXMinValue = shape->zLengthXMin.getValue();
		shape->m_lastZLengthXMaxValue = shape->zLengthXMax.getValue();
	}
}

void ShapeTroughHyperbole::updateHeightValue( void *data, SoSensor *)
{
	ShapeTroughHyperbole* shape = (ShapeTroughHyperbole *) data;
	if( shape->theoreticalHeight.getValue() < 0 )
	{
		QMessageBox::warning( 0, QString( "Tonatiuh" ), QString( "Hyperbole Trough theoretical height must take positive value. ") );
		shape->theoreticalHeight.setValue( shape->m_lastHeightValue );
	}
	else
		shape->m_lastHeightValue = shape->theoreticalHeight.getValue();
}
void ShapeTroughHyperbole::updateTruncationValue( void *data, SoSensor *)
{
	ShapeTroughHyperbole* shape = (ShapeTroughHyperbole *) data;
	if( ( shape->truncation.getValue() < 0 ) || ( shape->truncation.getValue() >= shape->theoreticalHeight.getValue() ) )
	{
		QMessageBox::warning( 0, QString( "Tonatiuh" ), QString( "Hyperbole Trough truncation must take values on the [0, height ) range. ") );
		shape->truncation.setValue( shape->m_lastTruncationValue );
	}
	else
		shape->m_lastTruncationValue = shape->truncation.getValue();
}


Point3D ShapeTroughHyperbole::GetPoint3D( double u, double v ) const
{
	if ( OutOfRange( u, v ) ) tgf::SevereError( "Function Poligon::GetPoint3D called with invalid parameters" );

	double a = a0.getValue();
	double b = a / tan( asymptoticAngle.getValue() );

	double xmin =  sqrt( a * a * ( 1 +
			( ( truncation.getValue() * truncation.getValue() )
					/ ( b* b ) ) ) );
	double xmax = sqrt( a * a * ( 1 +
			( ( theoreticalHeight.getValue() * theoreticalHeight.getValue() )
					/ ( b* b ) ) ) );
	double x = u * ( xmax - xmin ) + xmin;
	double y = sqrt( ( ( ( x * x ) / ( a * a ) ) - 1 ) * b * b );
	double m = ( 0.5 * zLengthXMax.getValue() - 0.5 * zLengthXMin.getValue() )
			/ ( xmax - xmin);
	double zMax = ( 0.5 * zLengthXMin.getValue() ) + m * ( x - xmin );
	double z = zMax * ( 2 * v - 1 );
	return Point3D (x, y, z);

}

NormalVector ShapeTroughHyperbole::GetNormal (double u ,double v) const
{
	Vector3D dpdu = GetDPDU( u, v );
	Vector3D dpdv = GetDPDV( u, v );

	return Normalize( NormalVector( CrossProduct( dpdu, dpdv ) ) );

}

bool ShapeTroughHyperbole::OutOfRange( double u, double v ) const
{
	return ( ( u < 0.0 ) || ( u > 1.0 ) || ( v < 0.0 ) || ( v > 1.0 ) );
}

void ShapeTroughHyperbole::computeBBox(SoAction*, SbBox3f& box, SbVec3f& /*center*/)
{
	double a = a0.getValue();
	double b = a / tan( asymptoticAngle.getValue() );

	double xMin =  sqrt( a * a * ( 1 +
			( ( truncation.getValue() * truncation.getValue() )
					/ ( b* b ) ) ) );
	double xMax = sqrt( a * a * ( 1 +
			( ( theoreticalHeight.getValue() * theoreticalHeight.getValue() )
					/ ( b* b ) ) ) );

	double yMin = truncation.getValue();
	double yMax = theoreticalHeight.getValue();

	double zLengthMax = std::max( zLengthXMin.getValue(), zLengthXMax.getValue() );
	double zMin = - zLengthMax /2;
	double zMax = zLengthMax /2;
	box.setBounds(SbVec3f( xMin, yMin, zMin ), SbVec3f( xMax, yMax, zMax ) );
}

void ShapeTroughHyperbole::generatePrimitives(SoAction *action)
{
    SoPrimitiveVertex   pv;
    SoState  *state = action->getState();

    SbBool useTexFunc = ( SoTextureCoordinateElement::getType(state) ==
                          SoTextureCoordinateElement::FUNCTION );

    const SoTextureCoordinateElement* tce = 0;
    if ( useTexFunc ) tce = SoTextureCoordinateElement::getInstance(state);

	const int rows = 20; // Number of points per row
    const int columns = 10; // Number of points per column
    const int totalPoints = (rows)*(columns); // Total points in the grid

    float vertex[totalPoints][6];

    int h = 0;
    double ui = 0;
	double vj = 0;

    for ( int i = 0; i < rows; ++i )
    {
    	ui =( 1.0 /(double)(rows-1) ) * i;

    	for ( int j = 0 ; j < columns ; ++j )
    	{

    		vj = ( 1.0 /(double)(columns-1) ) * j;

    		Point3D point = GetPoint3D(ui, vj);
    		NormalVector normal = GetNormal(ui, vj);

    		vertex[h][0] = point.x;
    		vertex[h][1] = point.y;
    		vertex[h][2] = point.z;
    		vertex[h][3] = normal.x;
    		vertex[h][4] = normal.y;
    		vertex[h][5] = normal.z;

    		pv.setPoint( vertex[h][0], vertex[h][1], vertex[h][2] );
    		h++; //Increase h to the next point.
    	}
    }

	const int totalIndices  = (rows-1)*(columns-1)*4;
    int32_t* indices = new int32_t[totalIndices];
    int k = 0;
    for( int irow = 0; irow < (rows-1); ++irow )
           for( int icolumn = 0; icolumn < (columns-1); ++icolumn )
           {
				indices[k] = irow*columns + icolumn;
				indices[k+1] = indices[k] + 1;
				indices[k+3] = indices[k] + columns;
				indices[k+2] = indices[k+3] + 1;

				k+=4; //Set k to the first point of the next face.
           }

    float finalvertex[totalIndices][6];
    for( int ivert = 0; ivert<totalIndices; ++ivert )
    {
    	finalvertex[ivert][0] = vertex[indices[ivert]][0];
    	finalvertex[ivert][1] = vertex[indices[ivert]][1];
    	finalvertex[ivert][2] = vertex[indices[ivert]][2];
    	finalvertex[ivert][3] = vertex[indices[ivert]][3];
    	finalvertex[ivert][4] = vertex[indices[ivert]][4];
    	finalvertex[ivert][5] = vertex[indices[ivert]][5];
    }
    delete[] indices;

    float u = 1;
    float v = 1;

	beginShape(action, QUADS);
    for( int i = 0; i < totalIndices; ++i )
    {
    	SbVec3f  point( finalvertex[i][0], finalvertex[i][1],  finalvertex[i][2] );
    	SbVec3f normal(-finalvertex[i][3],-finalvertex[i][4], -finalvertex[i][5] );
		SbVec4f texCoord = useTexFunc ? tce->get(point, normal): SbVec4f( u,v, 0.0, 1.0 );

		pv.setPoint(point);
		pv.setNormal(normal);
		pv.setTextureCoords(texCoord);
		shapeVertex(&pv);
    }
    endShape();
}


Vector3D ShapeTroughHyperbole::GetDPDU( double u, double v ) const
{

	double a = a0.getValue();
	double tanAngle = tan( asymptoticAngle.getValue() );
	double h = theoreticalHeight.getValue();
	double t = truncation.getValue();
	double cotAngle = 1 / tanAngle;
	double aux1 = sqrt( a * a * (1 + ( ( h * h * tanAngle * tanAngle )/ ( a * a ) ) ) );
	double aux2 = sqrt( a * a * (1 + ( ( t * t * tanAngle * tanAngle )/ ( a * a ) ) ) );
	double x = aux1 - aux2;


	double y = ( cotAngle * cotAngle * x * ( aux2 + u * x ) )
			/ sqrt( a * a * cotAngle * cotAngle * ( -1 + ( ( ( aux2 + u * x )  * ( aux2 + u * x ) ) / ( a * a ) ) ) );


	double z = (-0.5 * zLengthXMin.getValue() + 0.5 * zLengthXMax.getValue() ) * ( -1 + 2 * v );


	return Vector3D( x, y, z );


}

Vector3D ShapeTroughHyperbole::GetDPDV ( double u, double v ) const
{
	return Vector3D( 0.0, 0.0, 2 * ( 0.5 * zLengthXMin.getValue()
											+ ( -0.5 * zLengthXMin.getValue() + 0.5 * zLengthXMax.getValue() ) *  u ) );
}