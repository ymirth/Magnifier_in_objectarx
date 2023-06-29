#ifndef MAGNIFIER_H
#define MAGNIFIER_H
#pragma once
#include<vector>
class Magnifier : public AcDbEntity 
{
public:
	ACRX_DECLARE_MEMBERS(Magnifier);

protected:
	static Adesk::UInt32 kCurrentVersionNumber;

public:
	Magnifier();
	explicit Magnifier(std::vector<AcDbObjectId> idList, const AcDbExtents& E,
		const AcGePoint3d& sp) {
		Ext = E;
		StartPoint = sp;
		mId = idList;
	}
	explicit Magnifier(std::vector<AcDbObjectId> idList,const AcDbExtents& E,
		const AcGePoint3d& sp, const AcGePoint3d& ep) {
		Ext = E;
		StartPoint = sp;
		EndPoint = ep;
		mId = idList;
	}
	virtual ~Magnifier();

	//----- AcDbObject protocols
	//---- Dwg Filing protocol
	virtual Acad::ErrorStatus dwgOutFields (AcDbDwgFiler *pFiler) const;
	virtual Acad::ErrorStatus dwgInFields (AcDbDwgFiler *pFiler);

	//----- AcDbEntity protocols
	//----- Graphics protocol
	Acad::ErrorStatus setDest(const AcGePoint3d& ep);
protected:
	virtual Adesk::Boolean subWorldDraw (AcGiWorldDraw *mode);
	virtual Adesk::UInt32 subSetAttributes (AcGiDrawableTraits *traits);
	Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform);
private:
	double scaleFactor = 4.0; // 放大或缩小的比例因子
	AcDbExtents Ext;
	std::vector<AcDbObjectId> mId;
	AcGePoint3d EndPoint; // 平移的目标点
	AcGePoint3d StartPoint; // 平移的起始点
};

#endif
