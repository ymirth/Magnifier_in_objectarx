#include "pch.h"
#include "Magnifier.h"
#include"dbapserv.h"
#include"dbproxy.h"
#include"AcGiLineAttributes.h"
//----------------------------------------------------------------------------
Adesk::UInt32 Magnifier::kCurrentVersionNumber = 1 ;

//----------------------------------------------------------------------------
//---- runtime definition
ACRX_DXF_DEFINE_MEMBERS (
	Magnifier, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, MAGNIFIER, MAGNIFIERAPP
)

//----------------------------------------------------------------------------
//---- construct & destruct

Magnifier::Magnifier(){}

Magnifier::~Magnifier(){}

//----------------------------------------------------------------------------
//----- AcDbObject protocols
//---- Dwg Filing protocol
Acad::ErrorStatus Magnifier::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled ();
	Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	if ((es = pFiler->writeUInt32(Magnifier::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus Magnifier::dwgInFields(AcDbDwgFiler * pFiler) {
	assertWriteEnabled();
	Acad::ErrorStatus es = AcDbEntity::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > Magnifier::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//if ( version < Magnifier::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	return (pFiler->filerStatus());
}

//----------------------------------------------------------------------------
//----- AcDbEntity protocols


Adesk::Boolean Magnifier::subWorldDraw(AcGiWorldDraw * mode) {
	assertReadEnabled();

	auto pExt = &Ext;
	AcGiWorldGeometry * pGeom = &mode->geometry();
	//pGeom->pushModelTransform(myTransform());

	auto myTransform = [this]() {
		AcGePoint3d point3d; // 要进行放大或缩小的三维点
		
		// 创建放大或缩小的变换矩阵
		AcGeMatrix3d scaleMatrix;
		scaleMatrix.setToScaling(scaleFactor);

		return scaleMatrix;
	};

	mode->geometry().pushModelTransform(AcGeMatrix3d::translation(EndPoint - StartPoint));
	mode->geometry().pushModelTransform(myTransform());
	AcGiClipBoundary cb;
	cb.m_bDrawBoundary = true;
	cb.m_vNormal = AcGeVector3d::kZAxis;
	cb.m_ptPoint = AcGePoint3d::kOrigin;
	// Two points treated as a rectangle, three creates a triangle
	auto pt1 = AcGePoint2d(pExt->minPoint().x, pExt->minPoint().y);
	auto pt2 = AcGePoint2d(pExt->maxPoint().x, pExt->maxPoint().y);
	//放大Transform
	AcGePoint3d SP = StartPoint;
	AcGePoint3d EP = EndPoint;
	SP.transformBy(myTransform());
	EP.transformBy(myTransform());
	AcGePoint2d st(SP.x, SP.y);
	AcGePoint2d et(EP.x, EP.y);
	//平移Transform
	pt1.transformBy(AcGeMatrix2d::translation(et - st));
	pt2.transformBy(AcGeMatrix2d::translation(et - st));
	cb.m_aptPoints.append(pt1);
	cb.m_aptPoints.append(pt2);
	// We are clipping in our own space
	cb.m_xToClipSpace.setToIdentity();
	//cb.m_xInverseBlockRefXForm = myTransform().inverse();
	cb.m_xInverseBlockRefXForm = AcGeMatrix3d::translation(EP - SP).inverse();
	// No Z clipping
	cb.m_bClippingBack = cb.m_bClippingFront = false;
	cb.m_dFrontClipZ = cb.m_dBackClipZ = 0.;
	Adesk::Boolean bPopClipBoundary = pGeom->pushClipBoundary(&cb);
	// Draw something
	
	
	for (auto i = 0; i < mId.size(); i++) {
		AcDbObjectId objId = mId[i];
		AcDbEntity* pEnt = nullptr;
		acdbOpenAcDbEntity(pEnt, objId, AcDb::kForRead);
		if (pEnt)
		{
			mode->geometry().draw(pEnt);
			pEnt->close();
		}
		acutPrintf(_T("\nEntity: %d"), i);
	}


	pGeom->popModelTransform();
	if (bPopClipBoundary) { pGeom->popClipBoundary(); }
	return true; // world-only


	return (AcDbEntity::subWorldDraw(mode));
}


Adesk::UInt32 Magnifier::subSetAttributes(AcGiDrawableTraits * traits) {
	assertReadEnabled();
	return (AcDbEntity::subSetAttributes(traits));
}

Acad::ErrorStatus  Magnifier::subTransformBy(const AcGeMatrix3d& xform) {
	assertWriteEnabled();
	StartPoint = StartPoint.transformBy(xform);
	EndPoint = EndPoint.transformBy(xform);
	return Acad::eOk;
}

Acad::ErrorStatus Magnifier::setDest(const AcGePoint3d& ep) {
	this->EndPoint = ep;
	return Acad::eOk;
}