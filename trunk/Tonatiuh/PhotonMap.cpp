/***************************************************************************
    Copyright (C) 2007 by Manuel J. Blanco, Amaia Mutuberria,             
                          Victor Martin, Javier Garcia-Barberena,         
 			   			   Inaki Perez, Inigo Pagola					   
                                    					  			 	   
    mblanco@cener.com                                                     
                                                                          
    This program is free software; you can redistribute it and/or modify  
    it under the terms of the GNU General Public License as published by  
    the Free Software Foundation; either version 3 of the License, or     
    (at your option) any later version.                                   
                                                                          
    This program is distributed in the hope that it will be useful,       
    but WITHOUT ANY WARRANTY; without even the implied warranty of        
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
    GNU General Public License for more details.                          
                                                                          
    You should have received a copy of the GNU General Public License     
    along with this program; if not, write to the                         
    Free Software Foundation, Inc.,                                       
    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.		   	  
 									  									  	 
    Acknowledgments:							   						  
 									   									  
    The development of Tonatiuh was supported from May 2004 to October    
    2006 by the Department of Energy (DOE) and the National Renewable     
    Energy Laboratory (NREL) under the Minority Research Associate (MURA) 
    Program Subcontract ACQ-4-33623-06 with the University of Texas at    
    Brownsville (UTB).							   						  
 															   			  
    Since June 2006, the development of Tonatiuh is supported by	   	   
    the Spanish National Renewable Energy Centre (CENER), which has 	   
    established a permanent Tonatiuh software development team, under 	   
    the direction of Dr. Manuel J. Blanco, Director of CENER's Solar 	   
    Thermal Energy Department, Full-Professor of UTB, and former 	   	   
    Principal Investigator of the MURA Program Subcontract ACQ-4-33623-06.
 									   									   
    Support for the validation of Tonatiuh is also provided by NREL as	   
    in-kind contribution under the framework of the Memorandum 	   	   
    of Understanding between NREL and CENER (MOU#NREL-07-117) signed on   
    February, 20, 2007.						   						   							   									   
 ***************************************************************************/
 
#include "PhotonMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tgf.h"
#include "Trace.h"


#ifdef __mips
#include <alloca.h>
#endif

#include <sys/stat.h>

/* This is the constructor for the photon map.
 * To create the photon map it is necessary to specify the
 * maximum number of photons that will be stored
*/

PhotonMap :: PhotonMap( long unsigned maxPhotons )
{
    m_storedPhotons = 0;
	m_prevScale = 1;
	m_maxPhotons = maxPhotons;
	m_axis = 0;
	m_photons = new Photon*[m_maxPhotons+1];
	
	if (!m_photons)  tgf::SevereError( "Out of memory initializing photon map" );
	
	BBox m_bbox();

}

PhotonMap :: ~PhotonMap()
{
	Trace trace( "PhotonMap::~PhotonMap", false );
	for( long unsigned i = 1; i <= m_storedPhotons; i++) 
	{
		delete m_photons[i];
	}
	delete[] m_photons ;
	m_photons = 0;
}

void PhotonMap::savePhotonMap( const char *filename )
{
	FILE *fp=fopen(filename,"wb");
	fwrite(m_photons,sizeof(Photon),m_storedPhotons,fp);
	fclose(fp);
}

void PhotonMap::loadPhotonMap( char *filename )
{
	FILE *fp;
	fp=fopen(filename,"rb");
	assert(fp);
struct stat sbuf;
	m_storedPhotons = sbuf.st_size/sizeof(Photon);
	m_photons = new Photon*[m_maxPhotons+1];
	int count = fread(m_photons,sizeof(Photon),m_storedPhotons,fp);
	fclose(fp);
	if(count != m_storedPhotons)
	{
		std::cout << "PhotonMap::loadPhotonMap count != m_storePhotons" << std::endl;	
	}
	m_halfStoredPhotons = m_storedPhotons/2-1;
}

