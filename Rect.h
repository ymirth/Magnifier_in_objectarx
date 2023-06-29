#ifndef RECT_H
#define RECT_H
#pragma once

class Rect : public AcDbEntity
{
public:
	ACRX_DECLARE_MEMBERS(Rect);
public:
	enum PartialUndoCode
	{
		kStart,
		kEnd
	};
protected:
	static Adesk::UInt32 kCurrentVersionNumber;

public:
	Rect();
	virtual ~Rect();

	//----- AcDbObject protocols
	//---- Dwg Filing protocol
	virtual Acad::ErrorStatus dwgOutFields (AcDbDwgFiler *pFiler) const;
	virtual Acad::ErrorStatus dwgInFields (AcDbDwgFiler *pFiler);

	//----- AcDbEntity protocols
	//----- Graphics protocol

	Acad::ErrorStatus setStart(const AcGePoint3d&);
	Acad::ErrorStatus setEnd(const AcGePoint3d&);

	Acad::ErrorStatus applyPartialUndo(AcDbDwgFiler* undoFiler, AcRxClass* classObj);
protected:
	virtual Adesk::Boolean subWorldDraw (AcGiWorldDraw *mode);
	virtual Adesk::UInt32 subSetAttributes (AcGiDrawableTraits *traits);
	Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform);
private:
	//AcGePoint3d polygonPoints[4];
	// 定义全局变量来保存选择框的起点和终点
	AcGePoint3d	StartPoint;
	AcGePoint3d EndPoint;
};

#endif
