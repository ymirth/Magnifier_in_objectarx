#include "pch.h"
#include"dbapserv.h"
#include"aced.h"
#include"Rect.h"
#include"SelectionJig.h"
#include"Magnifier.h"
#include<vector>


void createMagnify(std::vector<AcDbObjectId> idList,const AcDbExtents& Ext)
{
	auto pExt = &Ext;
	ads_point sp;
	ads_point ep;
	int flag1 = acedGetPoint(NULL, _T("\nSpecify the endPoint of circle: "), ep);
	AcGePoint3d destination = asPnt3d(ep);		// 圆弧圆心
	AcGePoint3d source{
		(pExt->minPoint().x + pExt->maxPoint().x) / 2,
		(pExt->minPoint().y + pExt->maxPoint().y) / 2,
		0.0 };
	// 创建 MyEntity 对象
	Magnifier* pEntity = new Magnifier(idList, Ext, source, destination);

	// 将对象添加到当前空间（Model Space）
	AcDbBlockTable* pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);

	AcDbBlockTableRecord* pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);

	pBlockTableRecord->appendAcDbEntity(pEntity);
	pEntity->close();

	pBlockTableRecord->close();
	pBlockTable->close();

	// 释放内存

	acutPrintf(_T("\nHello world!"));
}


std::vector<AcDbObjectId> getID(AcDbExtents Ext) {
	auto pExt = &Ext;
	std::vector<AcDbObjectId> m_Ids;
	ads_name ss;
	acedSSGet(_T("C"), asDblArray(pExt->minPoint()), asDblArray(pExt->maxPoint()), NULL, ss);
	Adesk::Int32 len = 0;
	acedSSLength(ss, &len);
	for (auto i = 0; i < len; i++) {
		//if (i == 0) continue;
		ads_name en;
		acedSSName(ss, i, en);
		AcDbObjectId objId;
		acdbGetObjectId(objId, en);
		m_Ids.push_back(objId);
	}
	return m_Ids;
}

AcDbObjectId createEnt() {
	//Entity
	auto pEntity = new Rect();
	auto pJig = new SelectionJig();
	AcDbExtents ext;
	AcDbObjectId Id;
	if (pJig->startJig(pEntity) == AcEdJig::kNormal) {
		Id = pEntity->objectId();	
		pEntity->getGeomExtents(ext);
		pEntity->close();
	}
	else {
		delete pEntity;
		pEntity = nullptr;
	}
	delete pJig;
	pJig = nullptr;

	auto mId = getID(ext);
	createMagnify(mId, ext);
	return Id;
}



void helloworld()
{
	createEnt();
	acutPrintf(_T("\nHello world!"));
}


int showhello(struct resbuf *rb)
{
	ads_printf(_T("hello"));
	ads_retvoid();
	return RTNORM;
}

int showhellocmd(struct resbuf *rb)
{
	ads_printf(_T("hello"));
	ads_retvoid();
	return RTNORM;
}