void PhotonMap::locatePhotons( NearestPhotons* const np, const long unsigned index ) const
{
    Photon* p = m_photons[index];
	double dist1;
	
    if ( index < m_halfStoredPhotons)
    {
	    dist1 = np->m_pos[ p->m_plane ] - p->m_pos[ p->m_plane ];
	    if ( dist1>0.0 )
	    {
	        locatePhotons( np, 2*index+1 );
	        if ( dist1*dist1 < np->m_dist2[0] ) locatePhotons( np, 2*index );
	    }
	    else
	    {
	        locatePhotons( np, 2*index );
	        if ( dist1*dist1 < np->m_dist2[0] ) locatePhotons( np, 2*index+1 );
	    }
	}

	double dist2 = DistanceSquared(p->m_pos, np->m_pos);
	
	if ( dist2 < np->m_dist2[0] )
	{
	    if ( np->m_found < np->m_max )
	    {
	        np->m_found++;
	        np->m_dist2[np->m_found] = dist2;
	        np->m_index[np->m_found] = p;
	    }
	    else
	    {
	        long unsigned j,parent;
            if ( np->m_gotHeap==0 )
            {
	            double dst2;
	            Photon* phot;
	            long unsigned halfFound = np->m_found>>1;
	            for ( long unsigned k = halfFound; k >= 1; k-- )
	            {
	                parent = k;
	                phot = np->m_index[k];
	                dst2 = np->m_dist2[k];
	                while ( parent <= halfFound )
	                {
	                    j = parent + parent;
	                    if ( j < np->m_found && np->m_dist2[j] < np->m_dist2[j+1] )
	                    j++;
	                    if ( dst2 >= np->m_dist2[j] ) break;
	                    np->m_dist2[parent] = np->m_dist2[j];
	                    np->m_index[parent] = np->m_index[j];
	                    parent = j;
	                }
	                np->m_dist2[parent] = dst2;
	                np->m_index[parent] = phot;
	            }
	            np->m_gotHeap = 1;
	        }
	
	        // insert new photon into max heap
	        // delete largest element, insert new and reorder the heap
	
	        parent=1;
	        j = 2;
	        while ( j <= np->m_found )
	        {
	            if ( j < np->m_found && np->m_dist2[j] < np->m_dist2[j+1] )
	            j++;
	            if ( dist2 > np->m_dist2[j] ) break;
	            np->m_dist2[parent] = np->m_dist2[j];
	            np->m_index[parent] = np->m_index[j];
	            parent = j;
	            j += j;
	        }
	        np->m_index[parent] = p;
	        np->m_dist2[parent] = dist2;
	        np->m_dist2[0] = np->m_dist2[1];
	    }
    }
}

double PhotonMap::fluxAtPoint( const Point3D& point, int maxClosest ) const
{
	NearestPhotons np;
	//np.m_index = new Photon[maxClosest][];
	np.m_dist2 = (double*)malloc( sizeof(double)*(maxClosest+1) );
	np.m_index = (Photon**)malloc( sizeof(Photon*)*(maxClosest+1) );
	np.m_max = maxClosest;
	np.m_pos = point;
	np.m_found = 0;
	np.m_gotHeap = 0;
	np.m_dist2[0] = 10;
	
	locatePhotons(&np, 1);
	Point3D furthest = np.m_index[np.m_found]->m_pos;
	double radius = Distance(point, furthest);
	free(np.m_dist2);
    free(np.m_index);
	return maxClosest / radius;
}

void PhotonMap :: store( Photon* photon )
{
	if ( m_storedPhotons >= m_maxPhotons ) return;
	
  	m_storedPhotons++;
  	photon->m_id = m_storedPhotons;
  	m_photons[m_storedPhotons] = photon;
  	
	m_bbox = Union( m_bbox,photon->m_pos );
}

