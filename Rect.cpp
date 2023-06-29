#include "pch.h"
#include "Rect.h"
#include "dbapserv.h"
#include"dbproxy.h"
//----------------------------------------------------------------------------
Adesk::UInt32 Rect::kCurrentVersionNumber = 1 ;

//----------------------------------------------------------------------------
//---- runtime definition
ACRX_DXF_DEFINE_MEMBERS (
	Rect, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, RECT, RECTAPP
)

//----------------------------------------------------------------------------
//---- construct & destruct

Rect::Rect(){}

Rect::~Rect(){}

//----------------------------------------------------------------------------
//----- AcDbObject protocols
//---- Dwg Filing protocol
Acad::ErrorStatus Rect::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled ();
	Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	if ((es = pFiler->writeUInt32(Rect::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....
	pFiler->writePoint3d(StartPoint);
	pFiler->writePoint3d(EndPoint);
	return (pFiler->filerStatus());
}

Acad::ErrorStatus Rect::dwgInFields(AcDbDwgFiler * pFiler) {
	assertWriteEnabled();
	Acad::ErrorStatus es = AcDbEntity::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > Rect::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//if ( version < Rect::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....
	pFiler->readPoint3d(&StartPoint);
	pFiler->readPoint3d(&EndPoint);

	return (pFiler->filerStatus());
}

//----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean Rect::subWorldDraw(AcGiWorldDraw * mode) {
	assertReadEnabled();
	int num_pts = 5;
	AcGePoint3d *pVerts = new AcGePoint3d[num_pts];
	pVerts[0] = AcGePoint3d(min(StartPoint.x,EndPoint.x) , min(StartPoint.y, EndPoint.y), 0);
	pVerts[1] = AcGePoint3d(min(StartPoint.x, EndPoint.x), max(StartPoint.y, EndPoint.y), 0);
	pVerts[2] = AcGePoint3d(max(StartPoint.x, EndPoint.x), max(StartPoint.y, EndPoint.y), 0);
	pVerts[3] = AcGePoint3d(max(StartPoint.x, EndPoint.x), min(StartPoint.y, EndPoint.y), 0);
	
	pVerts[4] = AcGePoint3d(min(StartPoint.x, EndPoint.x), min(StartPoint.y, EndPoint.y), 0);
	mode->geometry().polyline(num_pts, pVerts);

	return (AcDbEntity::subWorldDraw(mode));
}


Adesk::UInt32 Rect::subSetAttributes(AcGiDrawableTraits * traits) {
	assertReadEnabled();
	return (AcDbEntity::subSetAttributes(traits));
}

Acad::ErrorStatus Rect::subTransformBy(const AcGeMatrix3d& xform) {
	assertWriteEnabled();
	StartPoint = StartPoint.transformBy(xform);
	EndPoint = EndPoint.transformBy(xform);
	return Acad::eOk;
}

Acad::ErrorStatus Rect::setStart(const AcGePoint3d& start) {
	assertWriteEnabled(false);
	AcDbDwgFiler *pFiler = NULL;
	if ((pFiler = undoFiler()) != NULL) {
		undoFiler()->writeAddress(Rect::desc());
		undoFiler()->writeItem((Adesk::Int16)kStart);
		undoFiler()->writePoint3d(this->StartPoint);
	}
	this->StartPoint = start;
	return Acad::eOk;
}
Acad::ErrorStatus Rect::setEnd(const AcGePoint3d& end) {
	assertWriteEnabled(false);
	AcDbDwgFiler *pFiler = NULL;
	if ((pFiler = undoFiler()) != NULL) {
		undoFiler()->writeAddress(Rect::desc());
		undoFiler()->writeItem((Adesk::Int16)kEnd);
		undoFiler()->writePoint3d(this->EndPoint);
	}
	this->EndPoint = end;
	return Acad::eOk;
}
//undo
Acad::ErrorStatus Rect::applyPartialUndo(AcDbDwgFiler* undoFiler, AcRxClass* classObj)
{
	if (classObj != Rect::desc())
		return AcDbEntity::applyPartialUndo(undoFiler, classObj);
	ads_printf(_T("PARTIAL UNDO"));
	Adesk::Int16 shortCode;
	undoFiler->readItem(&shortCode);
	PartialUndoCode code = (PartialUndoCode)shortCode;
	AcGePoint3d st, et;
	switch (code) {
		case kStart:
			undoFiler->readPoint3d(&st);
			setStart(et);
			break;
		case kEnd:
			undoFiler->readPoint3d(&et);
			setEnd(et);
			break;
		default:
			assert(Adesk::kFalse);
			break;
	}
	return Acad::eOk;
}