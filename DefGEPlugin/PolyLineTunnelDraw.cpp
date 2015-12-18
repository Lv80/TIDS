#include "StdAfx.h"
#include "PolyLineTunnelDraw.h"

#include "DrawTool.h"
#include "DrawSpecial.h"

#include <cmath>

ACRX_CONS_DEFINE_MEMBERS ( PolyLineTunnelDraw, LinkedGEDraw, 1 )

PolyLineTunnelDraw::PolyLineTunnelDraw()
{
}

void PolyLineTunnelDraw::setAllExtraParamsToDefault()
{
    m_width = 40;
    jdt = JDT_NULL;
}

void PolyLineTunnelDraw::configExtraParams()
{
    // �л���Դ
    //CAcModuleResourceOverride myResources;
}

void PolyLineTunnelDraw::updateExtraParams()
{
}

void PolyLineTunnelDraw::readExtraParam( DrawParamReader& reader )
{
    reader.readDouble( m_width );

    int i = 0;
    reader.readInt( i );
    jdt = ( JointDrawType )( i % 3 );
}

void PolyLineTunnelDraw::writeExtraParam( DrawParamWriter& writer )
{
    writer.writeDouble( m_width );
    writer.writeInt( ( int )jdt ); // ת��������
}

void PolyLineTunnelDraw::extendByLength( double length )
{
    AcGeVector3d v = m_endPt - m_startPt;
    v.normalize();

    m_endPt = m_endPt + v * length; // ����ĩ������
}

Adesk::Boolean PolyLineTunnelDraw::subWorldDraw( AcGiWorldDraw* mode )
{
    assertReadEnabled();

    //if(m_startPt == m_endPt) // ʼĩ���غ�
    //{
    //	// ������������
    //	DrawShaft(mode, m_startPt, m_width);
    //}
    //else
    //{
    double c = 0.618;
    DrawPolyLine( mode, m_startPt, m_endPt, m_width );
    DrawJoint( mode, m_startPt, m_width * 0.5 * c, jdt ); // ����ʼ�ڵ�
    DrawJoint( mode, m_endPt, m_width * 0.5 * c, jdt ); // ����ĩ�ڵ�
    //}

    return Adesk::kTrue;
}

Acad::ErrorStatus PolyLineTunnelDraw::subTransformBy( const AcGeMatrix3d& xform )
{
    m_startPt.transformBy( xform );
    m_endPt.transformBy( xform );
    updateExtraParams();

    return Acad::eOk;
}

Acad::ErrorStatus PolyLineTunnelDraw::subGetOsnapPoints (
    AcDb::OsnapMode osnapMode,
    Adesk::GsMarker gsSelectionMark,
    const AcGePoint3d& pickPoint,
    const AcGePoint3d& lastPoint,
    const AcGeMatrix3d& viewXform,
    AcGePoint3dArray& snapPoints,
    AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    // ֻ��׽1�����͵ĵ㣺�˵�
    if( osnapMode != AcDb::kOsModeEnd )
        return Acad::eOk;

    Acad::ErrorStatus es = Acad::eOk;

    // �����Ҫ�趨��׽��λ�ã����ܻ���Ҫ����һЩ��׽��
    // �����е㡢2/3����ȵ�
    if ( osnapMode == AcDb::kOsModeEnd )
    {
        snapPoints.append( m_startPt );
        snapPoints.append( m_endPt );
    }

    return es;
}

//- Grip points protocol
Acad::ErrorStatus PolyLineTunnelDraw::subGetGripPoints ( AcGePoint3dArray& gripPoints,
        AcDbIntArray& osnapModes,
        AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    gripPoints.append( m_startPt );

    if( m_startPt == m_endPt )
    {
        AcGePoint3d pt( m_startPt );
        pt.x = pt.x + m_width * 0.3;
        gripPoints.append( pt );
    }
    else
    {
        gripPoints.append( m_endPt );
    }

    return Acad::eOk;
}

Acad::ErrorStatus PolyLineTunnelDraw::subMoveGripPointsAt ( const AcDbIntArray& indices, const AcGeVector3d& offset )
{
    assertWriteEnabled () ;

    for( int i = 0; i < indices.length(); i++ )
    {
        int idx = indices.at( i );

        if ( idx == 0 )
        {
            /*if(m_startPt == m_endPt)
            {
            	m_startPt += offset;
            	m_endPt += offset;
            }
            else
            {*/
            m_startPt += offset;
            //}
        }

        if ( idx == 1 )
        {
            //if(m_startPt == m_endPt) // ʼĩ���غ�
            //{
            //	AcGePoint3d pt(m_startPt);
            //	pt.x = pt.x + m_width*0.3; // �⻷�뾶��60%
            //	pt += offset;
            //	m_endPt = pt;
            //}
            //else
            //{
            m_endPt += offset;
            //}
        }
    }
    return Acad::eOk;
}

Acad::ErrorStatus PolyLineTunnelDraw::subGetGeomExtents( AcDbExtents& extents ) const
{
    assertReadEnabled();

    extents.addPoint( m_startPt );
    extents.addPoint( m_endPt );

    return Acad::eOk;
}