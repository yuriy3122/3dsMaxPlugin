#ifndef __MAXEXPORT__H
#define __MAXEXPORT__H

#include "iskin.h"
#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "stdmat.h"
#include "decomp.h"
#include "shape.h"
#include "interpik.h"
#include "maxtextfile.h"
#include "ExpDefines.h"
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <iterator>
#include <functional>

#define VERSION	200							// Version number * 100
#define CFGFILENAME _T("3DSMAXEXPORT.CFG")	// Configuration file

extern ClassDesc* GetMaxExportDesc();
extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

struct VertexPosition {
	Point3 pos;
	Point3 normal;
	Point3 tangent;
	Point3 bitangent;
	Point3 tex;
	Point4 boneWeights;

	bool operator==(const VertexPosition& other) const {
		return pos == other.pos && normal == other.normal && tex == other.tex;
	}
};

//based on a mix function originally proposed by Bob Jenkins
inline void hash_combine(std::size_t& seed, std::size_t value) {
	// 0x9e3779b9 = 2,654,435,769 This is the fractional part of the golden ratio multiplied by 2 ^ 32. 
	// It’s used to spread entropy across bits
	seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std {
	template <>
	struct hash<Point3> {
		std::size_t operator()(const Point3& v) const {
			std::size_t h1 = std::hash<float>{}(v.x);
			std::size_t h2 = std::hash<float>{}(v.y);
			std::size_t h3 = std::hash<float>{}(v.z);
			std::size_t seed = h1;
			hash_combine(seed, h2);
			hash_combine(seed, h3);

			return seed;
		}
	};

	template <>
	struct hash<VertexPosition> {
		std::size_t operator()(const VertexPosition& v) const {
			std::size_t seed = std::hash<Point3>{}(v.pos);
			hash_combine(seed, std::hash<Point3>{}(v.normal));
			hash_combine(seed, std::hash<Point3>{}(v.tex));

			return seed;
		}
	};
}

class MaxExport : public SceneExport {
public:
	MaxExport();
	~MaxExport();

	int    ExtCount();				  // Number of extensions supported 
	const TCHAR * Ext(int n);		  // Extension #n (i.e. "RZ")
	const TCHAR * LongDesc();		  // Long 3DS MAX EXPORT description (i.e. "3DS MAX EXPORT") 
	const TCHAR * ShortDesc();		  // Short 3DS MAX EXPORT description (i.e. "3DS MAX EXPORT")
	const TCHAR * AuthorName();       // 3DS MAX EXPORT Author name
	const TCHAR * CopyrightMessage(); // 3DS MAX EXPORT Copyright message 
	const TCHAR * OtherMessage1();    // Other message #1
	const TCHAR * OtherMessage2();    // Other message #2
	unsigned int Version();			  // Version number * 100 (i.e. v3.01 = 301) 
	void	ShowAbout(HWND hWnd);	  // Show DLL's "About..." box
	int		DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0); // Export file
	BOOL	SupportsOptions(int ext, DWORD options);

private:
	Interface* ip_;
};

class MeshExporter : public ITreeEnumProc {
public:
	// SubExporters
	void PrepareVerts(TriObject* TObj, INode* node, ISkinContextData* skinData);
	void ExportFaces();
	void ExportVerts();
	void ExportMaterial(INode* node);

	// Main functions
	int callback(INode* node);
	void ProcNode(INode* node);

private:
	TriObject* GetTriObjFromNode(INode* node);
	Modifier* GetModifierFromNode(INode* node, Class_ID classId);

	Point3 GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
	Point3 GetVertexTangent(Mesh* mesh, int faceNo);
	Point3 GetVertexBitangent(Mesh* mesh, int faceNo);
	DWORD GetEqualVertex(const VertexPosition& vp);

	void ExportStdMaterial(Mtl* material);
	void ExportDefaultMaterial(DWORD col);
	void ExportAnimationKeys(INode* node, ISkin* iskin);
	
	std::vector<VertexPosition> vertices_;
	std::vector<DWORD> indices_;
	std::vector<int> mtlsIndices_;
	std::unordered_map<VertexPosition, DWORD> vertexHashes_;
};

#endif // __MAXEXPORT__H