// balance creates a left balanced kd-tree from the flat photon array.
// * This function should be called before the photon map is used for rendering.
void PhotonMap::balance()
{
    if ( m_storedPhotons > 1 )
	{
        // allocate two temporary arrays for the balancing procedure
		Photon*** pa1 = new Photon**[m_maxPhotons+1];
		Photon*** pa2 = new Photon**[m_maxPhotons+1];
	
		long unsigned i;
		for ( i = 0; i <= m_storedPhotons; i++ ) pa2[i] = &m_photons[i];
	   	balanceSegment( pa1, pa2, 1, 1, m_storedPhotons );
		
		delete[] pa2;
        pa2 = 0;
	
	    // reorganize balanced kd-tree (make a heap)
	    long unsigned d;
	    long unsigned j = 1;
	    long unsigned foo = 1;
	    Photon* foo_photon = m_photons[j];
	    for ( i=1; i <= m_storedPhotons; i++ )
	    {	
		    d = pa1[j]-m_photons;
		    pa1[j] = 0;
				
		    if ( d != foo ) m_photons[j] = m_photons[d];
		    else
		    {
		        m_photons[j] = foo_photon;
		        if ( i < m_storedPhotons )
		        {
					for ( ; foo<= m_storedPhotons; foo++ ) if ( pa1[foo] != 0 ) break;
					foo_photon = m_photons[foo];
		          	j = foo;
		        }
		        continue;
		    }
		    j = d;
	    }
	    delete[] pa1;
        pa1 = 0;
    }
	m_halfStoredPhotons = m_storedPhotons / 2 - 1;
}

#define swap(ph,a,b) { Photon** ph2=ph[a]; ph[a]=ph[b]; ph[b]=ph2; }

// median_split splits the photon array into two separate
// pieces around the median with all photons below the
// the median in the lower half and all photons above
// than the median in the upper half. The comparison
// criteria is the axis (indicated by the axis parameter)
// (inspired by routine in "Algorithms in C++" by Sedgewick)
void PhotonMap::medianSplit( Photon*** p, const long unsigned start, const long unsigned end, 
                             const long unsigned median, const long unsigned axis )         
{

	long unsigned left = start;
    long unsigned right = end;

    while ( right > left )
    {
        const double v = p[right][0]->m_pos[axis];
        long unsigned i = left-1;
        long unsigned j = right;
        for (;;)
        {
            while ( p[++i][0]->m_pos[axis]< v && i < right );
            while ( p[--j][0]->m_pos[axis] > v && j>left );
            if ( i >= j ) break;
            swap(p,i,j);
        }
        swap(p,i,right);
        if ( i >= median ) right=i-1;
        if ( i <= median ) left=i+1;
    }
}

void PhotonMap::balanceSegment( Photon*** pbal, Photon*** porg, const long unsigned index, const long unsigned start, const long unsigned end )
{
    long unsigned median=1;
  	while ( ( 4*median ) <= ( end-start+1 ) ) median += median;

	if ( (3*median ) <= ( end-start+1 ) )
	{
	    median += median;
		median += start-1;
	}
	else median = end - median+1;

    //Find axis to split along
  	long unsigned axis=2;
  	if ( ( m_bbox.pMax[0] - m_bbox.pMin[0] ) > ( m_bbox.pMax[1] - m_bbox.pMin[1] ) &&
		 ( m_bbox.pMax[0] - m_bbox.pMin[0] ) > ( m_bbox.pMax[2]-m_bbox.pMin[2] ) ) axis = 0;
 	else if ( ( m_bbox.pMax[1] - m_bbox.pMin[1] ) > ( m_bbox.pMax[2] - m_bbox.pMin[2] ) ) axis = 1;

	m_axis = axis;
	
    //Partition photon block around the median
  	medianSplit( porg, start, end, median, axis );
	porg[ median ][0]->m_plane = axis;
  	pbal[ index ] = porg[ median ];

    //Recursively balance the left and right block
    if ( median > start )
    {
        //Balance left segment
        if ( start < median-1 )
        {
            const double tmp = m_bbox.pMax[axis];
            m_bbox.pMax[axis] = pbal[index][0]->m_pos[axis];
            balanceSegment( pbal, porg, 2*index, start, median-1 );
            m_bbox.pMax[axis] = tmp;
        }
        else pbal[ 2*index ] = porg[start];
    }
  	
  	if ( median < end )
  	{
        //Balance right segment
    	if ( median+1 < end )
    	{
      	    const double tmp = m_bbox.pMin[axis];     
     		m_bbox.pMin[axis] = pbal[index][0]->m_pos[axis];
      		balanceSegment( pbal, porg, 2*index+1, median+1, end );
      		m_bbox.pMin[axis] = tmp;
    	} 
    	else pbal[ 2*index+1 ] = porg[end];
    }
}

Photon* PhotonMap::GetPhoton( int index ) const
{
	Trace trace( "PhotonMap::GetPhoton", false );
	return m_photons[index]; 
}
